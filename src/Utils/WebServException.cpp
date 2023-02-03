#include "../../include/WebServException.hpp"

// WebServException::WebServException(const char* file, const char* fun,
//                                    const int line, std::string message)
//     : file_(file), fun_(fun), line_(line), message_(message){}

// WebServException::WebServException(std::string message)
//     : file_(NULL), fun_(NULL), line_(0), message_(message) {}

WebServException::WebServException(std::string message) : message_(message) {}
