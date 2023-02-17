#include "../../include/ReqHandler.hpp"

ReqHandler::ReqHandler(void)
    : req_msg_(NULL), entity_flag_(0), buf_(NULL), client_(0), read_len_(0) {}

ReqHandler::~ReqHandler(void) { Clear(); }

void ReqHandler::SetClient(Data* client) {
  client_ = client;
  if (client_->is_remain == true || client_->is_chunked) {
    req_msg_ = client->req_message_;
  }
}

void ReqHandler::SetReadLen(int64_t kevent_data) { read_len_ = kevent_data; }

void ReqHandler::Clear() {
  if (buf_ != NULL) {
    delete[] buf_;
    buf_ = NULL;
  }
  entity_flag_ = 0;
  read_len_ = 0;
  client_ = NULL;
  req_msg_ = NULL;
}

ssize_t ReqHandler::RecvFromSocket() {
  ssize_t recv_return(0);
  buf_ = new char[sizeof(char) * read_len_];
  recv_return = recv(client_->GetClientFd(), buf_, read_len_, 0);
  if (recv_return == -1) {
#if REQ_HANDLER
    std::cout << "[ReqHandler] recv return -1. socket : "
              << client_->GetClientFd() << std::endl;
#endif
  }

#if REQ_HANDLER
  write(1, buf_, read_len_);
#endif
  return recv_return;
}

int64_t ReqHandler::ParseFirstLine() {  // end_idx = '\n'
  int64_t curr_idx(0), find_idx(0), end_idx(0);
  /* 첫 줄 찾기 */
  find_idx = strcspn(buf_, "\n");  // buf 에서 "\n"의 인덱스 찾는 함수
  while (find_idx != read_len_ && buf_[find_idx - 1] != '\r')
    find_idx += strcspn(&buf_[find_idx + 1], "\n");
  end_idx = find_idx;

  /* 첫 줄 유형 확인 */
  find_idx = 0;

  /* 첫 단어 Method 쪼개기 */
  find_idx = strcspn(buf_, " ");
  char* tmp = new char[end_idx + 1];
  if (find_idx >= end_idx) {
    // client_->SetStatusCode(400); // bad request
    return (read_len_);
  }
  strncpy(tmp, buf_, find_idx);
  tmp[find_idx] = '\0';
  /* 메소드 유효성 확인 필요 */
  req_msg_->method_ = tmp;
  RemoveTabSpace(req_msg_->method_);

  curr_idx += find_idx;
  // 두번 째 URL 쪼개기
  find_idx = strcspn(&buf_[curr_idx + 1], " ");
  if (find_idx >= end_idx) {
    // client_->SetStatusCode(400); // bad request
    return (read_len_);
  }
  strncpy(tmp, &buf_[curr_idx + 1], find_idx);
  tmp[find_idx] = '\0';
  req_msg_->req_url_ = tmp;
  // 중복 slash 제거 EX) ////// -> /
  ReduceSlash(req_msg_->req_url_);

  curr_idx += find_idx;

  /* 버전확인 406 Not Acceptable */
  find_idx = strcspn(&buf_[curr_idx + 1], "\r");
  if (find_idx >= end_idx) {
    client_->SetStatusCode(400);  // bad request
    return (read_len_);
  }
  strncpy(tmp, &buf_[curr_idx + 1], find_idx);
  tmp[find_idx] = '\0';

  req_msg_->protocol_ = tmp;
  RemoveTabSpace(req_msg_->protocol_);
  if (req_msg_->protocol_ != "HTTP/1.1") {
    client_->SetStatusCode(505);  // invalid protocol
#if REQ_HANDLER
    std::cout << "INVALID PROTOCOL" << std::endl;
#endif
    return (read_len_);
  }

  delete[] tmp;
  return (end_idx);
}

