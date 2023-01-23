#include "../../include/WebServ.hpp"

int ConfigParser::IsNumber(const std::string& str) const {
  for (size_t i = 0; i < str.length() - 1; i++)
    if (!isdigit(str[i])) return 0;
  return 1;
}

void ConfigParser::PrintConfigInfo(void) const {
  for (size_t i = 0; i < serverConfigInfos_.size(); ++i) {
    std::cout << BOLDGREEN << "---------- [server config info " << i
              << "] ----------" << std::endl;
    serverConfigInfos_[i].PrintInfo();
    std::cout << "--------------------------------------------" << RESET
              << std::endl;
  }
}

std::vector<std::string> ConfigParser::Split(const std::string& str,
                                             const std::string& charset,
                                             int once) const {
  std::vector<std::string> res;
  size_t start = str.find_first_not_of(charset);
  size_t end = str.find_first_of(charset, start);

  if (end == std::string::npos) res.push_back(str.substr(start, str.size()));

  while (end != std::string::npos) {
    res.push_back(str.substr(start, end - start));
    start = str.find_first_not_of(charset, end);
    end = str.find(charset, start);
    if (once) {
      res.push_back(str.substr(start, end - start));
      break;
    }
    if (start == std::string::npos) break;
    res.push_back(str.substr(start, end - start));
  }
  return res;
}

void ServerConfigInfo::PrintLocation(const location &l) const {
  if (l.is_cgi) {
    std::cout << "uri: " << l.uri << std::endl;
    std::cout << "cgi_pass: " << l.cgi_pass << std::endl;
  } else {
    std::cout << "uri: " << l.uri << std::endl;
    std::cout << "status_code: " << l.status_code << std::endl;
    std::cout << "directory_list: " << l.directory_list << std::endl;
    std::cout << "redir status : " << l.redir_status << std::endl;
    std::cout << "redirection_path: " << l.redirection_path << std::endl;
    std::cout << "methods:";
    for (size_t i = 0; i < l.methods.size(); ++i)
      std::cout << " " << l.methods[i];
    std::cout << std::endl;
    std::cout << "file_path:";
    for (size_t i = 0; i < l.file_path.size(); ++i)
      std::cout << " " << l.file_path[i];
    std::cout << std::endl;
  }
}

void ServerConfigInfo::PrintLocations(void) const {
  for (size_t i = 0; i < locations.size(); ++i) {
    std::cout << std::endl;
    std::cout << "------ [locations " << i << " -> cgi ";
    (locations[i].is_cgi) ? std::cout << "O" : std::cout << "X";
    std::cout << "] ------" << std::endl;
    PrintLocation(locations[i]);
  }
}

void ServerConfigInfo::PrintInfo(void) const {
  std::cout << "------ [server info] ------" << std::endl;
  std::cout << "name: " << name << std::endl;
  std::cout << "host: " << host << std::endl;
  std::cout << "port: " << port << std::endl;
  std::cout << "body_size: " << body_size << std::endl;
  std::cout << "directory_list: " << directory_list << std::endl;
  std::cout << "redirection_path: " << redirection_path << std::endl;
  std::cout << "root_path: " << root_path << std::endl;
  std::cout << "methods:";
  PrintVector(methods);
  std::cout << "keep_alive_time: " << keep_alive_time << std::endl;
  std::cout << "error_pages:";
  std::map<int, std::string>::const_iterator it;
  for (it = error_pages.begin(); it != error_pages.end(); ++it)
    std::cout << " [" << it->first << " -> " << it->second << "]";
  std::cout << std::endl;
  PrintLocations();
}

