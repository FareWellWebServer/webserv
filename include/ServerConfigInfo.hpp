#ifndef SERVERCONFIGINFO_HPP
#define SERVERCONFIGINFO_HPP

#include <iostream>
#include <map>
#include <vector>

// struct server_data {
//   int port;                          // port number
//   int body_size;                     // client body size limit
//   std::string name;                  // server name
//   std::string host;                  // host
//   std::string error_page;            // default error page
//   std::string redirection_path;      // redirection path
//   std::vector<std::string> methods;  // limit method
// };

struct location {
  std::string uri;
  int status_code;
  bool directory_list;
  std::string redirection_path;
  std::vector<std::string> methods;
  std::vector<std::string> file_path;

  std::string cgi_pass;
};

class ServerConfigInfo {
  // private 가 없고 public인 이유!
  // 값 저장하려면 변수 별로 setter 함수를 만들어야 되서 일단 public으로
  // 진행했습니다
 public:
  ServerConfigInfo(void);
  ~ServerConfigInfo(void);

  void ClearInfo(void);
  void PrintLocation(const location &l) const;
  void PrintLocations(void) const;
  void PrintInfo(void) const;

  std::string name;
  std::string host;
  int port;
  int body_size;
  std::vector<std::string> methods;

  /** error_page 변수 수정
   * std::string error_page
   * -> std::map<int, std::string>  error_pages
   *
   * error_page 404 www/404.html
   * error_page 500 www/500.html
   * 같은 식으로 들어오니 그냥 map으로 변경
   */
  std::map<int, std::string> error_pages;

  std::vector<location> locations;

 private:
  ;
};

template <typename T>
void PrintVector(const std::vector<T> &vec) {
  for (size_t i = 0; i < vec.size(); ++i) std::cout << " " << vec[i];
  std::cout << std::endl;
}

#endif