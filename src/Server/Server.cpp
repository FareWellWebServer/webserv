#include "../../include/Server.hpp"


#include <netdb.h>

// Server::Server() : kq_(kqueue()) {}
Server::Server(std::vector<ServerConfigInfo> server_info) : server_infos_(server_info), mp_(server_info), kq_(kqueue()) {}
Server::~Server(void) {
  close(kq_);
  servers_.clear();
}

void Server::Run(void) {
  if (servers_.size() == 0) {
    throw std::runtime_error("[Server Error]: no listening server");
  }
  while (true) {
    Act();
  }
}

// void Server::Init(const std::vector<ServerConfigInfo>& server_infos) {
void Server::Init(void) {
  for (size_t i = 0; i < server_infos_.size(); ++i) {
    SetHostPortAvaiable(server_infos_[i].host_, server_infos_[i].port_);
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
		int which_fd = reinterpret_cast<t_fd_info *>(events_[idx].udata)->which_fd;
		int16_t filter = events_[idx].filter;

    if (which_fd == LISTEN_FD) {
      AcceptNewClient(idx);
    } else if (which_fd == CLIENT_FD && filter == EVFILT_READ) { 
			ActCoreLogic(idx);
    } else if (which_fd == CLIENT_FD && filter == EVFILT_WRITE) {
			DisConnect(events_[idx].ident);
		} else if (which_fd == CLIENT_FD && events_[idx].flags == EV_EOF) {
			DisConnect(events_[idx].ident);
		} else if (which_fd == CLIENT_FD && filter == EVFILT_TIMER) {
			DisConnect(events_[idx].ident);
		} else if ((which_fd == FILE_FD || which_fd == PIPE_FD) && EVFILT_READ) {
			DisConnect(events_[idx].ident);
		} else if ((which_fd == FILE_FD || which_fd == PIPE_FD) && EVFILT_WRITE) {
			DisConnect(events_[idx].ident);
		}
	}
}

void Server::AcceptNewClient(int idx) {
  int connfd;
  socklen_t client_len;
  struct sockaddr_storage client_addr;
  char host[MAXBUF];
  char port[MAXBUF];
  struct kevent event;

  client_len = sizeof(client_addr);
  connfd = accept(events_[idx].ident, reinterpret_cast<struct sockaddr*>(&client_addr), &client_len);
  if (connfd == -1) {
    throw std::runtime_error("Error: accept()");
  }

	t_fd_info udata;
	udata.parent = NULL;
	udata.which_fd = CLIENT_FD;

  EV_SET(&event, connfd, EVFILT_READ, EV_ADD, 0, 0, &udata);
  if (kevent(kq_, &event, 1, NULL, 0, NULL) == -1) {
    throw std::runtime_error("Error: kevent()");
  }

  getnameinfo(reinterpret_cast<struct sockaddr*>(&client_addr), client_len, host, MAXBUF, port, MAXBUF, 0);
  clients_.AddData(events_[idx].ident, connfd, atoi(port));
  std::cout << "Connected to (" << host << ", " << port << "). socket : " << connfd << std::endl;
}

void Server::ActCoreLogic(int idx) {
  ReqHandler rq;
	rq.SetClient(&clients_.GetDataByFd(events_[idx].ident));
	rq.SetReadLen(events_[idx].data);
	rq.RecvFromSocket();
	rq.ParseRecv();

	clients_.SetReqMessageByFd(rq.req_msg_, events_[idx].ident);
	std::cout << rq.req_msg_->method_ << " " << rq.req_msg_->req_url_ << "\n";
	std::map<std::string, std::string>::iterator it = rq.req_msg_->headers_.begin();
	for(; it != rq.req_msg_->headers_.end(); ++it) {
		std::cout << it->first << ": " << it->second << "\n";
	}

	// clients_.GetDataByFd(events_[idx].ident).e_stage = GET_HTML;
	// mp_.GETFirst(events_[idx].ident, &clients_, events_[idx]);
	// mp.GET(events_[idx].ident, clients_.GetDataByFd(events_[idx].ident), events_[idx], clients_.GetReqMsgByFd(events_[idx].ident));


	

  // TODO: call ReqHandler and run other process
  // TODO: call ResHandler and send data to client
  DisConnect(events_[idx].ident);
}

void Server::SetHostPortAvaiable(const std::string& host, const int& port) {
#if SERVER
  if (servers_.size() == MAXLISTEN) {
    std::cerr << "Error: full of listening\n";
    return;
  }
#endif
  struct kevent event;
  int listenfd;

  BindListen(host, port, listenfd);

	t_fd_info udata;
	udata.parent = NULL;
	udata.which_fd = LISTEN_FD;
  EV_SET(&event, listenfd, EVFILT_READ, EV_ADD, 0, 0, &udata);
  if (kevent(kq_, &event, 1, NULL, 0, NULL) == -1) {
    throw std::runtime_error("Error: kevent()");
  }
  servers_.insert(CreateListening(host, port, listenfd));
#if SERVER
  std::cout << host << " is listening port on " << port << "\n";
#endif
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
  clients_.DeleteByFd(fd);
  close(fd);
}
