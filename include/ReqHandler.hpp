#ifndef REQHANDLER_HPP
#define REQHANDLER_HPP

#include <cstring>

#include "WebServ.hpp"

// 테스트 끝나면 지우기

// TODO : type 추가
typedef struct s_entity {
  char* entity_;
  size_t entity_length_;
  std::string type_;
} t_entity;

typedef struct s_req_msg {
  std::string method_;
  std::string req_url_;
  std::string protocol_;  // 프로토콜 값을 저장하지 말고 파싱딴에서 HTTP/1.1이
                          // 아니면 에러.
  std::map<std::string, std::string> headers_;
  t_entity body_data_;
} t_req_msg;

class ReqHandler {
 public:
  ReqHandler(void);
  ~ReqHandler(void);  // buf 확인하고 해제
  void SetClient(Data* client);
  void SetReadLen(int64_t kevent_data);
  void RecvFromSocket();
  void ParseRecv();
  t_req_msg*
  PopReqMassage();  // 동적할당해서 나가고, 받은 쪽에서 delete 처리해주기
  void Clear();
  t_req_msg* req_msg_;

 private:
  char* buf_;  // 동적할당 / 해제
  int64_t read_len_;
  Data* client_;
  int64_t ParseFirstLine();
  int64_t ParseHeaders(int start_idx);
  void ParseHeadersSetKeyValue(char* line);
  void ParseEntity(int start_idx);
};

void Print_Map(std::map<std::string, std::string>& map);
void Remove_Tab_Space(std::string& str);
std::vector<std::string> split(const std::string& s, char delimiter, int cnt);
#endif