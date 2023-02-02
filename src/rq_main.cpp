#include <errno.h>
#include <fcntl.h>
#include <sys/wait.h>

#include "../include/WebServ.hpp"

void Print_Map(std::map<std::string, std::string>& map) {
  std::cout << RED << "[[PRINT MAP]]" << RESET << std::endl;
  for (std::map<std::string, std::string>::iterator it = map.begin();
       it != map.end(); ++it) {
    std::cout << "key :" << it->first << " || value :" << it->second
              << std::endl;
  }
}
int main() {
  // int fd = open("/Users/sihunlee/42Seoul/webserv/src/test2", O_RDONLY);
  int fd = open("/Users/silee/42seoul/webserv/src/test2", O_RDONLY);
  if (fd < 0) {
    std::cout << "ERROR" << std::endl;
    std::cout << strerror(errno) << std::endl;
    return 0;
  }
  ReqHandlerd req;

  req.ParseRecv(fd);
  // std::cout << "Entity :"<<req.req_msg_->body_data_.entity_ << std::endl;
  for (int i = 0; i < req.req_msg_->body_data_.entity_length_; ++i)
  {

    printf("%c", req.req_msg_->body_data_.entity_[i]);
  }
  // Parse_Req_Msg(fd, reqhandle);
  // system("leaks Req_handle");
}