void ReqHandler::ParseHeadersSetKeyValue(char* line) {
  std::string tmp;
  tmp = line;
  size_t next_loc;
  std::vector<std::string> kv_tmp;

  kv_tmp = split(tmp, ':', 0);
  RemoveTabSpace(kv_tmp[0]);
  RemoveTabSpace(kv_tmp[1]);
  next_loc = kv_tmp[1].find_first_of('\n', 0);
  if (next_loc != std::string::npos) {
    kv_tmp[1].erase(next_loc, 1);
  }
  if (kv_tmp[0] == "Content-Length") {
    req_msg_->body_data_.length_ = atoi(kv_tmp[1].c_str());
    if (req_msg_->body_data_.length_ < 1) {
      client_->SetStatusCode(400);
      client_->res_message_->body_data_.data_ =
          strdup("<h3>400 Bad Request</h3>");
      client_->res_message_->body_data_.length_ = 24;
      client_->res_message_->headers_["Content-Type"] = "text/html";
      client_->res_message_->headers_["Content-Length"] = "24";
    }
    entity_flag_ = 1;
  }
  if (kv_tmp[0] == "Content-Type") {
    req_msg_->body_data_.type_ = strdup(kv_tmp[1].c_str());
    entity_flag_ = 1;
  }
  if (kv_tmp[0] == "Content-Disposition" && kv_tmp[1] == "attachment") {
    client_->is_download = true;
  }
  if (kv_tmp[0] == "Transfer-Encoding" && kv_tmp[1] == "chunked") {
    client_->is_chunked = true;
  }
  if (kv_tmp[0] == "Transfer-Encoding" && kv_tmp[1] != "chunked") {
    client_->SetStatusCode(400);
  }

  req_msg_->headers_[kv_tmp[0]] = kv_tmp[1];
}

int64_t ReqHandler::ParseHeaders(int start_idx) {
  // buf_[start_idx]를 헤더의 첫줄로 만들기
  while (buf_[start_idx] == '\r' || buf_[start_idx] == '\n') {
    ++start_idx;
    if (start_idx == read_len_) {
      return (start_idx);
    }
  }
  int64_t curr_idx(start_idx), end_idx(start_idx);
  // buf_에서 한줄씩 찾아서 key-value로 만들어서 넣어주기
  int i(0);

  while (start_idx + i < read_len_) {
    if (buf_[start_idx + i] == '\r') {
      if (start_idx + i + 3 >= read_len_) break;
      if (strncmp(&buf_[start_idx + i], "\r\n\r\n", 4) == 0) {
        buf_[start_idx + i] = '\0';
        ParseHeadersSetKeyValue(&buf_[curr_idx]);
        break;
      }
      buf_[start_idx + i] = '\0';
      ParseHeadersSetKeyValue(&buf_[curr_idx]);
      curr_idx = start_idx + i + 2;
    }
    i++;
  }
  end_idx += i;
  return (end_idx);
}

void ReqHandler::ParseEntity(int start_idx) {
  start_idx += 1;
  while (buf_[start_idx] == '\r' || buf_[start_idx] == '\n') {
    start_idx++;
    if (start_idx == read_len_) {
#if REQ_HANDLER
      std::cout << "[ReqHandler] There is no entity(body)" << std::endl;
#endif
      client_->is_remain = true;
      return;
    }
  }
  char* entity;
  if (client_->is_chunked) {
    entity = new char[read_len_ - start_idx];
    memcpy(entity, &buf_[start_idx], read_len_ - start_idx);
    req_msg_->body_data_.length_ = read_len_ - start_idx;
    req_msg_->body_data_.data_ = entity;
  } else {
    entity = new char[req_msg_->body_data_.length_];
    memcpy(entity, &buf_[start_idx], req_msg_->body_data_.length_);
    req_msg_->body_data_.data_ = entity;
  }
}

