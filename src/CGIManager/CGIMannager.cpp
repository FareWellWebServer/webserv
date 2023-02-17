#include "../../include/CGIManager.hpp"

/* CGIManager 사용시 반드시 세팅하기 */
void CGIManager::SetData(Data* client) { client_ = client; }

/* fork 후 exeve 직전 */
void CGIManager::SetCGIEnv(Data* client) {
  SetData(client);

  /* Content-Type */
  std::string key = "Content_Type";
  std::string value = client_->GetReqHeaderByKey(key);
  if (value.empty() == true) setenv("CONTENT_TYPE", value.c_str(), 1);

  /* Content-Length */
  key = "Content-Length";
  value = client_->GetReqHeaderByKey(key);
  if (value.empty() == true) setenv("CONTENT_LENGTH", value.c_str(), 1);

  /* Cookie */
  key = "Cookie";
  value = client_->GetReqHeaderByKey(key);
  if (value.empty() == true) setenv("HTTP_COOKIE", value.c_str(), 1);

  /* User-Agent */
  key = "User-Agent";
  value = client_->GetReqHeaderByKey(key);
  if (value.empty() == true) setenv("HTTP_USER_AGENT", value.c_str(), 1);

  /* Cgi script path - PATH_INFO */
  setenv("PATH_INFO", "../cgi/", 1);

  /* QUERY_STRING. URL 뒤에 붙은 녀석 */
  // .py? 인데, ? 부터 넣어줘야되는가? ? 뒤부터 넣어줘야 하는가?
  std::string url_query = client_->GetReqURL();
  std::string query_string = url_query.substr(url_query.find(".py?") + 1);
  setenv("QUERY_STRING", query_string.c_str(), 1);

  /* REMOTE_ADDR */
  // The IP address of the remote host making the request.
  setenv("REMOTE_ADDR", client_->GetClientName(), 1);
  setenv("REMOTE_HOST", client_->GetClientName(), 1);

  /* REQUEST_METHOD */
  setenv("REQUEST_METHOD", client_->GetReqMethod().c_str(), 1);

  /* Cgi script full path -  */
  setenv("SCRIPT_FILENAME", "../cgi/cgi.py", 1);

  /* SCRIPT_NAME */
  setenv("SCRIPT_NAME", "cgi.py", 1);

  /* SERVER_NAME */
  setenv("SERVER_NAME", client_->GetConfig()->host_.c_str(), 1);

  /* SERVER_SOFTWARE */
  setenv("SERVER_NAME", client_->GetConfig()->host_.c_str(), 1);
}

// 요청 들어온 것들 CGI 로 넘겨주는 것
// req_massage의 첫줄은 argv로, 헤더는 env로 본문은 stdin으로 보내줌
void CGIManager::SendToCGI(Data* client, int kq) {
  SetData(client);
  int p[2];
  pid_t pid;

  if (!client->is_chunked || (client->is_chunked && client->is_first)) {
    pipe(p);
    client_->SetPipeRead(p[READ]);
    client_->SetPipeWrite(p[WRITE]);
    pid = fork();
  } else {
    pid = 1;
    p[READ] = client_->GetPipeRead();
    p[WRITE] = client_->GetPipeWrite();
  }
  if (pid > 0) {
    struct kevent event;
    EV_SET(&event, p[WRITE], EVFILT_WRITE, EV_ADD, 0, 0, client_);
    kevent(kq, &event, 1, NULL, 0, NULL);
    EV_SET(&event, pid, EVFILT_PROC, EV_ADD, NOTE_EXIT, 0, client_);
    kevent(kq, &event, 1, NULL, 0, NULL);
  } else if (pid == 0) {
    SetCGIEnv(client);
    dup2(p[WRITE], STDOUT_FILENO);
    dup2(p[READ], STDIN_FILENO);
    close(p[READ]);
    close(p[WRITE]);
    extern char** environ;
    char** argv = new char*[3];
    argv[0] = strdup("python3");
    argv[1] = strdup(
        ("." + client->GetReqURL().substr(0, client->GetReqURL().find('?')))
            .c_str());
    argv[2] = NULL;
    if (execve("/usr/bin/python3", argv, environ) < 0) {
#if CGI
      std::cout << "[CGI] execve 에러" << std::endl;
#endif
    }
  } else {
#if CGI
    std::cout << "[CGI] fork() 에러" << std::endl;
#endif
  }
}

