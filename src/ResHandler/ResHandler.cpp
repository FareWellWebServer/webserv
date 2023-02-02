#include "../../include/ResHandler.hpp"

ResHandler::ResHandler(const char* response, std::size_t response_length)
    : response_(response), response_length_(response_length) {}

ResHandler::~ResHandler(void) {}

void ResHandler::SendToClient(int client_fd) {
  #if RES_HANDLER
    std::cout << "=== send response to client ===" << std::endl;
  #endif
  int r = write(client_fd, response_, response_length_);
  if (r < 0)
    std::cout << "Error Code: 500" << std::endl;
  else if (r == 0)
    std::cout << "Error Code: 400" << std::endl;
  delete response_;
}
