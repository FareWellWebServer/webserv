#ifndef TYPES_HPP
#define TYPES_HPP

#include <iostream>

enum stage {
  READY,
  REQ_READY,
  REQ_FINISHED,
  GET_READY,
  GET_CGI,
  GET_FINISHED,
  POST_READY,
  POST_PROCESSING,
  POST_
};

#endif
