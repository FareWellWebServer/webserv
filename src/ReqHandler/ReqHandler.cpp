#include "../../include/WebServ.hpp"

ReqHandler::ReqHandler(void) { std::memset(this, 0, sizeof(ReqHandler)); }

ReqHandler::~ReqHandler(void) {}

void Parse_Req_msg(int c_socket) {
  char* c_tmp;
  std::string s_tmp;
  std::string contents;

  while ((c_tmp = get_next_line(c_socket)) != 0) {
    s_tmp = c_tmp;
    contents.append(s_tmp);
    free(c_tmp);
    s_tmp.clear();
  }
}
