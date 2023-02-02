#ifndef DATA_HPP
#define DATA_HPP

enum stage {
  READY,
  REQ_READY,
  REQ_FINISHED,
  GET_READY,
  GET_CGI,
  GET_FINISHED,
  POST_READY,
  POST_PROCESSING,
  POST_CHECKED,
  DELETE,
  RESPONSE
};

/* 포인터로 갖고있는 녀석들의 메모리 관리는 어디서 해줄지? */
class Data {
 public:
  Data();
  ~Data();
  int litsen_fd_;  // 어느 listen fd에 연결됐는지
  int port_;  // listen fd에 bind 되어있는 port 번호. config볼 때 필요
  // int client_fd_;
  struct kevent* event_;  // fd(ident), flag들
  // ServerConfigInfo* config_;
  // struct HTTPMessage* req_message_;  // HTTP 요청/응답 헤더 구분 어떻게?
  // struct HTTPMessage* res_message_;  // 같은 클래스로? 다른 클래스로?
  int status_code_;  // 상태코드 enum 정의 필요
  char* entity_;     // 응답 본문
  stage e_stage;
};

#endif
