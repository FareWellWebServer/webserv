#ifndef REQHANDLER_HPP
#define REQHANDLER_HPP

#include <sys/stat.h>

#include <cstring>
#include <string>

#include "WebServ.hpp"

class ReqHandler {
 public:
  ReqHandler(void);
  ~ReqHandler(void);
  void SetClient(Data* client);
  void SetReadLen(int64_t kevent_data);
  ssize_t RecvFromSocket(void);
  void ParseRecv();
  void Clear();

  t_req_msg* req_msg_;
  int entity_flag_;

 private:
  char* buf_;
  Data* client_;
  int64_t read_len_;
  int64_t ParseFirstLine();
  int64_t ParseHeaders(int start_idx);
  void ParseHeadersSetKeyValue(char* line);
  void ParseEntity(int start_idx);
  void ValidateReq();
};

void PrintMap(std::map<std::string, std::string>& map);
void RemoveTabSpace(std::string& str);
void ReduceSlash(std::string& tmp);
bool CheckValidPath(const std::string& file_path);
std::vector<std::string> split(const std::string& s, char delimiter, int cnt);
#endif
