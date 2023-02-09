#include "../../include/Config.hpp"

ServerConfigInfo::ServerConfigInfo(void) {}

ServerConfigInfo::~ServerConfigInfo(void) {}

t_location* ServerConfigInfo::GetCurrentLocation(
    const std::string& req_uri) const {
  if (locations_.empty()) return NULL;

  std::map<std::string, t_location>::const_iterator it = locations_.begin();
  for (; it != locations_.end(); ++it) {
    std::string location_path = it->first;
    if (req_uri == "/" && req_uri == location_path) {
      return const_cast<t_location*>(&it->second);
    } else if (req_uri != "/" &&
               req_uri.substr(0, req_uri.size() - 1) == location_path) {
      return const_cast<t_location*>(&it->second);
    }
  }
  return NULL;
}

bool ServerConfigInfo::CheckAvailableMethod(const std::string& req_uri,
                                            const std::string& method) const {
  t_location* loc = GetCurrentLocation(req_uri);
  std::vector<std::string>::const_iterator begin =
      (loc) ? loc->methods_.begin() : methods_.begin();
  std::vector<std::string>::const_iterator end =
      (loc) ? loc->methods_.end() : methods_.end();
  if (std::find(begin, end, method) != end) {
    return true;
  }
  return false;
}
