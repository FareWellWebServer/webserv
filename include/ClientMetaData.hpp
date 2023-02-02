/* fd 있는 버전
#ifndef CLIENTMETADATA_HPP
#define CLIENTMETADATA_HPP
#include <map>

 * @brief
 * kevent와 config의 new | free는 여기서 하지 않음
 * req&res_message 는 관리하는거 상황 봐서 어디서 free할지 정하기

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

ClientMetaData::ClientMetaData() {}

ClientMetaData::~ClientMetaData() {}

#endif

*/

/* fd 없는 버전 */
#ifndef CLIENTMETADATA_HPP
#define CLIENTMETADATA_HPP
#include "WebServ.hpp"

#include <sys/event.h>
#include <algorithm>
#include <exception>
#include <map>

// #include "config"
// #include "http parser"

class ReqHandler;
// 테스트 끝나면 주석풀기 0130_0019

// typedef struct entity {
//   char* entity_;
//   size_t entity_length_;
// } entity;

class ServerConfigInfo;

typedef struct Data  // struct로
{
  int litsen_fd_;  // 어느 listen fd에 연결됐는지
  int port_;  // listen fd에 bind 되어있는 port 번호. config볼 때 필요
  // int client_fd_;
  struct kevent* event_;  // fd(ident), flag들
  ServerConfigInfo* config_;
  ReqHandler* req_message_;
  struct HTTPMessage* res_message_;  // 같은 클래스로? 다른 클래스로?
  int status_code_;                  // 상태코드 enum 정의 필요

  t_entity* entity_;
} Data;

class ClientMetaData {
 public:
  typedef int fd;

 private:
  std::map<fd, Data> datas_;
  int current_fd_;
  void ValidCheckToAccessData();
  class WrongFd : public std::exception {
   public:
    const char* what() const throw();
  };
  void InitializeData(Data* data);

 public:
  ClientMetaData();
  ~ClientMetaData();
  // accept 된 client 의 kevent 발생시 설정
  void SetCurrentFd(const fd& client_fd);

  // kevent에서 listen fd가 read 했을 때, accept할 때, listen_fd저장, port번호
  // 저장
  void AddData(const fd& listen_fd, const fd& client_fd, const fd& port);
  void SetConfig(struct kevent& event);

  // accept 된 fd를 close할때, map에서 지워줌
  void DeleteByFd(const fd& client_fd);

  // reqHandler에서 요청헤더 파싱 후
  void SetReqMessage(struct HTTPMessage* header);

  // MsgComposer에서
  void SetResMessage(struct HTTPMessage* header);

  // core에서 처리 후
  void SetEntity(char* entitiy);

  // data 통채로 원할 때
  Data& GetData();

  // core에서 요청 헤더 데이터 필요할 때
  struct HTTPMessage* GetReqHeader();

  // core에서 응답 헤더 데이터 필요할 때
  struct HTTPMessage* GetResHeader();

  // core에서 헤더 데이터 필요할 때
  ServerConfigInfo* GetConfig();

  int GetPort();

  int GetDataCount(void);  // return data_.size();

  // 루프 처음시작할때 errorHandler에 *나& 로 넣어주면서 인스턴스화 해도 될듯?
  // 그러면 한번만 호출해도 되니까

  void SetStatusCode(int status_code);

  int GetStatusCode();

  // Method 전체 리턴
  std::vector<std::string> GetMethods();

  // Method 있는지 확인
  bool FindMethods(std::string method);

  char* GetURL();
};

#endif
