#ifndef SERVERCONFIGINFO_HPP
#define SERVERCONFIGINFO_HPP

#include <iostream>
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
  int status_code;
  bool autoindex;  // 변수명 변경(directory_list -> autoindex)
  std::string uri;
  std::string redirection_path;
  std::vector<std::string> methods;
  std::vector<std::string> file_path;

  bool is_cgi;
  std::string cgi_pass;
};

class ServerConfigInfo {
 public:
  ServerConfigInfo(void);
  ~ServerConfigInfo(void);

 private:
  int port;                          // port number
  int body_size;                     // client body size limit
  std::string name;                  // server name
  std::string host;                  // host
  std::string error_page;            // default error page
  std::string redirection_path;      // redirection path
  std::vector<std::string> methods;  // limit method

  std::vector<location> locations_;
};

#endif