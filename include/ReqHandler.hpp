#ifndef ReqHandler_HPP
#define ReqHandler_HPP

#include "WebServ.hpp"

typedef struct t_req_enti {
  size_t length;
  char* entity;
} s_req_enti;

typedef struct t_req_msg {
  std::string method;
  std::string req_url;
  std::string protocol;
  std::map<std::string, std::string> headers;
  s_req_enti body_data;
} s_req_msg;

class ReqHandler {
 private:
 public:
  s_req_msg req_msg;
  void print_structure();
  ReqHandler(void);
  ~ReqHandler(void);
};

// void Parse_Req_msg(int c_socket);
void Parse_Req_msg(int c_socket, ReqHandler& reqhandle);
std::vector<std::string> split(const std::string& s, char delimiter, int cnt);
std::vector<std::string> s_split(std::string& s, const std::string& delimiter,
                                 int cnt);
void Remove_Tab_Space(std::string& str);
void Print_Map(std::map<std::string, std::string>& map);

#endif