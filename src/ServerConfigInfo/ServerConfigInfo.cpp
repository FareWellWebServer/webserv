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
  if (l.is_cgi) {
    std::cout << "uri: " << l.uri << std::endl;
    std::cout << "cgi_pass: " << l.cgi_pass << std::endl;
  } else {
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
  std::cout << "methods:";
  PrintVector(methods);
  std::cout << "error_pages:";
  std::map<int, std::string>::const_iterator it;
  for (it = error_pages.begin(); it != error_pages.end(); ++it)
    std::cout << " [" << it->first << " -> " << it->second << "]";
  std::cout << std::endl;
  PrintLocations();
}

int ServerConfigInfo::ValidationCheck(void) const {
  // 필수 요소 확인
  std::cout << "Server Essential Check" << std::endl;
  if (port < 0 || port > 65535 || host == "" || body_size < 1 ||
      !methods.size() || !error_pages.size())
    return 1;

  // method 체크
  std::cout << "Server Method Check" << std::endl;
  for (size_t i = 0; i < methods.size(); ++i) {
    std::string m = methods[i];
    if (!(m == "GET" || m == "HEAD" || m == "POST" || m == "PUT" ||
          m == "DELETE"))
      return 1;
  }

  // 에러 페이지 상태 코드와 열 수 있는 페이지인지 확인
  std::cout << "Server Error_page Check" << std::endl;
  std::map<int, std::string>::const_iterator it;
  for (it = error_pages.cbegin(); it != error_pages.cend(); ++it) {
    if (it->first < 100 || it->first > 511) return 1;
    std::ifstream fs(it->second);
    if (fs.fail()) return 1;
  }
  for (size_t i = 0; i < locations.size(); ++i)
    if (LocationCheck(locations[i])) return 1;

  return 0;
}

int ServerConfigInfo::LocationCheck(const location &l) const {
  std::cout << "====== Location Check Start ======" << std::endl;
  // 필수 요소 확인
  std::cout << "Location Essential Check" << std::endl;
  if (l.status_code == -1) return 1;

  // cgi 인 경우
  if (l.is_cgi) {
    std::cout << "Location Cgi Check" << std::endl;
    std::ifstream fs(l.cgi_pass);
    if (fs.fail()) return 1;
  } else {  // cgi 아닌 경우
    std::cout << "Location Not Cgi Check" << std::endl;
    // file path 체크
    if (!l.file_path.size()) return 1;
    for (size_t i = 0; i < l.file_path.size(); ++i) {
      std::ifstream fs(l.file_path[i]);
      if (fs.fail()) return 1;
    }

    // method 체크
    std::cout << "Location Method Check" << std::endl;
    for (size_t i = 0; i < methods.size(); ++i) {
      std::string m = methods[i];
      if (!(m == "GET" || m == "HEAD" || m == "POST" || m == "PUT" ||
            m == "DELETE"))
        return 1;
    }
  }
  std::cout << "====== Location Check Finish =====" << std::endl;

  return 0;
}
