#ifndef SERVERCONFIGINFO_HPP
#define SERVERCONFIGINFO_HPP

#include <iostream>
#include <map>
#include <vector>

typedef struct s_location {
  std::string uri;        // 없을 수가 없는 변수
  int status_code;        // 필수 입력 O
  std::string file_path;  // 필수 입력 O

  std::vector<std::string> methods;  // 필수 입력 X

  // redir_path -> 필수 입력 X
  int redir_status;
  std::string redir_path;

  bool is_cgi;
  std::string cgi_pass;
} t_location;

class ServerConfigInfo {
 public:
  /* ======================== Variable ======================== */
  std::string host_;         // default -> 127.0.0.1
  int port_;                 // 필수 입력 O
  int body_size_;            // 필수 입력 O
  std::string root_path_;    // 필수 입력 O
  std::string file_path_;    // 필수 입력 O
  std::string upload_path_;  // 필수 입력 O

  std::string server_name_;  // 필수 입력 X
  bool directory_list_;      // 필수 입력 X
  int timeout_;              // 필수 입력 X

  std::vector<std::string> methods_;        // 필수 입력 O
  std::map<int, std::string> error_pages_;  // 필수 입력 O

  std::vector<t_location> locations_;  // 필수 입력 X

 public:
  ServerConfigInfo(void);
  ~ServerConfigInfo(void);

  t_location* get_cur_location(const std::string& req_uri) const;

  bool is_in_location(const std::string& location_path,
                      const std::string& req_uri) const;
};

#endif