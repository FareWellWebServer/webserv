#ifndef SERVER_HPP
#define SERVER_HPP

#include "ClientMetaData.hpp"

#include <arpa/inet.h> /* htons, htonl, ntohs, ntohl */
#include <fcntl.h>     /* fcntl */
#include <netdb.h>     /* getprotobyname */
#include <poll.h>      /* poll */
#include <sys/errno.h> /* errno */
#include <sys/event.h>
#include <sys/select.h> /* select */
#include <sys/socket.h> /* AF_INET, SOCK_STREAM, gai_strerror, socket, accept, listen, send, recv, bind, connect, getaddrinfo, freeaddrinfo, setsockopt, getsockname */
#include <sys/types.h>  /* kqueue kevent */
#include <unistd.h>     /* execve, dup, dup2, pipe */
#include <cstring> /* memset, strerror */
#include <iostream>
#include <set>

#define MAXLINE 1000000
#define MAXBUF 1000000
#define MAXLISTEN 10
#define BACKLOG 128
#define DISABLE 0
#define ENABLE 1

typedef struct s_litening {
  std::string host;
  int port;
  int fd;
} t_listening;

class Server {
 public:
  Server(void);
  ~Server(void);

  void Run(void);
  void Init(const std::vector<ServerConfigInfo>& server_infos);

 private:
  int kq_;
  struct kevent events_[MAXLISTEN + BACKLOG];
  std::set<t_listening*> servers_;
  ClientMetaData clients_;

  void Act(void);
  void AcceptNewClient(int idx);
  void ActCoreLogic(int idx);
  void SetHostPortAvaiable(const std::string& host, const int& port);
  void BindListen(const std::string& host, const int& port, int& listenfd);
  void GetAddrInfo(const std::string& host, const int& port,
                   struct addrinfo** listp);
  t_listening* CreateListening(const std::string& host, const int& port,
                               const int& fd);
  bool IsListenFd(const int& fd);
  void DisConnect(const int& fd);
};

#endif
