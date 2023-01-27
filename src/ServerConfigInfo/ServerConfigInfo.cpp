#include "../../include/WebServ.hpp"

ServerConfigInfo::ServerConfigInfo(void) {}

ServerConfigInfo::~ServerConfigInfo(void) {}

t_location* ServerConfigInfo::get_cur_location(
    const std::string& req_uri) const {
  if (locations_.empty()) return NULL;

  std::vector<t_location>::const_iterator res;

  for (std::vector<t_location>::const_iterator it = locations_.begin();
       it != locations_.end(); ++it) {
    std::string location_path = it->uri;
    if (is_in_location(location_path, req_uri)) {
      return const_cast<t_location*>(&*it);
    }
  }
  return NULL;
}

bool ServerConfigInfo::is_in_location(const std::string& location_path,
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