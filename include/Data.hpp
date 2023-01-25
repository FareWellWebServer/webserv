class Data {
 public:
  Data()
      : litsen_fd_(-1),
        port_(0),
        event_(NULL),
        config(NULL),
        req_message_(NULL),
        res_message_(NULL),
        status_code_(200),
        entity_(NULL) {}
  Data(const int& listen_fd)
      : litsen_fd_(listen_fd),
        port_(0),
        event_(NULL),
        config(NULL),
        req_message_(NULL),
        res_message_(NULL),
        status_code_(200),
        entity_(NULL) {}
  ~Data() {}
  void setEvent(struct kevent& event);
  bool setConfig(const struct config* config);
  void setReqMessage(HTTPMessage req_message);
  void setResMessage(HTTPMessage res_message);


 private:
  const int litsen_fd_;  // 어느 listen fd에 연결됐는지
  const int port_;  // listen fd에 bind 되어있는 port 번호. config볼 때 필요
  struct kevent* event_;  // fd(ident), flag들
  const struct config* config_;
  struct HTTPMessage* req_message_;  // HTTP 요청/응답 헤더 구분 어떻게?
  struct HTTPMessage* res_message_;  // 같은 클래스로? 다른 클래스로?
  int status_code_;                  // 상태코드
  char* entity_;                     // 응답 본문
};