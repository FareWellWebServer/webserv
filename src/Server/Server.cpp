#include "../../include/Server.hpp"


#include <netdb.h>

// Server::Server() : kq_(kqueue()) {}
Server::Server(std::vector<ServerConfigInfo> server_info) 
: server_infos_(server_info), kq_(kqueue()) {
  // mp_ = new MethodProcessor(server_info);
  clients_ = new ClientMetaData;
  req_handler_ = new ReqHandler;
  res_handler_ = new ResHandler;
  msg_composer_ = new MsgComposer;
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
      EV_SET(&event, listenfd, EVFILT_READ, EV_ADD, 0, 0, (void *)&server_infos_[i]);
      // EV_SET(&event, listenfd, EVFILT_READ, EV_ADD, 0, 0, NULL);
      if (kevent(kq_, &event, 1, NULL, 0, NULL) == -1) {
        throw std::runtime_error("Error: kevent()");
      }
			t_listening* tmp = CreateListening(server_infos_[i].host_, server_infos_[i].port_, listenfd);
      servers_.insert(tmp);
			// delete tmp;
    #if SERVER
      std::cout << server_infos_[i].host_ << " is listening port on " << server_infos_[i].port_ << "\n";
    #endif
  }
}
	// 임의로 public에 나둠 나중에 setter구현해야함


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
		Data* client = reinterpret_cast<Data *>(events_[idx].udata);
    int event_fd = events_[idx].ident;
    if (events_[idx].filter == EVFILT_READ) {
      /* accept 된 port로 request 요청메세지 들어옴 */
      if (event_fd == client->GetClientFd()) {
        #if SERVER
          std::cout << "[Server] Client READ fd : " << client->GetClientFd() << std::endl;
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
          std::cout << "[Server] File READ fd : " << event_fd << " == " << client->GetFileFd() << std::endl;
        #endif
        // 
				ReadFile(idx);
      }
      /* CGI에게 반환받는 pipe[READ]에 대한 이벤트 */
      else if (event_fd == client->GetPipeRead()) {
        #if SERVER
          std::cout << "[Server] Pipe READ fd : " << event_fd << " == " << client->GetPipeRead() << std::endl;
        #endif
        // pipe 읽기
      }
    }
    else if (events_[idx].filter == EVFILT_WRITE) {
      /* response 보낼 때에 대한 이벤트 */
      if (event_fd == client->GetClientFd()) {
        #if SERVER
          std::cout << "[Server] Client Write : " << client->GetClientFd() << std::endl;
        #endif
				Send(idx);
				client->Clear();
      }
      /* POST, PUT file에 대한 write 발생시 */
      else if (event_fd == client->GetFileFd()) {

      }
      /* CGI에게 보내줄 pipe[WRITE]에 대한 이벤트 */
      else if (event_fd == client->GetPipeRead()) {

      }
      /* log file에 대한 write 발생시 */
      else if (event_fd == client->GetLogFileFd())
        ;
    }
    else if (events_[idx].flags == EV_EOF) {
      /* socket이 닫혔을 때 */
      if (event_fd == client->GetClientFd()) {

      }
      /* file, pipe 등이 예상치 못하게 닫혔을 경우도 있을지 나중에 보기 */
      else {

      }
    }
    // /* timeout 발생시 */
    else if (events_[idx].filter == EVFILT_TIMER) {
    }
		client = NULL;
	}
}

void Server::ActCoreLogic(int idx) {
	req_handler_->SetClient(clients_->GetDataByFd(events_[idx].ident));
	req_handler_->SetReadLen(events_[idx].data);
  if (events_[idx].data == 0)
  {
    std::cout << RED << "req len is 0\n" << RESET;
    return;
  }
	req_handler_->RecvFromSocket();
	req_handler_->ParseRecv();

	// clients_->SetReqMessageByFd(req_handler_->req_msg_, events_[idx].ident);
	Data* client = reinterpret_cast<Data*>(events_[idx].udata);


	client->SetReqMessage(req_handler_->req_msg_);

	// client->SetReqMessage(req_handler_->req_msg_);
	// std::cout << req_handler_->req_msg_->method_ << " " << req_handler_->req_msg_->req_url_ << "\n";
	// std::map<std::string, std::string>::iterator it = req_handler_->req_msg_->headers_.begin();
	// for(; it != req_handler_->req_msg_->headers_.end(); ++it) {
	// 	std::cout << it->first << ": " << it->second << "\n";
	// }

	// for(size_t i = 0; i < req_handler_->req_msg_->body_data_.length_; ++i)
	// 	std::cout << req_handler_->req_msg_->body_data_.data_[i];
	// std::cout << "\n";
	req_handler_->Clear();


	// Data* client = clients_->GetDataByFd(events_[idx].ident);
	if (client->GetReqMethod() == "GET")
  {
		Get(idx);
  }



	// DisConnect(events_[idx].ident);

}

