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
  void RecvFromSocket(void);
  void ParseRecv(void);
  void ParseRecv(int fd);
  // void SetReadLen(int64_t len);
  void SetBuf(int fd);
  void Clear();



  t_req_msg* req_msg_;
  int entity_flag_;

 private:
  char* buf_;  // 동적할당 / 해제
  Data* client_;
  int64_t read_len_;
  int64_t ParseFirstLine();
  int64_t ParseHeaders(int start_idx);
  void ParseHeadersSetKeyValue(char* line);
  void ParseEntity(int start_idx);
  void ValidateReq(void);
};

void ReduceSlash(std::string& tmp);
void PrintMap(std::map<std::string, std::string>& map);
void RemoveTabSpace(std::string& str);
std::vector<std::string> split(const std::string& s, char delimiter, int cnt);
#endif
