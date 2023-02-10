#include "../../include/Server.hpp"

Server::Server(std::vector<ServerConfigInfo> server_info)
    : server_infos_(server_info), kq_(kqueue()) {
  clients_ = new ClientMetaData;
  req_handler_ = new ReqHandler;
  res_handler_ = new ResHandler;
  msg_composer_ = new MsgComposer;
  cgi_manager_ = new CGIManager;
}
Server::~Server(void) {
  close(kq_);
  servers_.clear();
  delete clients_;
  delete req_handler_;
  delete res_handler_;
  delete msg_composer_;
}
void Server::Run(void) {
  if (servers_.size() == 0) {
    throw std::runtime_error("[Server Error]: no listening server");
  }
  while (true) {
    Act();
  }
}
void Server::Init(void) {
  for (size_t i = 0; i < server_infos_.size(); ++i) {
#if SERVER
    if (servers_.size() == MAXLISTEN) {
      std::cerr << "Error: full of listening\n";
      return;
    }
#endif
    struct kevent event;
    int listenfd;

    BindListen(server_infos_[i].host_, server_infos_[i].port_, listenfd);
    EV_SET(&event, listenfd, EVFILT_READ, EV_ADD, 0, 0,
           (void*)&server_infos_[i]);
    // EV_SET(&event, listenfd, EVFILT_READ, EV_ADD, 0, 0, NULL);
    if (kevent(kq_, &event, 1, NULL, 0, NULL) == -1) {
      throw std::runtime_error("Error: kevent()");
    }
    t_listening* tmp = CreateListening(server_infos_[i].host_,
                                       server_infos_[i].port_, listenfd);
    servers_.insert(tmp);
    // delete tmp;
#if SERVER
    std::cout << server_infos_[i].host_ << " is listening port on "
              << server_infos_[i].port_ << "\n";
#endif
  }
}

// private
void Server::Act(void) {
  int n = kevent(kq_, NULL, 0, events_, MAXLISTEN, NULL);
  if (n == -1) {
    throw std::runtime_error("Error: kevent()");
  }
  for (int idx = 0; idx < n; ++idx) {
    /* listen port로 새로운 connect 요청이 들어옴 */
    if (IsListenFd(events_[idx].ident) && events_[idx].filter == EVFILT_READ) {
      AcceptNewClient(idx);
      continue;
    }
    Data* client = reinterpret_cast<Data*>(events_[idx].udata);
    int event_fd = events_[idx].ident;
    if (events_[idx].filter == EVFILT_READ) {
      /* accept 된 port로 request 요청메세지 들어옴 */
      if (event_fd == client->GetClientFd()) {
#if SERVER
        std::cout << "[Server] Client READ fd : " << client->GetClientFd()
                  << std::endl;
#endif
        client->Init();
        if (events_[idx].flags == EV_EOF)
          std::cout << RED << "FUCK\n" << RESET;
        else
          ActCoreLogic(idx);
      }
      /* 내부에서 읽으려고 Open()한 File에 대한 이벤트 */
      else if (event_fd == client->GetFileFd()) {
#if SERVER
        std::cout << "[Server] File READ fd : " << event_fd
                  << " == " << client->GetFileFd() << std::endl;
#endif
        ReadFile(idx);
      }
      /* CGI에게 반환받는 pipe[READ]에 대한 이벤트 */
      else if (event_fd == client->GetPipeRead()) {
#if SERVER
        std::cout << "[Server] Pipe READ fd : " << event_fd
                  << " == " << client->GetPipeRead() << std::endl;
#endif
        // pipe 읽기
      }
    } else if (events_[idx].filter == EVFILT_WRITE) {
      /* response 보낼 때에 대한 이벤트 */
      if (event_fd == client->GetClientFd()) {
#if SERVER
        std::cout << "[Server] Client Write : " << client->GetClientFd()
                  << std::endl;
#endif
        Send(idx);
        client->Clear();
      }
      /* POST, PUT file에 대한 write 발생시 */
      else if (event_fd == client->GetFileFd()) {
        WriteFile(idx);
      }
      /* CGI에게 보내줄 pipe[WRITE]에 대한 이벤트 */
      else if (event_fd == client->GetPipeRead()) {
      }
      /* log file에 대한 write 발생시 */
      else if (event_fd == client->GetLogFileFd())
        ;
    } else if (events_[idx].flags == EV_EOF) {
      /* socket이 닫혔을 때 */
      if (event_fd == client->GetClientFd()) {
      }
      /* file, pipe 등이 예상치 못하게 닫혔을 경우도 있을지 나중에 보기 */
      else {
      }
    }
    // /* timeout 발생시 */
    else if (events_[idx].filter == EVFILT_TIMER) {
      if (client->is_working == true) {
        client->timeout_ = true;
      } else {
        std::cout << RED << "[Server] Client fd : " << client->GetClientFd() << " Time Out!\n" << RESET;
        DisConnect(event_fd);
      }
    }
    client = NULL;
  }
}

