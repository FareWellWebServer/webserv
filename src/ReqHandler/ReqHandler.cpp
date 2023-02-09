#include "../../include/ReqHandler.hpp"

ReqHandler::ReqHandler(void)
    : req_msg_(NULL), entity_flag_(0), buf_(NULL), client_(0), read_len_(0) {}

ReqHandler::~ReqHandler(void) { Clear(); }

void ReqHandler::SetClient(Data* client) { client_ = client; }

void ReqHandler::SetReadLen(int64_t kevent_data) { read_len_ = kevent_data; }

void ReqHandler::Clear() {
  if (buf_ != NULL) {
    delete[] buf_;
    buf_ = NULL;
  }
  read_len_ = 0;
  client_ = NULL;
  req_msg_ = NULL;

  // req_msg_는 ClientMetaData가 가지고 있어야하니 여기서 할당해제하면 안될 듯.
  // if (req_msg_ != NULL) {
  //   delete req_msg_;
  //   req_msg_ = NULL;
  // }
}

// void ReqHandler::SetReadLen(int64_t len) { read_len_ = len; }

void ReqHandler::SetBuf(int fd) {
  int byte = 0;
  buf_ = new char[read_len_ * sizeof(char)];
  byte = read(fd, buf_, read_len_);
  if (byte < 0) {
    std::cout << "ERROR IN SET BUF" << std::endl;
  }
}

void ReqHandler::RecvFromSocket() {
  if (client_ == NULL || read_len_ == 0) {
#if REQ_HANDLER
    std::cout << " [ReqHandler] Recv error. Need client data. call SetClient() "
              << "or SetReadLen() " << &client_ << read_len_ << std::endl;
#endif
    return;
  }

  ssize_t recv_return(0);
  buf_ = new char[sizeof(char) * read_len_];
  recv_return = recv(client_->GetClientFd(), buf_, read_len_, 0);
  if (recv_return == -1) {
#if REQ_HANDLER
    std::cout << "[ReqHandler] recv return -1. socket : "
              << client_->GetClientFd() << std::endl;
#endif
    // EV_SET;
    // return ; ?? 어디로 돌아가야되지
  } else if (recv_return != read_len_) {
#if REQ_HANDLER
    std::cout << "[ReqHandler] recv return != kevent->data : \
    loss data?"
              << std::endl;
#endif
  }
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
  // 체크할 때 -1을 반환하는 형식으로 변경해도 될듯
  //   if (req_msg_->method_ != "GET" || req_msg_->method_ != "POST" ||
  //       req_msg_->method_ != "HEAD" || req_msg_->method_ != "DELETE") {
  // #if REQ_HANDLER
  //     std::cout << "[ReqHandler] Invalid method input ! : \
//     loss data?"
  //               << std::endl;
  // #endif
  //   }

  curr_idx += find_idx;
  /* 두번 째 URL 쪼개기 */
  find_idx = strcspn(&buf_[curr_idx + 1], " ");
  if (find_idx >= end_idx) {
    // client_->SetStatusCode(400); // bad request
    return (read_len_);
  }
  strncpy(tmp, &buf_[curr_idx + 1], find_idx);
  tmp[find_idx] = '\0';
  /* 올바른 URL 인지 확인 필요 */
  // TODO : config 파일에서 들어온 경로와 일치하는가 확인 ->파싱이 끝난 이후
  req_msg_->req_url_ = tmp;
  ReduceSlash(req_msg_->req_url_);

  curr_idx += find_idx;

  /* 버전확인 406 Not Acceptable */
  find_idx = strcspn(&buf_[curr_idx + 1], "\r");
  if (find_idx >= end_idx) {
    // client_->SetStatusCode(400); // bad request
    return (read_len_);
  }
  strncpy(tmp, &buf_[curr_idx + 1], find_idx);
  tmp[find_idx] = '\0';

  req_msg_->protocol_ = tmp;
  RemoveTabSpace(req_msg_->protocol_);
  if (req_msg_->protocol_ != "HTTP/1.1") {
    // client_->SetStatusCode(400); // bad request
    std::cout << "INVALID PROTOCOL" << std::endl;
    // TODO : ERROR처리 필요
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
    entity_flag_ = 1;
  }
  if (kv_tmp[0] == "Content-type") {
    req_msg_->body_data_.type_ = strdup(kv_tmp[1].c_str());
    entity_flag_ = 1;
  }
  req_msg_->headers_[kv_tmp[0]] = kv_tmp[1];
}

