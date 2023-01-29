#include "../../include/Server.hpp"

#include <netdb.h>

Server::Server() : kq_(kqueue()) {}

Server::~Server(void) {}

void Server::Run(void) {
  if (servers_.size() == 0) {
    throw std::runtime_error("[Server Error]: no listening server");
  }
  Act();
}

void Server::Init(const std::vector<ServerConfigInfo> server_infos) {
  for (int i = 0; i < server_infos.size(); ++i) {
    Listen(server_infos[i].host, server_infos[i].port);
  }
}

// private

void Server::Act(void) {
  while (true) {
    int n = kevent(kq_, NULL, 0, events_, MAXLISTEN, NULL);
    if (n == -1) {
      throw std::runtime_error("Error: kevent()");
    }
    for (int idx = 0; idx < n; ++idx) {
      if (IsListenFd(events_[idx].ident) &&
          events_[idx].filter == EVFILT_READ) {
        AcceptNewClient(idx);
      } else {
        ActCoreLogic(idx);
      }
    }
  }
}

void Server::AcceptNewClient(int idx) {
  int connfd;
  socklen_t client_len;
  struct sockaddr_storage client_addr;
  int flags;
  char host[MAXBUF];
  int port;
  struct kevent event;

  client_len = sizeof(client_addr);
  connfd =
      accept(events_[idx].ident,
             reinterpret_cast<struct sockaddr*>(&client_addr), &client_len);
  if (connfd == -1 || errno != EWOULDBLOCK) {
    throw std::runtime_error("Error: accept()");
  }
  flags = fcntl(connfd, F_GETFL, 0);
  if (flags == -1) {
    throw std::runtime_error("Error: fcntl()");
  }
  flags |= O_NONBLOCK;
  if (fcntl(connfd, F_SETFL, flags) == -1) {
    throw std::runtime_error("Error: fcntl()");
  }
  EV_SET(&event, connfd, EVFILT_READ, EV_ADD, 0, 0, NULL);
  if (kevent(kq_, &event, 1, NULL, 0, NULL) == -1) {
    throw std::runtime_error("Error: kevent()");
  }
  getnameinfo(reinterpret_cast<struct sockaddr*>(&client_addr), client_len,
              host, MAXBUF, const_cast<char*>(std::to_string(port).c_str()),
              MAXBUF, 0);
  clients_.AddData(events_[idx].ident, connfd, port);
  std::cout << "Connected to (" << host << ", " << port << ")\n";
}

void Server::ActCoreLogic(int idx) {
  struct kevent event;

  EV_SET(&event, events_[idx].ident, EVFILT_READ, EV_DELETE, 0, 0, NULL);
  if (kevent(kq_, &event, 1, NULL, 0, NULL) == -1) {
    std::cerr << "Error: kevent()\n";
  }
  // TODO: call ReqHandler and run other process
  // TODO: call ResHandler and send data to client
  DisConnect(events_[idx].ident);
}

void Server::Listen(const std::string& host, const int& port) {
#if DG
  if (servers_.size() == MAXLISTEN) {
    std::cerr << "Error: full of listening\n";
    return;
  }
#endif

  struct addrinfo hints;
  struct addrinfo* listp;
  struct addrinfo* p;
  int listenfd;
  int status;
  int optval = ENABLE;
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
      throw std::runtime_error("failed at socket create");
    }
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR,
               reinterpret_cast<const void*>(&optval), sizeof(int));
    if (bind(listenfd, p->ai_addr, p->ai_addrlen) == 0) break;
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
  struct kevent event;
  EV_SET(&event, listenfd, EVFILT_READ, EV_ADD, 0, 0, NULL);
  if (kevent(kq_, &event, 1, NULL, 0, NULL) == -1) {
    throw std::runtime_error("Error: kevent()");
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