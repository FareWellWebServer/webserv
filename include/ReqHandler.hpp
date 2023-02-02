#ifndef REQ_HANDLER_HPP
#define REQ_HANDLER_HPP

#include "Data.hpp"
#include "HTTPMessage.hpp"
#include "GetNextLine.hpp"
#include "Color.hpp"

class Data;

// 테스트 끝나면 지우기
class ReqHandler {
 public:
  t_req_msg req_msg_;
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
