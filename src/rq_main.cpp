#include <errno.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <stdio.h>
#include "../include/WebServ.hpp"

int main() {
  // int fd = open("/Users/sihunlee/42Seoul/webserv/src/test2", O_RDONLY);
  int fd = open("/Users/silee/42seoul/webserv/src/test2", O_RDONLY);
  if (fd < 0) {
    std::cout << "ERROR" << std::endl;
    std::cout << strerror(errno) << std::endl;
    return 0;
  }
  ReqHandler reqhandle;

  Parse_Req_Msg(fd, reqhandle);
  // ParseReqMsg(fd, reqhandle);
  // system("leaks Req_handle");
}