void ReqHandler::ValidateReq() {
  // POST -> body_size 유호성 확인
  // POST인 경우에는 아래의 유호성 검사가 필요가 없다
  if (req_msg_->method_ == "DELETE") {
    return;
  }

  if (req_msg_->method_ == "POST") {
    if (client_->config_->body_size_ <
        static_cast<int>(req_msg_->body_data_.length_)) {
      // body size error의 경우에는 메서드를 GET으로 바꿔줘야 브라우저에
      // 501페이지가 나옴
      // req_msg_->method_ = "GET";
      // client_->SetStatusCode(501);

      req_msg_->method_ = "GET";
      client_->SetStatusCode(413);
    } else if (client_->GetStatusCode() == 400) {
      return;
    } else {
      client_->SetStatusCode(200);
      req_msg_->req_url_.clear();
    }
    return;
  }

  std::string req_url = decode(req_msg_->req_url_);
  // std::cout << BLUE << "decode req_url: " << req_url << RESET << std::endl;
  size_t last_slash_idx = req_url.find_last_of("/");
  std::string req_location_path = req_url.substr(0, last_slash_idx + 1);
  std::string req_file_path = req_url.substr(last_slash_idx + 1);

  if (req_location_path == "/download/") {
    client_->is_download = true;
  }
  // location이 없는 경우
  t_location* loc = client_->config_->GetCurrentLocation(req_location_path);
  if (!loc) {
    client_->SetStatusCode(404);
    req_msg_->req_url_ = client_->config_->error_pages_.find(404)->second;
    return;
  }

  // method가 유효하지 않은 경우
  if (!client_->config_->CheckAvailableMethod(req_location_path,
                                              req_msg_->method_)) {
    client_->SetStatusCode(405);
    req_msg_->req_url_ = client_->config_->error_pages_.find(405)->second;
    return;
  }

  // cgi인 경우
  if (loc->is_cgi_) {
    client_->SetCGi(true);
    // cgi인데 file_path가 없는 경우 -> ~:8080/cgi/
    if (req_file_path.empty()) {
      client_->SetStatusCode(501);
      req_msg_->req_url_ = client_->config_->error_pages_.find(501)->second;
      client_->cgi_ = false;
      return;
    }
    std::string cgi_file_path =
        req_file_path.substr(0, req_file_path.find('?'));
    std::vector<std::string>::iterator it =
        std::find(loc->cgi_path_.begin(), loc->cgi_path_.end(),
                  loc->root_path_ + cgi_file_path);

    // cgi인데 file_path가 있는 경우
    if (it != loc->cgi_path_.end()) {
      client_->SetStatusCode(200);
    }
    // cgi인데 file_path가 없는 경우 -> ~:8080/cgi/not-found.py
    else {
      client_->SetStatusCode(501);
      req_msg_->req_url_ = client_->config_->error_pages_.find(501)->second;
      client_->cgi_ = false;
    }
    return;
  }

  // location에 리다이랙션이 있는 경우
  if (!loc->redir_path_.empty()) {
    client_->SetStatusCode(301);
    req_msg_->req_url_ = loc->redir_path_;
    return;
  }

  // file_path가 없는 경우 -> / or /test/
  if (req_file_path.empty()) {
    // directory_list인 경우
    if (loc->directory_list_) {
      client_->SetDirectoryList(true);
      client_->SetStatusCode(200);
      req_msg_->req_url_ = loc->root_path_;
    }
    // directory_list가 아닌 경우
    else {
      client_->SetStatusCode(200);
      req_msg_->req_url_ = loc->file_path_[0];
    }
    return;
  }

  // file_path가 있는 경우 -> /index.html or /test/index.html
  std::vector<std::string>::iterator it =
      std::find(loc->file_path_.begin(), loc->file_path_.end(),
                loc->root_path_ + req_file_path);
  // location 안에 파일이 있는 경우
  if (it != loc->file_path_.end()) {
    // directory_list인 경우
    if (loc->directory_list_) {
      client_->SetDirectoryList(true);
      client_->SetStatusCode(200);
      req_msg_->req_url_ = loc->root_path_;
    }
    // directory_list가 아닌 경우
    else {
      client_->SetStatusCode(200);
      req_msg_->req_url_ = *it;
    }
    return;
  }
  // location 안에 파일이 없는 경우
  else {
    client_->SetStatusCode(404);
    req_msg_->req_url_ = client_->config_->error_pages_.find(404)->second;
    return;
  }

  return;
}

void ReqHandler::ParseRecv() {
  if (buf_ == NULL) {
#if REQ_HANDLER
    std::cout << "[ReqHandler] Parse error. Need buf_. call RecvFromSocket()"
              << std::endl;
#endif
    return;
  }

  if (client_->is_remain == true || client_->is_chunked == true) {
    if (client_->req_message_->body_data_.data_)
      delete[] client_->req_message_->body_data_.data_;
    client_->req_message_->body_data_.data_ = new char[read_len_];
    memcpy(client_->req_message_->body_data_.data_, buf_, read_len_);
    client_->req_message_->body_data_.length_ = read_len_;
  } else {
    if (req_msg_ == NULL) req_msg_ = new t_req_msg;
    memset(&req_msg_->body_data_, 0, sizeof(t_entity));
    int64_t idx(0);
    // 첫줄 파싱
    idx = ParseFirstLine();  // buf[idx] = 첫줄의 \n
    // 헤더 파싱
    idx = ParseHeaders(idx);  // buf[idx] = 마지막 헤더줄의 /r
    // entity 넣기
    if (entity_flag_ == 1) {
      ParseEntity(idx);
    }
    ValidateReq();
  }
  delete[] buf_;
  buf_ = NULL;
}
