#include "../../include/Server.hpp"

void Server::ExecuteReadEvent(const int& idx) {
  Data* client = reinterpret_cast<Data*>(events_[idx].udata);
  int event_fd = events_[idx].ident;

  if (event_fd == client->GetClientFd()) {
    ExecuteReadEventClientFd(idx);
    return;
  }
  /* 내부에서 읽으려고 Open()한 File에 대한 이벤트 */
  if (event_fd == client->GetFileFd()) {
    ExecuteReadEventFileFd(idx);
    return;
  }
  /* CGI에게 반환받는 pipe[READ]에 대한 이벤트 */
  if (event_fd == client->GetPipeRead() && events_[idx].flags & EV_EOF) {
#if SERVER
    std::cout << "[Server] Pipe READ fd : " << event_fd
              << " == " << client->GetPipeRead() << std::endl;
#endif
    ExecuteReadEventPipeFd(idx);
    return;
  }
}

void Server::ExecuteReadEventClientFd(const int& idx) {
  Data* client = reinterpret_cast<Data*>(events_[idx].udata);
#if SERVER
  std::cout << "[Server] Client READ fd : " << client->GetClientFd()
            << std::endl;
#endif
  if (client->is_remain == false && client->is_chunked == false) {
    client->Init();
  }
  if (events_[idx].flags & EV_EOF) {
    DisConnect(events_[idx].ident);
#if SERVER
    std::cout << "[Server] eof fd : " << events_[idx].ident << std::endl;
#endif
  } else
    ActClientCoreLogic(idx);
}

void Server::ExecuteReadEventFileFd(int idx) {
  Data* client = reinterpret_cast<Data*>(events_[idx].udata);
  int client_fd = client->GetClientFd();
  int file_fd = client->GetFileFd();
#if SERVER
  std::cout << "[Server] File READ fd : " << events_[idx].ident
            << " == " << client->GetFileFd() << std::endl;
#endif

  int size = client->event_[idx].data;
  char* buf = new char[size];
  int read_return = read(file_fd, buf, size);

  struct kevent event;
  EV_SET(&event, file_fd, EVFILT_READ, EV_DELETE, 0, 0, client);
  kevent(kq_, &event, 1, NULL, 0, NULL);
  if (read_return < 0) {
#if SERVER
    std::cout << "[Server] File READ error : " << events_[idx].ident
              << " == " << client->GetFileFd() << std::endl;
#endif
    client->status_code_ = 500;
    client->req_message_->req_url_ =
        client->config_->error_pages_.find(500)->second;
    delete[] buf;
    Get(idx);
    return;
  }
  client->res_message_->body_data_.data_ = buf;
  client->res_message_->body_data_.length_ = size;
  client->res_message_->headers_["Content-Length"] = to_string(size);

  EV_SET(&event, client_fd, EVFILT_WRITE, EV_ENABLE, 0, 0, client);
  kevent(kq_, &event, 1, NULL, 0, NULL);
}

void Server::ExecuteReadEventPipeFd(int idx) {
  Data* client = reinterpret_cast<Data*>(events_[idx].udata);
  if (cgi_manager_->GetFromCGI(client, events_[idx].data, kq_) == false) {
    Get(idx);
  }
}

void Server::Pong(int idx) {
  Data* client = reinterpret_cast<Data*>(events_[idx].udata);
  int client_fd = client->GetClientFd();
  struct kevent event;

  client->res_message_->headers_["Content-Type"] = "text/plain";
  client->res_message_->headers_["Content-Length"] = "4";
  client->res_message_->body_data_.data_ = strdup("pong");
  client->res_message_->body_data_.length_ = 4;

  EV_SET(&event, client_fd, EVFILT_WRITE, ENABLE, 0, 0, client);
  kevent(kq_, &event, 1, NULL, 0, NULL);
}

void Server::ActClientCoreLogic(int idx) {
  Data* client = reinterpret_cast<Data*>(events_[idx].udata);
  const ServerConfigInfo* config = client->GetConfig();
  struct kevent event;
  int client_fd = client->GetClientFd();

  EV_SET(&event, client_fd, EVFILT_READ, EV_DISABLE, 0, 0, client);
  kevent(kq_, &event, 1, NULL, 0, NULL);

  req_handler_->SetClient(clients_->GetDataByFd(events_[idx].ident));
  req_handler_->SetReadLen(events_[idx].data);
  if (events_[idx].data == 0) {
#if SERVER
    std::cout << "[Server] pong fd : " << client_fd << std::endl;
    std::cout << RED << "PONG!\n" << RESET;
#endif
    Pong(idx);
    return;
  }
  if (req_handler_->RecvFromSocket() < 1) {
    DisConnect(client_fd);
    return;
  }
  req_handler_->ParseRecv();

  client->SetReqMessage(req_handler_->req_msg_);
  req_handler_->Clear();

  if (session_->IsValidCookie(client) == false) {
    session_->SetCookie(client);
  } else {
    client->res_message_->headers_["Set-Cookie"] =
        client->req_message_->headers_["Cookie"];
  }

  if (client->GetStatusCode() == 413 || client->GetStatusCode() == 400) {
    EV_SET(&event, client_fd, EVFILT_WRITE, EV_ENABLE, 0, 0, client);
    kevent(kq_, &event, 1, NULL, 0, NULL);
  } else if (client->GetReqMethod() == "GET") {
    Get(idx);
  } else if (client->GetReqMethod() == "POST") {
    if (client->is_chunked == true) {
      HandleChunkedData(idx);
    } else if (client->is_remain == true &&
               client->req_message_->body_data_.data_ == NULL) {
      EV_SET(&event, client_fd, EVFILT_READ, EV_ENABLE, 0, 0, client);
      kevent(kq_, &event, 1, NULL, 0, NULL);
    } else {
      Post(idx);
    }
  } else if (client->GetReqMethod() == "DELETE") {
    Delete(idx);
  } else {
    client->SetStatusCode(501);
    client->SetReqURL(config->error_pages_.find(501)->second);
    Get(idx);
    return;
  }
}
