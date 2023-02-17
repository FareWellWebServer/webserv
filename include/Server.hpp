#ifndef SERVER_HPP
#define SERVER_HPP

#include <arpa/inet.h> /* htons, htonl, ntohs, ntohl */
#include <fcntl.h>     /* fcntl */
#include <netdb.h>     /* getprotobyname */
#include <netdb.h>
#include <poll.h> /* poll */
#include <signal.h>
#include <stdlib.h>
#include <sys/errno.h> /* errno */
#include <sys/event.h>
#include <sys/select.h> /* select */
#include <sys/socket.h> /* AF_INET, SOCK_STREAM, gai_strerror, socket, accept, listen, send, recv, bind, connect, getaddrinfo, freeaddrinfo, setsockopt, getsockname */
#include <sys/types.h>  /* kqueue kevent */
#include <unistd.h>     /* execve, dup, dup2, pipe */

#include <cstring> /* memset, strerror */
#include <iostream>
#include <set>
#include <vector>

#include "CGIManager.hpp"
#include "ClientMetaData.hpp"
#include "MsgComposer.hpp"
#include "ReqHandler.hpp"
#include "ServerConfigInfo.hpp"
#include "Session.hpp"
#include "Utils.hpp"

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
  Server(const Config& config);
  ~Server(void);
  void Run(void);
  void Act(void);

  void Prompt(void);
  void Init(void);
  void BindListen(const std::string& host, const int& port, int& listenfd);
  void GetAddrInfo(const std::string& host, const int& port,
                   struct addrinfo** listp);
  t_listening* CreateListening(const std::string& host, const int& port,
                               const int& fd);
  bool IsListenFd(const int& fd);
  void DisConnect(const int& fd);
  void AcceptNewClient(int idx);

  void Get(int idx);
  void Post(int idx);
  void Delete(int idx);
  void HandleChunkedData(int idx);

  void ExecuteReadEvent(const int& idx);
  void ExecuteReadEventFileFd(int idx);
  void ExecuteReadEventPipeFd(int idx);
  void ExecuteReadEventClientFd(const int& idx);
  void Pong(int idx);
  void ActClientCoreLogic(int idx);

  void ExecuteWriteEvent(const int& idx);
  void ExecuteWriteEventFileFd(int idx);
  void ExecuteWriteEventPipeFd(int idx);
  void ExecuteWriteEventClientFd(int idx);

  void ExecuteTimerEvent(const int& idx);
  void ExecuteProcessEvent(const int& idx);
  void ExecuteLogEvent(void);

 private:
  int kq_;
  std::vector<ServerConfigInfo> server_infos_;
  Logger logger_;
  struct kevent events_[MAXLISTEN + BACKLOG];
  std::set<t_listening*> servers_;
  ClientMetaData* clients_;
  ReqHandler* req_handler_;
  MsgComposer* msg_composer_;
  CGIManager* cgi_manager_;
  Session* session_;
};

#endif