int64_t ReqHandler::ParseHeaders(int start_idx) {
  /* buf_[start_idx]를 헤더의 첫줄로 만들기 */
  while (buf_[start_idx] == '\r' || buf_[start_idx] == '\n') {
    ++start_idx;
    if (start_idx == read_len_) {
      return (start_idx);
    }
  }
  int64_t curr_idx(start_idx), end_idx(start_idx);
  /* buf_에서 한줄씩 찾아서 key-value로 만들어서 넣어주기 */
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
    // TODO : read_len 은 임의 지정값이라 실제 데이터의 길이를 반영하지
    // 못할것으로 생각되어 read로 읽어온
    // byte로 봐야하지 않나..
    if (start_idx == read_len_) {
#if DG
      std::cout << "[ReqHandler] There is no entity(body)" << std::endl;
#endif
      return;
    }
  }
  char* entity = new char[req_msg_->body_data_.length_ + 1];
  memcpy(entity, &buf_[start_idx], req_msg_->body_data_.length_);
  req_msg_->body_data_.data_ = entity;
}

void ReqHandler::ParseRecv() {
  if (buf_ == NULL) {
#if DG
    std::cout << "[ReqHandler] Parse error. Need buf_. call RecvFromSocket()"
              << std::endl;
#endif
    return;
  }
  client_->e_stage = REQ_PROCESSING;
  if (req_msg_ == NULL) req_msg_ = new t_req_msg;
  memset(&req_msg_->body_data_, 0, sizeof(t_entity));
  int64_t idx(0);
  // 첫줄 파싱
  idx = ParseFirstLine();  // buf[idx] = 첫줄의 \n
  // 헤더 파싱
  idx = ParseHeaders(idx);  // buf[idx] = 마지막 헤더줄의 /r
  // entity 넣기
  if (entity_flag_ == 1) ParseEntity(idx);

  // request 유효성 검사 -> body_size, method
  std::cout << BLUE << "original req_url: " << req_msg_->req_url_ << std::endl;
  ValidateReq();
  std::cout << "status code: " << client_->status_code_ << std::endl;
  std::cout << "served req_url: " << req_msg_->req_url_ << RESET << std::endl;

  delete[] buf_;
  buf_ = NULL;
}

void ReqHandler::ValidateReq(void) {
  // body_size가 너무 큰 경우
  if (client_->config_->body_size_ <
      static_cast<int>(req_msg_->body_data_.length_)) {
    client_->SetStatusCode(501);
    req_msg_->req_url_ = client_->config_->error_pages_.find(501)->second;
  }

  std::string req_url = req_msg_->req_url_;
  // 중복 slash 제거 EX) ////// -> /
  ReduceSlash(req_msg_->req_url_);
  size_t last_slash_idx = req_url.find_last_of("/");
  std::string req_location_path = req_url.substr(0, last_slash_idx + 1);
  std::string req_file_path = req_url.substr(last_slash_idx + 1);

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
      return;
    }
    std::vector<std::string>::iterator it =
        std::find(loc->cgi_path_.begin(), loc->cgi_path_.end(),
                  loc->root_path_ + req_file_path);

    // cgi인데 file_path가 있는 경우
    if (it != loc->file_path_.end()) {
      client_->SetStatusCode(200);
      req_msg_->req_url_ = *it;
    }
    // cgi인데 file_path가 없는 경우 -> ~:8080/cgi/not-found.py
    else {
      client_->SetStatusCode(501);
      req_msg_->req_url_ = client_->config_->error_pages_.find(501)->second;
    }
    return;
  }

  // location에 리다이랙션이 있는 경우
  if (!loc->redir_path_.empty()) {
    client_->SetStatusCode(301);
    req_msg_->req_url_ = loc->redir_path_;
    return;
  }

  // location에 directory_list가 있는 경우
  if (loc->directory_list_) {
    client_->SetDirectoryList(true);
    client_->SetStatusCode(200);
    req_msg_->req_url_ = loc->root_path_;
    return;
  }

  // file_path 가 없는 경우 -> / or /test/
  if (req_file_path.empty()) {
    client_->SetStatusCode(200);
    req_msg_->req_url_ = loc->file_path_[0];
    return;
  }

  // file_path가 있는 경우 -> /index.html or /test/index.html
  std::vector<std::string>::iterator it =
      std::find(loc->file_path_.begin(), loc->file_path_.end(),
                loc->root_path_ + req_file_path);
  // location 안에 파일이 있는 경우
  if (it != loc->file_path_.end()) {
    client_->SetStatusCode(200);
    req_msg_->req_url_ = *it;
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
