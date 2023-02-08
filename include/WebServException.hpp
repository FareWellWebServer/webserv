#ifndef WEB_SERV_EXCEPTION_HPP
#define WEB_SERV_EXCEPTION_HPP

#include <iostream>

class WebServException : public std::exception {
 public:
  // WebServException(const char* file, const char* fun, const int line,
  //                  std::string message);
  WebServException(std::string message);
  virtual const char* what() const throw() { return message_.c_str(); };
  ~WebServException() throw() {}

 private:
  // 안써서 주석 처리
  // const char* file_;
  // const char* fun_;
  // int line_;
  std::string message_;
};

#endif
