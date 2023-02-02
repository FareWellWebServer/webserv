#ifndef DATA_HPP
#define DATA_HPP

#include "Types.hpp"
#include "ServerConfigInfo.hpp"
#include "ResHandler.hpp"
#include "ReqHandler.hpp"
#include "HTTPMessage.hpp"

class ResHandler;
class ReqHandler;

/* 포인터로 갖고있는 녀석들의 메모리 관리는 어디서 해줄지? */
class Data {
 public:
  Data();
  ~Data();
  int litsen_fd_;  // 어느 listen fd에 연결됐는지
  int port_;  // listen fd에 bind 되어있는 port 번호. config볼 때 필요
  // int client_fd_;
  struct kevent* event_;  // fd(ident), flag들
  ServerConfigInfo* config_;
  ReqHandler* req_message_;
  ResHandler* res_message_;
  int status_code_;  // 상태코드 enum 정의 필요
  t_entity* entity_;     // 응답 본문
  stage e_stage;
};

#endif
