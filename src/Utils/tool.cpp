#include "../../include/WebServ.hpp"

std::string GetCurrentDate(void) {
  time_t raw_time;
  struct tm* pm;
  char format[29];

  time(&raw_time);
  pm = gmtime(&raw_time);
  strftime(format, 29, "%a, %d %b %G %X GMT", pm);
  return std::string(format);
}