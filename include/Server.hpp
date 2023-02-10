#ifndef SERVER_HPP
#define SERVER_HPP

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
#include <stdlib.h>
#include <netdb.h>

#include <cstring> /* memset, strerror */
#include <iostream>
#include <set>
#include <vector>

#include "ClientMetaData.hpp"
#include "MsgComposer.hpp"
#include "ReqHandler.hpp"
#include "ResHandler.hpp"
#include "ServerConfigInfo.hpp"
#include "CGIManager.hpp"
#include "Utils.hpp"

#define MAXLINE 1000000
#define MAXBUF 1000000
#define MAXLISTEN 10
#define BACKLOG 128
#define DISABLE 0
#define ENABLE 1

// #define LISTEN_FD 255
// #define CLIENT_FD 256
// #define FILE_FD 257
// #define PIPE_FD 258

// typedef struct s_fd_info {
// 	int which_fd;
// 	Data *parent;
// } t_fd_info;

typedef struct s_litening {
  std::string host;
  int port;
  int fd;
} t_listening;

class Server {
 public:
  // Server(void);
  Server(std::vector<ServerConfigInfo>);
  ~Server(void);

  void Run(void);
  // void Init(const std::vector<ServerConfigInfo>& server_infos);
  void Init(void);

  // 임의로 public에 나둠 나중에 setter구현해야함
  // server_info를 Method_Processor 호출할 때, 필요하기 때문에
  // Class 내부에 저장함.
  std::vector<ServerConfigInfo> server_infos_;

  /* ----- METHOD -----*/

  void Get(int idx);
  void Post(int idx);
  void ReadFile(int idx);
  void WriteFile(int idx);
  void Send(int idx);

  /* ------------------*/
 private:
  int kq_;
  //  TODO: logger fd
  struct kevent events_[MAXLISTEN + BACKLOG];
  std::set<t_listening*> servers_;
  ClientMetaData* clients_;
  ReqHandler* req_handler_;
  ResHandler* res_handler_;
  MsgComposer* msg_composer_;
  CGIManager* cgi_manager_;

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
