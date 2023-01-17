#include "../../include/ErrorHandler.hpp"

ErrorHandler::ErrorHandler(void) {}

ErrorHandler::ErrorHandler(const WebServException& error) {
  // error 상태에 따라 적절하게 처리
  std::cout << error.what() << std::endl;
  // logger 호출
  // logger.error("~~~") 같은 형태가 될 것
}

ErrorHandler::~ErrorHandler() {}
