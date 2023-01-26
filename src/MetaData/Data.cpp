#include "../../include/Data.hpp"
#define NULL 0

Data::Data()
{
  litsen_fd_ = -1;
  port_ = -1;
  // client_fd_ = -1;
  event_ = NULL;
  // config_ = NULL;
  req_message_ = NULL;
  res_message_ = NULL;
  status_code_ = 200;
  entity_ = NULL;
}

Data::~Data()
{}