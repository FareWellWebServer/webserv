#ifndef SERVERCONFIGINFO_HPP
#define SERVERCONFIGINFO_HPP

#include <iostream>
#include <map>
#include <vector>

struct location {
  std::string uri;        // 없을 수가 없는 변수
  int status_code;        // 필수 입력 O
  std::string file_path;  // 필수 입력 O

  std::vector<std::string> methods;  // 필수 입력 X

  // redir_path -> 필수 입력 X
  int redir_status;
  std::string redir_path;

  bool is_cgi;
  std::string cgi_pass;
};

class ServerConfigInfo {
 public:
  ServerConfigInfo(void);
  ~ServerConfigInfo(void);

  /* ======================== Variable ======================== */
  std::string host;         // default -> 127.0.0.1
  int port;                 // 필수 입력 O
  int body_size;            // 필수 입력 O
  std::string root_path;    // 필수 입력 O
  std::string file_path;    // 필수 입력 O
  std::string upload_path;  // 필수 입력 O

  std::string server_name;  // 필수 입력 X
  bool directory_list;      // 필수 입력 X
  int timeout;              // 필수 입력 X

  std::vector<std::string> methods;        // 필수 입력 O
  std::map<int, std::string> error_pages;  // 필수 입력 O

  std::vector<location> locations;  // 필수 입력 X
};

#endif
