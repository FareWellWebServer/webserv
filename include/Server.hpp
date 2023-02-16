#ifndef SERVER_HPP
#define SERVER_HPP

#include <signal.h>

#include <arpa/inet.h> /* htons, htonl, ntohs, ntohl */
#include <fcntl.h>     /* fcntl */
#include <netdb.h>     /* getprotobyname */
#include <netdb.h>
#include <poll.h> /* poll */
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
  Server(const Config& config);
  ~Server(void);

  void Run(void);
  void Init(void);
  void Prompt(void);

  // 임의로 public에 나둠 나중에 setter구현해야함
  // server_info를 Method_Processor 호출할 때, 필요하기 때문에
  // Class 내부에 저장함.
  std::vector<ServerConfigInfo> server_infos_;

  /* ----- METHOD -----*/

  void Get(int idx);
  void Post(int idx);
  void Delete(int idx);
  void HandleChunkedData(int idx);
  void ExecuteReadEventFileFd(int idx);
  void ExecuteReadEventPipeFd(int idx);
  void ExecuteWriteEventFileFd(int idx);
  void ExecuteWriteEventPipeFd(int idx);
  void ExecuteWriteEventClientFd(int idx); // Send()를 이거로 바꿈

  void Pong(int idx);

  /* ------------------*/
 private:
  int kq_;
  Logger logger_;
  struct kevent events_[MAXLISTEN + BACKLOG];
  std::set<t_listening*> servers_;
  ClientMetaData* clients_;
  ReqHandler* req_handler_;
  MsgComposer* msg_composer_;
  CGIManager* cgi_manager_;
  Session* session_;


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
  void ExecuteReadEvent(const int& idx);
  void ExecuteReadEventClientFd(const int& idx);
  void ExecuteWriteEvent(const int& idx);
  void ExecuteTimerEvent(const int& idx);
  void ExecuteLogEvent(void);
};

#endif
