#include "../../include/Server.hpp"

void Server::ExecuteWriteEvent(const int& idx) {
  Data* client = reinterpret_cast<Data*>(events_[idx].udata);
  int event_fd = events_[idx].ident;

  if (event_fd == client->GetClientFd()) {
    const int client_status_code = client->GetStatusCode();
    if (client_status_code >= 200 && client_status_code < 300) {
      logger_.info("[farewell_webserv]", client);
    } else {
      logger_.error("[farewell_webserv]", client);
    }
    ExecuteWriteEventClientFd(idx);
    if (client->is_remain == false &&
        (client->is_chunked == false || client->GetReqMethod() == "GET")) {
      if (client->file_fd_ > 0) {
        close(client->file_fd_);
      }
      client->Clear();
    }
    return;
  }
  /* POST, PUT file에 대한 write 발생시 */
  if (event_fd == client->GetFileFd()) {
    ExecuteWriteEventFileFd(idx);
    return;
  }
  /* CGI에게 보내줄 pipe[WRITE]에 대한 이벤트 */
  if (event_fd == client->GetPipeWrite()) {
    ExecuteWriteEventPipeFd(idx);
    return;
  }
}

void Server::ExecuteWriteEventClientFd(int idx) {
  Data* client = reinterpret_cast<Data*>(events_[idx].udata);
  ServerConfigInfo* config = client->GetConfig();
#if SERVER
  std::cout << "[Server] Client Write : " << client->GetClientFd() << std::endl;
#endif
  client->res_message_->headers_["Server"] = config->server_name_;
  client->res_message_->headers_["Date"] = GetCurrentDate();

  if (client->timeout_ == true || client->GetStatusCode() == 413 ||
      client->GetStatusCode() == 400) {
    client->res_message_->headers_["Connection"] = "close";
  } else {
    client->res_message_->headers_["Connection"] = "keep-alive";
  }

  if (client->is_download == true) {
    client->res_message_->headers_["Content-Disposition"] =
        "attachment; filename=" + client->file_name;
  }

  client->res_message_->headers_["Cache-Control"] =
      "no-cache, no-store, must-revalidate";
  client->res_message_->headers_["Pragma"] = "no-cache";
  client->res_message_->headers_["Expires"] = "0";

  msg_composer_->SetData(client);
  msg_composer_->InitResMsg(client);
  int client_fd = client->GetClientFd();
  int file_fd = client->GetFileFd();
  struct kevent event;
  const char* response = msg_composer_->GetResponse(client);
  if (send(client_fd, response, msg_composer_->GetLength(), 0) < 1) {
#if SERVER
    std::cout << "[send] ExecuteWriteEventClientFd send return -1. socket :
                 "
              << client_fd << std::endl;
#endif
    DisConnect(client_fd);
    delete[] response;
    response = NULL;
    EV_SET(&event, client_fd, EVFILT_WRITE, EV_DISABLE, 0, 0, client);
    kevent(kq_, &event, 1, NULL, 0, NULL);
    return;
  }

  delete[] response;
  response = NULL;

  // struct kevent event;
  EV_SET(&event, client_fd, EVFILT_WRITE, EV_DISABLE, 0, 0, client);
  kevent(kq_, &event, 1, NULL, 0, NULL);

  EV_SET(&event, client_fd, EVFILT_READ, EV_ENABLE, 0, 0, client);
  kevent(kq_, &event, 1, NULL, 0, NULL);

  if (file_fd != -1 && client->is_remain == false &&
      client->is_chunked == false) {
    EV_SET(&event, file_fd, EVFILT_READ, EV_DELETE, 0, 0, NULL);
    kevent(kq_, &event, 1, NULL, 0, NULL);

    EV_SET(&event, file_fd, EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
    kevent(kq_, &event, 1, NULL, 0, NULL);

    close(file_fd);
  }
  if (client->timeout_ == true || client->GetStatusCode() == 413 ||
      client->GetStatusCode() == 400) {
    DisConnect(client_fd);
  }
}

void Server::ExecuteWriteEventFileFd(int idx) {
  Data* client = reinterpret_cast<Data*>(events_[idx].udata);
#if SERVER
  std::cout << "[Server] File Fd Write : " << client->GetClientFd()
            << std::endl;
#endif
  int client_fd = client->GetClientFd();
  int file_fd = client->GetFileFd();

  if (client->is_chunked == true) {
    write(file_fd, client->chunk_body, client->binary_size);
    delete client->chunk_body;
    client->chunk_body = NULL;
    if (client->chunked_done == true) {
      client->is_chunked = false;
    }
  } else if (client->binary_size == 0) {
    write(file_fd, client->post_data_.c_str(), client->post_data_.size());
  } else
    write(file_fd,
          &client->req_message_->body_data_.data_[client->binary_start_idx],
          client->binary_size);

  struct kevent event;

  if (client->is_remain == true || client->is_chunked == true) {
    EV_SET(&event, client_fd, EVFILT_READ, EV_ENABLE, 0, 0, client);
    kevent(kq_, &event, 1, NULL, 0, NULL);
    EV_SET(&event, file_fd, EVFILT_WRITE, EV_DISABLE, 0, 0, client);
    kevent(kq_, &event, 1, NULL, 0, NULL);
  } else {
    EV_SET(&event, client_fd, EVFILT_WRITE, EV_ENABLE, 0, 0, client);
    kevent(kq_, &event, 1, NULL, 0, NULL);
    EV_SET(&event, file_fd, EVFILT_WRITE, EV_DELETE, 0, 0, client);
    kevent(kq_, &event, 1, NULL, 0, NULL);
  }
}

void Server::ExecuteWriteEventPipeFd(int idx) {
  Data* client = reinterpret_cast<Data*>(events_[idx].udata);
  if (cgi_manager_->WriteToCGIPipe(client, kq_) == false) {
    Get(idx);
  }
}
