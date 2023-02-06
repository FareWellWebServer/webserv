#ifndef RES_HANDLER_HPP
#define RES_HANDLER_HPP

#include <unistd.h>

#include <iostream>
#include <sstream>
#include <vector>

class ResHandler {
 public:
  ResHandler(const char* response, std::size_t response_length);
  ~ResHandler(void);
  void SendToClient(int client_fd);

 private:
  const char* response_;
  std::size_t response_length_;
};

#endif