void Server::ActCoreLogic(int idx) {
  req_handler_->SetClient(clients_->GetDataByFd(events_[idx].ident));
  req_handler_->SetReadLen(events_[idx].data);
  if (events_[idx].data == 0) {
    // 핑처리 해야함.
    std::cout << RED << "req len is 0\n" << RESET;
    return;
  }
  req_handler_->RecvFromSocket();
  req_handler_->ParseRecv();

  Data* client = reinterpret_cast<Data*>(events_[idx].udata);
  client->SetReqMessage(req_handler_->req_msg_);
  req_handler_->Clear();

  // req body data 출력 for 문
  // for(size_t i = 0; i < client->req_message_->body_data_.length_; ++i)
  //   write(1, &client->req_message_->body_data_.data_[i], 1);


  // Data* client = clients_->GetDataByFd(events_[idx].ident);
  if (client->GetReqMethod() == "GET") {
    Get(idx);
  } else if (client->GetReqMethod() == "POST") {
    Post(idx);
  } else if (client->GetReqMethod() == "DELETE") {
  } else {
  }
}

void Server::AcceptNewClient(int idx) {
  int connfd;
  socklen_t client_len;
  struct sockaddr_storage client_addr;
  char host[MAXBUF];
  char port[MAXBUF];
  struct kevent event[3];

  client_len = sizeof(client_addr);
  connfd = accept(events_[idx].ident, reinterpret_cast<struct sockaddr*>(&client_addr), &client_len);
  if (connfd == -1) {
    throw std::runtime_error("Error: accept()");
  }

  /* non-block 세팅 필요 */

  /* Data 만들기 */
  ServerConfigInfo* config = reinterpret_cast<ServerConfigInfo*>(events_[idx].udata);
  getnameinfo(reinterpret_cast<struct sockaddr*>(&client_addr), client_len,
              host, MAXBUF, port, MAXBUF, 0);
  clients_->AddData(events_[idx].ident, connfd, config->port_, host, port);
  clients_->SetEvent(&events_[idx]);
  clients_->SetConfig();
  std::cout << "Connected to (" << host << ", " << port << "). socket : " << connfd << std::endl;

  /* event setting */
  EV_SET(&event[0], connfd, EVFILT_READ, EV_ADD, 0, 0, clients_->GetData());
  EV_SET(&event[1], connfd, EVFILT_WRITE, EV_ADD | EV_DISABLE, 0, 0, clients_->GetData());
  EV_SET(&event[2], connfd, EVFILT_TIMER, EV_ADD, NOTE_SECONDS, config->timeout_, clients_->GetData());
  if (kevent(kq_, event, 3, NULL, 0, NULL) == -1) {
    throw std::runtime_error("Error: kevent()");
  }
}
void Server::BindListen(const std::string& host, const int& port,
                        int& listenfd) {
  struct addrinfo* listp;
  struct addrinfo* p;
  int optval = ENABLE;
  GetAddrInfo(host, port, &listp);
  for (p = listp; p; p = p->ai_next) {
    listenfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
    if (listenfd < 0) {
      throw std::runtime_error("failed at socket create");
    }
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR,
               reinterpret_cast<const void*>(&optval), sizeof(int));
    if (bind(listenfd, p->ai_addr, p->ai_addrlen) == 0) {
      break;
    }
    close(listenfd);
  }
  freeaddrinfo(listp);
  if (p == NULL) {
    throw std::runtime_error("Error: socket()");
  }
  if (listen(listenfd, BACKLOG) < 0) {
    close(listenfd);
    throw std::runtime_error("Error: listen()");
  }
}
void Server::GetAddrInfo(const std::string& host, const int& port,
                         struct addrinfo** listp) {
  struct addrinfo hints;
  int status;

  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;
  hints.ai_flags |= AI_ADDRCONFIG;
  hints.ai_flags |= AI_NUMERICSERV;
  hints.ai_flags |= AI_NUMERICHOST;
  status =
      getaddrinfo(host.c_str(), std::to_string(port).c_str(), &hints, listp);
  if (status != 0) {
    throw std::runtime_error(gai_strerror(status));
  }
}
t_listening* Server::CreateListening(const std::string& host, const int& port,
                                     const int& fd) {
  t_listening* new_listening = new t_listening;
  new_listening->host = host;
  new_listening->port = port;
  new_listening->fd = fd;
  return new_listening;
}
bool Server::IsListenFd(const int& fd) {
  std::set<t_listening*>::iterator it = servers_.begin();
  for (; it != servers_.end(); ++it) {
    if (fd == (*it)->fd) {
      return true;
    }
  }
  return false;
}
void Server::DisConnect(const int& fd) {
  struct kevent event[3];

  EV_SET(&event[0], fd, EVFILT_READ, EV_DELETE, 0, 0, NULL);
  EV_SET(&event[1], fd, EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
  EV_SET(&event[2], fd, EVFILT_TIMER, EV_DELETE, 0, 0, NULL);
  kevent(kq_, event, 3, NULL, 0, NULL);
  clients_->DeleteByFd(fd);
  close(fd);
}

void Server::Get(int idx) {
  Data* client = reinterpret_cast<Data*>(events_[idx].udata);
  // const ServerConfigInfo* config = client->GetConfig();
  t_req_msg* req_msg = client->GetReqMessage();
  int client_fd = client->GetClientFd();
  struct kevent event;

  std::string req_url = req_msg->req_url_;
  std::string file_path;

  if (client->is_directory_list_ == true) {
    const std::string html_str = generate_directory_list(req_url);
    char* tmp_html_str = new char[html_str.size()];
    for (size_t i = 0; i < html_str.size(); ++i) {
      tmp_html_str[i] = html_str[i];
    }
    client->res_message_->status_code_ = 200;
    client->res_message_->body_data_.data_ = tmp_html_str;
    client->res_message_->body_data_.length_ = html_str.size();

    client->res_message_->headers_["Content-Type"] = "text/html";
    client->res_message_->headers_["Content-Length"] = to_string(html_str.size());

    // header 설정 이후 바로 client_fd EVFILT_WRITE ENABLE

  } else if (client->cgi_ == true) {
    cgi_manager_->SetData(client);
    cgi_manager_->SendToCGI(client, kq_);

  } else {
    int file_fd = open(req_msg->req_url_.c_str(), O_RDONLY);

    std::string file_name =
        req_msg->req_url_.substr(req_msg->req_url_.rfind('/'));
    size_t pos = file_name.rfind('.');

    if (pos == std::string::npos) {
      client->res_message_->headers_["Content-Type"] = "text/plain";
    } else {
      std::string file_extention = file_name.substr(pos + 1);
      if (file_extention == "html") {
        client->res_message_->headers_["Content-Type"] = "text/html";
      } else if (file_extention == "png") {
        client->res_message_->headers_["Content-Type"] = "image/png";
      } else if (file_extention == "jpg") {
        client->res_message_->headers_["Content-Type"] = "image/jpeg";
      } else if (file_extention == "txt") {
        client->res_message_->headers_["Content-Type"] = "ext/plain";
      } else if (file_extention == "py") {
        client->res_message_->headers_["Content-Type"] = "text/x-python";
      } else {
        // 지원하지 않는 타입.
      }
    }

    client->SetFileFd(file_fd);
    EV_SET(&event, file_fd, EVFILT_READ, EV_ADD, 0, 0, client);
    kevent(kq_, &event, 1, NULL, 0, NULL);
  }

  EV_SET(&event, client_fd, EVFILT_READ, EV_DISABLE, 0, 0, client);
  kevent(kq_, &event, 1, NULL, 0, NULL);
}





void Server::Post(int idx) {
  Data* client = reinterpret_cast<Data*>(events_[idx].udata);
  const ServerConfigInfo* config = client->config_;
  t_req_msg* req_msg = client->GetReqMessage();
  struct kevent event;

  std::string content_type = req_msg->headers_["Content-Type"];
  if (content_type == "application/x-www-form-urlencoded") {
    std::string encoded_string;
    std::string decoded_string;
    size_t len = req_msg->body_data_.length_;
    for (size_t i = 0; i < len; ++i)
      encoded_string.push_back(req_msg->body_data_.data_[i]);

    decoded_string = decode(encoded_string);

    std::string query = decoded_string;
    int and_pos = query.find('&');

    std::string title = query.substr(0, and_pos);
    int equal_pos = title.find('=');
    title = title.substr(equal_pos + 1, and_pos - equal_pos);

    std::string content = query.substr(and_pos);
    equal_pos = content.find('=');
    content = content.substr(equal_pos + 1);

    int file_fd = open((config->upload_path_ + title).c_str(),
                       O_WRONLY | O_CREAT | O_TRUNC);
    if (file_fd == -1) {
      // 오픈에러 처리
      std::cout << RED << "OPEN ERROR\n";
    }

    fchmod(file_fd, S_IRWXU | S_IRWXG | S_IRWXO);
    client->SetFileFd(file_fd);
    EV_SET(&event, file_fd, EVFILT_WRITE, EV_ADD, 0, 0, client);
    kevent(kq_, &event, 1, NULL, 0, NULL);
    client->post_data_ = content;

    client->SetStatusCode(201);


    and_pos = encoded_string.find('&');
    std::string encoded_title = encoded_string.substr(0, and_pos);
    equal_pos = encoded_title.find('=');
    encoded_title = encoded_title.substr(equal_pos + 1, and_pos - equal_pos);
    client->res_message_->headers_["Location"] = config->upload_path_ + encoded_title;
    client->res_message_->headers_["Content-Type"] = "text/plain; charset=UTF-8";
  } else {
    size_t semicolon_pos = content_type.find(';');
    std::string boundary = content_type.substr(semicolon_pos + 1);
    size_t equal_pos = boundary.find('=');
    boundary = boundary.substr(equal_pos + 1);
    content_type = content_type.substr(0, semicolon_pos);

    if (content_type == "multipart/form-data") {
      
    } else {
      client->SetStatusCode(501);
    }
  }
}


void Server::ReadFile(int idx) {
  Data* client = reinterpret_cast<Data*>(events_[idx].udata);
  int client_fd = client->GetClientFd();
  int file_fd = client->GetFileFd();

  int size = client->event_[idx].data;
  char* buf = new char[size];
  read(file_fd, buf, size);

  client->res_message_->headers_["Content-Length"] = to_string(size);
  client->res_message_->body_data_.data_ = buf;
  client->res_message_->body_data_.length_ = size;

  struct kevent event;
  EV_SET(&event, client_fd, EVFILT_WRITE, EV_ENABLE, 0, 0, client);
  kevent(kq_, &event, 1, NULL, 0, NULL);
}
void Server::WriteFile(int idx) {
  Data* client = reinterpret_cast<Data*>(events_[idx].udata);
  int client_fd = client->GetClientFd();
  int file_fd = client->GetFileFd();

  write(file_fd, client->post_data_.c_str(), client->post_data_.size());

  struct kevent event;
  EV_SET(&event, client_fd, EVFILT_WRITE, EV_ENABLE, 0, 0, client);
  kevent(kq_, &event, 1, NULL, 0, NULL);

  EV_SET(&event, file_fd, EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
  kevent(kq_, &event, 1, NULL, 0, NULL);
}

void Server::Send(int idx) {
  Data* client = reinterpret_cast<Data*>(events_[idx].udata);

  if (client->timeout_ == true) {
    client->res_message_->headers_["Connection"] = "close";
  } else {
    client->res_message_->headers_["Connection"] = "keep-alive";
  }



  msg_composer_->SetData(client);
  msg_composer_->InitResMsg(client);
  int client_fd = client->GetClientFd();
  int file_fd = client->GetFileFd();
  const char* response = msg_composer_->GetResponse(client);
  send(client_fd, response, msg_composer_->GetLength(), 0);
  delete[] response;
  response = NULL;

  struct kevent event;
  EV_SET(&event, client_fd, EVFILT_WRITE, EV_DISABLE, 0, 0, client);
  kevent(kq_, &event, 1, NULL, 0, NULL);

  EV_SET(&event, client_fd, EVFILT_READ, EV_ENABLE, 0, 0, client);
  kevent(kq_, &event, 1, NULL, 0, NULL);

  EV_SET(&event, file_fd, EVFILT_READ, EV_DELETE, 0, 0, NULL);
  kevent(kq_, &event, 1, NULL, 0, NULL);

  close(file_fd);
  client->is_working = false;
  // DisConnect(client_fd);
}
