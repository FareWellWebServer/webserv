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
  std::vector<std::string> file_path;  // 필수 입력 O

  std::vector<std::string> methods;  // 필수 입력 X

  // redir_path -> 필수 입력 X
  int redir_status;
  std::string redir_path;

  bool is_cgi;
  std::string cgi_pass;
};

class ServerConfigInfo {
  // private가 없는 이유
  // 값 저장하려면 변수 별로 setter 함수를 만들어야 되서 일단 public으로
  // 진행했습니다
 public:
  ServerConfigInfo(void);
  ~ServerConfigInfo(void);

  /* ======================== Variable ======================== */
  std::string host;       // default -> 127.0.0.1
  int port;               // 필수 입력 O
  int body_size;          // 필수 입력 O
  std::string root_path;  // 필수 입력 O

  std::string server_name;  // 필수 입력 X

  // 이름 변경된 변수 -> directory_list -> autoindex
  bool autoindex;  // 필수 입력 X

  // 논의 필요한 변수 keep_alive_time
  // default를 정해준다고 치면 필수는 아닌데 정의는 되어야함
  int timeout;  // 필수 입력 X

  std::vector<std::string> methods;        // 필수 입력 O
  std::map<int, std::string> error_pages;  // 필수 입력 O
  std::vector<location> locations;         // 필수 입력 X
};

#endif