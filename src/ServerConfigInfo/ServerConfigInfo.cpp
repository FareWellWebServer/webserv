#include "../../include/WebServ.hpp"

ServerConfigInfo::ServerConfigInfo(void) {}

ServerConfigInfo::~ServerConfigInfo(void) {}

void ConfigParser::ValidationCheck(void) const {
  std::cout << BOLDCYAN << "===== Validation Check Start =====" << std::endl;
  for (size_t i = 0; i < serverConfigInfos_.size(); ++i) {
    if (serverConfigInfos_[i].ValidationCheck())
      throw ConfigValidationException();
  }
  std::cout << "===== Validation Check Finish =====" << RESET << std::endl;
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

int ServerConfigInfo::LocationCheck(const location& l) const {
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
