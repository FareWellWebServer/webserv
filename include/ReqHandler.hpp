#ifndef REQHANDLER_HPP
#define REQHANDLER_HPP

#include <cstring>

#include "WebServ.hpp"

class ReqHandler {
 public:
  ReqHandler(void);
  ~ReqHandler(void);  // buf 확인하고 해제
  void SetClient(Data* client);
  void SetReadLen(int64_t kevent_data);
  void RecvFromSocket();
  void ParseRecv();
  void ParseRecv(int fd);
  // void SetReadLen(int64_t len);
  void SetBuf(int fd);
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
