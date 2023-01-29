#ifndef ReqHandler_HPP
#define ReqHandler_HPP

#include "WebServ.hpp"

// typedef struct entity {
//   char* entity_;
//   size_t entity_length_;
// } entity_t;

typedef struct t_req_msg {
  std::string method_;
  std::string req_url_;
  std::string protocol_;
  std::map<std::string, std::string> headers_;
  entity_t body_data_;
} s_req_msg;

class ReqHandler {
 public:
  s_req_msg req_msg_;
  void print_structure();
  ReqHandler(void);
  ~ReqHandler(void);

 private:
};

// void Parse_Req_msg(int c_socket);
void Parse_Req_Msg(int c_socket, ReqHandler& reqhandle);

void Parse_Req_Msg(int c_socket, Data& m_data);

std::vector<std::string> split(const std::string& s, char delimiter, int cnt);
std::vector<std::string> s_split(std::string& s, const std::string& delimiter,
                                 int cnt);
void Remove_Tab_Space(std::string& str);
void Print_Map(std::map<std::string, std::string>& map);

#endif