#ifndef ERROR_HANDLER_HPP
#define ERROR_HANDLER_HPP

#include "WebServException.hpp"

class ErrorHandler {
 public:
  ErrorHandler(void);
  ErrorHandler(const WebServException& error);
  virtual ~ErrorHandler();
};

#endif
