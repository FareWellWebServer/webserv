#include "../../include/Config.hpp"

ServerConfigInfo::ServerConfigInfo(void) {}

ServerConfigInfo::~ServerConfigInfo(void) {}

bool ServerConfigInfo::CheckHostPortMatching(const std::string& host,
                                             const int& port) const {
  if (host_ == host && port_ == port) return true;
  return false;
}

t_location* ServerConfigInfo::GetCurrentLocation(
    const std::string& req_uri) const {
  if (locations_.empty()) return NULL;

  std::vector<t_location>::const_iterator res;

  for (std::vector<t_location>::const_iterator it = locations_.begin();
       it != locations_.end(); ++it) {
    std::string location_path = it->uri_;
    if (IsInLocation(location_path, req_uri)) {
      return const_cast<t_location*>(&*it);
    }
  }
  return NULL;
}

bool ServerConfigInfo::IsInLocation(const std::string& location_path,
                                    const std::string& req_uri) const {
  if (req_uri.compare(location_path)) return false;
  if (req_uri[location_path.length()]) {
    if (req_uri[location_path.length()] == '/' ||
        req_uri[location_path.length()] == '?')
      return true;
    else
      return false;
  } else
    return true;
}

std::string ServerConfigInfo::GetCurrentDate(void) const {
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
