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
  std::string uri;                     // 없을 수가 없는 변수
  int status_code;                     // 필수 입력 O
  bool directory_list;                 // 필수 입력 X
  std::string redirection_path;        // 필수 입력 X
  std::vector<std::string> methods;    // 필수 입력 X
  std::vector<std::string> file_path;  // 필수 입력 O

  bool is_cgi;
  std::string cgi_pass;
};

class ServerConfigInfo {
  // private 가 없고 public인 이유)
  // 값 저장하려면 변수 별로 setter 함수를 만들어야 되서 일단 public으로
  // 진행했습니다
 public:
  ServerConfigInfo(void);
  ~ServerConfigInfo(void);

  /* ======================== Utils ======================== */
  void ClearInfo(void);
  void PrintLocation(const location &l) const;
  void PrintLocations(void) const;
  void PrintInfo(void) const;

  /* ======================== Validation ======================== */
  int ValidationCheck(void) const;
  int LocationCheck(const location &l) const;

  /* ======================== Variable ======================== */
  std::string name;  // 필수 입력 X
  std::string host;  // 필수 입력 O
  int port;          // 필수 입력 O
  int body_size;     // 필수 입력 O

  // 추가한 변수들 설명 -> 없어도 상관은 없을 거 같은데 일단 받게 해놓음
  // 추가한 변수 -> server에도 directory_list이 들어갈 수 있어야 될 것 같음
  bool directory_list;  // 필수 입력 X
  // 추가한 변수 -> server에도 리다이랙션이 들어갈 수 있어야 될 것 같음
  std::string redirection_path;  // 필수 입력 X

  std::vector<std::string> methods;  // 필수 입력 O

  /** error_page 변수 수정
   * std::string error_page
   * -> std::map<int, std::string>  error_pages
   *
   * error_page 404 www/404.html
   * error_page 500 www/500.html
   * 같은 식으로 들어오니 그냥 map으로 변경
   */
  std::map<int, std::string> error_pages;  // 필수 입력 O

  std::vector<location> locations;  // 필수 입력 X
};

template <typename T>
void PrintVector(const std::vector<T> &vec) {
  for (size_t i = 0; i < vec.size(); ++i) std::cout << " " << vec[i];
  std::cout << std::endl;
}

#endif