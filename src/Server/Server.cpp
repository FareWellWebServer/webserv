#include "../../include/Server.hpp"

Server::Server() : kq_(kqueue()) {}

Server::~Server(void) {}

void Server::Run(void) {
  // TODO: accept, bind
  // TODO: client응답을 받아서 '처리' 해서 응답을 보냄
}

void Server::Init(const std::vector<ServerConfigInfo> server_infos) {
  for (int i = 0; i < server_infos.size(); ++i) {
    Listen(server_infos[i].host, server_infos[i].port);
  }
}

// private

void Server::Listen(const std::string& host, const int& port) {
  if (servers_.size() == MAXLISTEN) {
    std::cerr << "Error: full of listening\n";
    return;
  }

  struct addrinfo hints, *listp, *p;
  int listenfd, status, optval = ENABLE;
  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;
  hints.ai_flags |= AI_ADDRCONFIG;
  hints.ai_flags |= AI_NUMERICSERV;
  hints.ai_flags |= AI_NUMERICHOST;
  status =
      getaddrinfo(host.c_str(), std::to_string(port).c_str(), &hints, &listp);

  for (p = listp; p; p = p->ai_next) {
    listenfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
    if (listenfd < 0) {
      continue;
    }
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR,
               reinterpret_cast<const void*>(&optval), sizeof(int));
    if (bind(listenfd, p->ai_addr, p->ai_addrlen) == 0) break;
    close(listenfd);
  }
  freeaddrinfo(listp);
  if (p == NULL) {
    std::cerr << "Error: socket()\n";
    return;
  }
  if (listen(listenfd, BACKLOG) < 0) {
    close(listenfd);
    std::cerr << "Error: listen()\n";
    return;
  }
  struct kevent event;
  EV_SET(&event, listenfd, EVFILT_READ, EV_ADD, 0, 0, NULL);
  if (kevent(kq_, &event, 1, NULL, 0, NULL) == -1) {
    std::cerr << "Error: kevent()\n";
    return;
  }
  servers_.insert(CreateListening(host, port, listenfd));
#if DG
  std::cout << host << " is listening port on " << port << "\n";
#endif
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
    return false;
  }
  return false;
}

void Server::DisConnect(const int& fd) {
  close(fd);
  clients_.DeleteByFd(fd);
}
