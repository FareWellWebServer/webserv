#include "../../include/WebServ.hpp"

ServerConfigInfo::ServerConfigInfo(void) {}

ServerConfigInfo::~ServerConfigInfo(void) {}

void ServerConfigInfo::ClearInfo(void) {
  name = "";
  host = "";
  port = -1;
  body_size = 0;
  directory_list = false;
  redirection_path = "";
  methods.clear();
  error_pages.clear();
  locations.clear();
}

void ServerConfigInfo::PrintLocation(const location &l) const {
  if (!l.cgi_pass.size()) {
    std::cout << "uri: " << l.uri << std::endl;
    std::cout << "status_code: " << l.status_code << std::endl;
    std::cout << "directory_list: " << l.directory_list << std::endl;
    std::cout << "redirection_path: " << l.redirection_path << std::endl;
    std::cout << "methods:";
    for (size_t i = 0; i < l.methods.size(); ++i)
      std::cout << " " << l.methods[i];
    std::cout << std::endl;
    std::cout << "file_path:";
    for (size_t i = 0; i < l.file_path.size(); ++i)
      std::cout << " " << l.file_path[i];
    std::cout << std::endl;
  } else {
    std::cout << "uri: " << l.uri << std::endl;
    std::cout << "cgi_pass: " << l.cgi_pass << std::endl;
  }
}

void ServerConfigInfo::PrintLocations(void) const {
  for (size_t i = 0; i < locations.size(); ++i) {
    std::cout << std::endl;
    std::cout << "--- [locations " << i << " -> cgi ";
    (locations[i].cgi_pass.size()) ? std::cout << "O" : std::cout << "X";
    std::cout << "] ---" << std::endl;
    PrintLocation(locations[i]);
  }
}

void ServerConfigInfo::PrintInfo(void) const {
  std::cout << "--- [server info] ---" << std::endl;
  std::cout << "name: " << name << std::endl;
  std::cout << "host: " << host << std::endl;
  std::cout << "port: " << port << std::endl;
  std::cout << "body_size: " << body_size << std::endl;
  std::cout << "directory_list: " << directory_list << std::endl;
  std::cout << "redirection_path: " << redirection_path << std::endl;
  std::cout << "methods:";
  PrintVector(methods);
  std::cout << "error_pages:";
  std::map<int, std::string>::const_iterator it;
  for (it = error_pages.begin(); it != error_pages.end(); ++it)
    std::cout << " [" << it->first << " -> " << it->second << "]";
  std::cout << std::endl;
  PrintLocations();
}
