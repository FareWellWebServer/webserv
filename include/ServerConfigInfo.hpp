#ifndef SERVERCONFIGINFO_HPP
#define SERVERCONFIGINFO_HPP

#include <iostream>
#include <map>
#include <vector>

typedef struct s_location {
  std::string file_path_;             // 필수 입력 O
  std::vector<std::string> methods_;  // 필수 입력 X
  std::string redir_path_;            // 필수 입력 X

  bool is_cgi_;
  std::string cgi_pass_;
} t_location;

class ServerConfigInfo {
 public:
  ServerConfigInfo(void);
  ~ServerConfigInfo(void);

  // host, port가 같은지 확인해주는 함수
  bool CheckHostPortMatching(const std::string& host, const int& port) const;

  // request url에 매칭되는 location 반환해주는 함수
  t_location* GetCurrentLocation(const std::string& req_uri) const;

  // bool CheckAvailableMethod(const std::string& req_uri,
  //                           const std::string method) const;

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

  std::map<std::string, t_location> locations_;  // 필수 입력 X

 private:
  bool IsInLocation(const std::string& location_path,
                    const std::string& req_uri) const;
};

#endif
