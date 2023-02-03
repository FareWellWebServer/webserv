#ifndef SERVERCONFIGINFO_HPP
#define SERVERCONFIGINFO_HPP

#include <iostream>
#include <map>
#include <vector>

typedef struct s_location {
  std::string uri_;        // 없을 수가 없는 변수
  std::string file_path_;  // 필수 입력 O

  std::vector<std::string> methods_;  // 필수 입력 X

  std::string redir_path_;

  bool is_cgi_;
  std::string cgi_pass_;
} t_location;

class ServerConfigInfo {
 public:
  ServerConfigInfo(void);
  ~ServerConfigInfo(void);

  // host, port가 같은지 확인 -> 같은 경우 매칭되는 서버임
  bool CheckHostPortMatching(const std::string& host, const int& port) const;
  t_location* GetCurrentLocation(const std::string& req_uri) const;
  bool IsInLocation(const std::string& location_path,
                    const std::string& req_uri) const;

  std::string GetCurrentDate(void) const;

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
};

#endif
