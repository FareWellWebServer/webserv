#ifndef CLIENTMETADATA_HPP
#define CLIENTMETADATA_HPP
#include <map>

/**
 * @brief
 * kevent와 config의 new | free는 여기서 하지 않음
 * req&res_message 는 관리하는거 상황 봐서 어디서 free할지 정하기
 */
typedef struct s_data {
  const int litsen_fd_;  // 어느 listen fd에 연결됐는지
  const int port_;  // listen fd에 bind 되어있는 port 번호. config볼 때 필요
  struct kevent* event_;  // fd(ident), flag들
  const struct config* config_;
  struct HTTPMessage* req_message_;  // HTTP 요청/응답 헤더 구분 어떻게?
  struct HTTPMessage* res_message_;  // 같은 클래스로? 다른 클래스로?
  int status_code_;                  // 상태코드
  char* entity_;                     // 응답 본문

} data;

class ClientMetaData {
 private:
  std::map<int, data> data_;

 public:
  ClientMetaData();
  bool Init(const int& listen_fd);
  // kevent에서 listen fd가 read 했을 때, listen_fd저장, port번호 저장

  bool Insert(struct kevent& event, const struct config* config);
  // kevent에서 accept된 fd가 read했을 때

  bool Delete(const int& client_fd);
  // accept 된 fd를 close할때, map에서 지워줌

  bool SetReqMessage(const int& client_fd, struct HTTPMessage* header);
  // reqHandler에서 요청헤더 파싱 후

  bool SetResMessage(const int& client_fd, struct HTTPMessage* header);
  // core에서 처리 후

  data* GetDataByFd(const int& client_fd);
  // data 통채로 원할 때

  struct HTTPMessage* GetReqHeaderByFd(const int& client_fd);
  // core에서 요청 헤더 데이터 필요할 때

  struct HTTPMessage* GetResHeaderByFd(const int& client_fd);
  // core에서 응답 헤더 데이터 필요할 때

  struct config* GetConfigByFd(const int& client_fd);
  // core에서 헤더 데이터 필요할 때

  char* GetPortByFd(const int& client_fd);

  int GetDataCount(void);  // return data_.size();

  int GetStatusCodeByFd(const int& client_fd);  // error 확인가능.
  // 루프 처음시작할때 errorHandler에 *나& 로 넣어주면서 인스턴스화 해도 될듯?
  // 그러면 한번만 호출해도 되니까
  char* GetMethodByFd(const int& client_fd);

  char* GetURLByFd(const int& client_fd);

  ~ClientMetaData();
};

ClientMetaData::ClientMetaData(/* args */) {}

ClientMetaData::~ClientMetaData() {}

#endif