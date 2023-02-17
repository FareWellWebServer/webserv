#include "../../include/Server.hpp"

void Server::Prompt(void) {
  std::cout << "\n-------- [ " BOLDBLUE << "FareWell Web Server Info" << RESET
            << " ] --------\n"
            << std::endl;

  std::set<t_listening*>::const_iterator it = servers_.begin();
  for (; it != servers_.end(); ++it) {
    std::cout << "socket: " << BOLDGREEN << (*it)->fd << RESET;
    std::cout << "   | host: " << BOLDGREEN << (*it)->host << RESET;
    std::cout << "   | port: " << BOLDGREEN << (*it)->port << RESET << std::endl
              << std::endl;
  }
  std::cout << "----------------------------------------------" << std::endl;
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

void Server::AcceptNewClient(int idx) {
  int connfd;
  socklen_t client_len;
  struct sockaddr_storage client_addr;
  char host[MAXBUF];
  char port[MAXBUF];
  struct kevent event[3];

  client_len = sizeof(client_addr);
  connfd =
      accept(events_[idx].ident,
             reinterpret_cast<struct sockaddr*>(&client_addr), &client_len);
  if (connfd == -1) {
    throw std::runtime_error("Error: accept()");
  }

  /* non-block 세팅 필요 */

  /* Data 만들기 */
  ServerConfigInfo* config =
      reinterpret_cast<ServerConfigInfo*>(events_[idx].udata);
  getnameinfo(reinterpret_cast<struct sockaddr*>(&client_addr), client_len,
              host, MAXBUF, port, MAXBUF, 0);
  clients_->AddData(events_[idx].ident, connfd, config->port_, host, port);
  clients_->SetEvent(&events_[idx]);
  clients_->SetConfig();
#if SERVER
  std::cout << "Connected to (" << host << ", " << port
            << "). socket : " << connfd << std::endl;
#endif
  /* event setting */
  EV_SET(&event[0], connfd, EVFILT_READ, EV_EOF | EV_ADD, 0, 0,
         clients_->GetData());
  EV_SET(&event[1], connfd, EVFILT_WRITE, EV_ADD | EV_DISABLE, 0, 0,
         clients_->GetData());
  EV_SET(&event[2], connfd, EVFILT_TIMER, EV_ADD, NOTE_SECONDS,
         config->timeout_, clients_->GetData());
  if (kevent(kq_, event, 3, NULL, 0, NULL) == -1) {
    throw std::runtime_error("Error: kevent()");
  }
}
