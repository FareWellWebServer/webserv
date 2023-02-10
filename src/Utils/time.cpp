#include "../../include/Utils.hpp"

std::string GetCurrentDate(void) {
  time_t raw_time;
  struct tm* pm;
  char format[40];

  time(&raw_time);
  pm = gmtime(&raw_time);
  pm->tm_hour += 9;
  pm->tm_mday += pm->tm_hour / 24;
  pm->tm_hour %= 24;
  strftime(format, 29, "%a, %d %b %G %X KST", pm);
  return std::string(format);
}