bool CGIManager::WriteToCGIPipe(Data* client, int kq) {
  int write_return = write(client->GetPipeWrite(), client_->GetReqBodyData(),
                           client_->GetReqBodyLength());
  struct kevent event;
  EV_SET(&event, client->GetPipeWrite(), EVFILT_WRITE, EV_DELETE, 0, 0,
         client_);
  kevent(kq, &event, 1, NULL, 0, NULL);
  if (write_return < 0) {
    close(client->GetPipeWrite());
    close(client->GetPipeRead());
    client_->SetPipeRead(-1);
    client_->SetPipeWrite(-1);
    client->status_code_ = 500;
    client->req_message_->req_url_ =
        client->config_->error_pages_.find(500)->second;
    client->cgi_ = false;
    return false;
  }
  if (client->is_chunked == false ||
      (client->is_chunked == true &&
       strncmp(client_->GetReqBodyData(), "0\r\n", 3) == 0)) {
    close(client->GetPipeWrite());
    client_->SetPipeWrite(-1);
  }
  EV_SET(&event, client->GetPipeRead(), EVFILT_READ, EV_EOF | EV_ADD, 0, 0,
         client_);
  kevent(kq, &event, 1, NULL, 0, NULL);
  return true;
}

bool CGIManager::GetFromCGI(Data* client, int64_t len, int kq) {
  SetData(client);

  char* buf = new char[len + 1];
  int read_return = read(client_->GetPipeRead(), buf, len);
  buf[len] = '\0';
  struct kevent event;
  EV_SET(&event, client_->GetPipeRead(), EVFILT_READ, EV_EOF | EV_DELETE, 0, 0,
         NULL);
  kevent(kq, &event, 1, NULL, 0, NULL);
  close(client_->GetPipeRead());
  if (read_return < 0) {
    client->status_code_ = 500;
    client->req_message_->req_url_ =
        client_->config_->error_pages_.find(500)->second;
    delete[] buf;
    client->cgi_ = false;
    return false;
  } else {
    ParseCGIData(buf);
    EV_SET(&event, client->GetClientFd(), EVFILT_WRITE, EV_ENABLE, 0, 0,
           client);
    kevent(kq, &event, 1, NULL, 0, NULL);
    delete[] buf;
    return true;
  }
}

void CGIManager::SetFirstLine() {
  client_->res_message_->http_version_ = "HTTP/1.1";
  client_->res_message_->status_code_ = 200;
}

void CGIManager::ParseCGIData(char* buf) {
  std::string body = buf;
  size_t idx(0);
  // 헤더 세팅
  idx = SetHeaders(body);
  // 본문 길이 설정
  SetBodyLength(body, idx);
  // 본문 넣어주기
  SetBody(buf, idx);
  // 첫줄 컨텐츠타입 잘 왔는지 확인. 없으면 CGI 에러
  if (client_->res_message_->headers_.find("Content-Type") ==
      client_->res_message_->headers_.end())
    client_->status_code_ = 501;
  else
    SetFirstLine();
}

size_t CGIManager::SetHeaders(std::string& body) {
  size_t start_idx(0), endline_idx(0), delimiter_idx(0);
  std::string key, val;

  while (1) {
    endline_idx = body.find_first_of('\n', start_idx);
    delimiter_idx = body.find_first_of(":", start_idx);
    key = body.substr(start_idx, delimiter_idx - start_idx);
    val = body.substr(delimiter_idx + 1, endline_idx - delimiter_idx - 1);
    RemoveTabSpace(val);
    client_->res_message_->headers_[key] = val;
    start_idx = endline_idx + 1;
    if (body[endline_idx + 1] == '\r') break;
  }
  start_idx += 2;
  return start_idx;
}

void CGIManager::SetBodyLength(std::string& body, size_t idx) {
  size_t len(body.length() - (idx)-1);
  client_->res_message_->body_data_.length_ = len;
  if (len > 0)
    client_->res_message_->headers_["Content-Length"] = to_string(len);
}

void CGIManager::SetBody(char* buf, size_t idx) {
  if (client_->res_message_->body_data_.length_ != 0)
    client_->res_message_->body_data_.data_ =
        new char[client_->res_message_->body_data_.length_];
  memcpy(client_->res_message_->body_data_.data_, &buf[idx],
         client_->res_message_->body_data_.length_);
  if (client_->res_message_->body_data_
          .data_[client_->res_message_->body_data_.length_ - 1] == '\n') {
    client_->res_message_->body_data_
        .data_[client_->res_message_->body_data_.length_ - 1] = '\0';
    client_->res_message_->body_data_.length_--;
  }
}
