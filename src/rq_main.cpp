#include <errno.h>
#include <fcntl.h>
#include <sys/wait.h>

#include "../include/WebServ.hpp"

int main() {
  int fd = open("/Users/sihunlee/42Seoul/webserv/src/test2", O_RDONLY);
  // int fd = open("/Users/silee/42seoul/webserv/src/test2", O_RDONLY);
  if (fd < 0) {
    std::cout << "ERROR" << std::endl;
    std::cout << strerror(errno) << std::endl;
    return 0;
  }
  ReqHandler req;

  req.SetReadLen(405);
  req.SetBuf(fd);

  req.ParseRecv();
  // std::cout << "Entity :"<<req.req_msg_->body_data_.entity_ << std::endl;
  for (size_t i = 0; i < req.req_msg_->body_data_.length_; ++i) {
    printf("%c", req.req_msg_->body_data_.data_[i]);
  }
  // Parse_Req_Msg(fd, reqhandle);
  // system("leaks Req_handle");
}