void Server::AcceptNewClient(int idx) {
  int connfd;
  socklen_t client_len;
  struct sockaddr_storage client_addr;
  char host[MAXBUF];
  char port[MAXBUF];
  struct kevent event[2];

  client_len = sizeof(client_addr);
  connfd = accept(events_[idx].ident, 
          reinterpret_cast<struct sockaddr*>(&client_addr), &client_len);
  if (connfd == -1) {
    throw std::runtime_error("Error: accept()");
  }

  /* non-block 세팅 필요 */

  /* Data 만들기 */
  getnameinfo(reinterpret_cast<struct sockaddr*>(&client_addr), client_len, 
              host, MAXBUF, port, MAXBUF, 0);
  clients_->AddData(events_[idx].ident, connfd,
                (reinterpret_cast<ServerConfigInfo *>(events_[idx].udata))->port_,
                host, port);
  clients_->SetEvent(&events_[idx]);
  clients_->SetConfig();
  std::cout << "Connected to (" << host << ", " << port << "). socket : " << connfd << std::endl;

  /* event setting */
  EV_SET(&event[0], connfd, EVFILT_READ, EV_ADD, 0, 0, clients_->GetData());
  EV_SET(&event[1], connfd, EVFILT_WRITE, EV_ADD | EV_DISABLE, 0, 0, clients_->GetData()); // 추가
  if (kevent(kq_, event, 2, NULL, 0, NULL) == -1) {
    throw std::runtime_error("Error: kevent()");
  }
}
void Server::BindListen(const std::string& host, const int& port, int& listenfd) {
  struct addrinfo* listp;
  struct addrinfo* p;
  int optval = ENABLE;
	GetAddrInfo(host, port, &listp);
  for (p = listp; p; p = p->ai_next) {
    listenfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
    if (listenfd < 0) {
      throw std::runtime_error("failed at socket create");
    }
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const void*>(&optval), sizeof(int));
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
void Server::GetAddrInfo(const std::string& host, const int& port, struct addrinfo** listp) {
  struct addrinfo hints;
  int status;

  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;
  hints.ai_flags |= AI_ADDRCONFIG;
  hints.ai_flags |= AI_NUMERICSERV;
  hints.ai_flags |= AI_NUMERICHOST;
  status = getaddrinfo(host.c_str(), std::to_string(port).c_str(), &hints, listp);
  if (status != 0) {
    throw std::runtime_error(gai_strerror(status));
  }
}
t_listening* Server::CreateListening(const std::string& host, const int& port, const int& fd) {
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
  clients_->DeleteByFd(fd);
	close(fd);
}





/* ----- seojin ----- */

void Server::Get(int idx) {
	// int client_fd = events_[idx].ident;

	Data* client = reinterpret_cast<Data*>(events_[idx].udata);
	// t_req_msg* req_msg = client->GetReqMessage();
	// (void) req_msg;
	const ServerConfigInfo* info = client->config_;
	// std::cout << client->GetListenFd() << "\n";
	// std::cout << client->GetListenPort() << "\n";
	// std::cout << req_msg->req_url_ << "\n";
	// std::cout << info->root_path_ << "\n";

	std::string file_path = info->root_path_ + "index.html";
	file_path = "./" + file_path;
	
	// std::cout << "file_path: " << file_path << "\n";
	int file_fd = open(file_path.c_str(), O_RDONLY);
  int client_fd = client->GetClientFd();
	// std::cout << "file_fd: " << file_fd << "\n";
	client->SetFileFd(file_fd);
	
	struct kevent event;
	EV_SET(&event, file_fd, EVFILT_READ, EV_ADD, 0, 0, client);
	kevent(kq_, &event, 1, NULL, 0, NULL);

  EV_SET(&event, client_fd, EVFILT_READ, EV_DISABLE, 0, 0, client);
  kevent(kq_, &event, 1, NULL, 0, NULL);

}




void Server::ReadFile(int idx) {
	int file_fd = events_[idx].ident;
	Data* client = reinterpret_cast<Data*>(events_[idx].udata);
	// t_req_msg* req_msg = client->GetReqMessage();

	int size = client->event_[idx].data;
	char* buf = new char[size];
	read(file_fd, buf, size);


	client->SetMethodEntityData(buf);
	client->SetMethodEntityLength(size);
	client->SetMethodEntityType(strdup("text/html"));


	struct kevent event;
	EV_SET(&event, client->GetClientFd(), EVFILT_WRITE, EV_ENABLE, 0, 0, client);
	kevent(kq_, &event, 1, NULL, 0, NULL);
}

void Server::Send(int idx) {
	Data* client = reinterpret_cast<Data*>(events_[idx].udata);

	msg_composer_->SetData(client);
	msg_composer_->InitResMsg();
	int client_fd = client->GetClientFd();
	int file_fd = client->GetFileFd();
	const char* response = msg_composer_->GetResponse();
	send(client_fd, response, msg_composer_->getLength(), 0);
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
	DisConnect(client_fd);
}
