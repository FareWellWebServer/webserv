#ifndef CLIENTMETADATA_HPP
#define CLIENTMETADATA_HPP

#include <sys/event.h>
#include <algorithm>
#include <exception>
#include <map>
#include "ServerConfigInfo.hpp"
#include "Data.hpp"

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
