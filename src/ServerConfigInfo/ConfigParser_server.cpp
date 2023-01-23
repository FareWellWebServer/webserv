#include "../../include/WebServ.hpp"

void ServerConfigInfo::ClearInfo(void) {
  name = "";
  host = "127.0.0.1";
  port = -1;
  body_size = 0;

  autoindex = false;
  keep_alive_time = 0;
  root_path = "";

  methods.clear();
  error_pages.clear();
  locations.clear();
}

int ConfigParser::ParseServer(std::istringstream& iss) {
  std::string line, key, val;

  while (42) {
    std::getline(iss, line, '\n');
    if (line.find_first_not_of(" \t") == std::string::npos) continue;

    std::vector<std::string> vec = Split(line, " \t", 1);
    if (vec.size() == 1 && vec[0] == "}") break;
    if (vec.size() != 2) return 1;
    if (SetServerConfigInfo(iss, vec[0], vec[1])) return 1;
  }
  return 0;
}

int ConfigParser::SetServerConfigInfo(std::istringstream& iss,
                                      const std::string& key,
                                      const std::string& val) {
  std::string c = (key == "listen") ? ":" : " ";
  std::vector<std::string> vec = Split(val, c);

  PrintVector(vec);
  printf("key: %-15s| val: %s\n", key.c_str(), val.c_str());

  if (key == "server_name") {
    if (vec.size() != 1) return 1;
    serverConfigInfo_.name = val;
  } else if (key == "listen") {
    if (vec.size() != 1 || !IsNumber(vec[0])) return 1;
    serverConfigInfo_.port = atoi(vec[0].c_str());
  } else if (key == "body_size") {
    if (vec.size() != 1 || !IsNumber(vec[0])) return 1;
    serverConfigInfo_.body_size = atoi(vec[0].c_str());
  } else if (key == "root") {
    if (vec.size() != 1) return 1;
    serverConfigInfo_.root_path = vec[0];
  } else if (key == "autoindex") {
    if (vec.size() != 1 || (vec[0] != "on" && vec[0] != "off")) return 1;
    serverConfigInfo_.autoindex = (vec[0] == "on") ? true : false;
  } else if (key == "keepalive_timeout") {
    if (!IsNumber(vec[0])) return 1;
    serverConfigInfo_.keep_alive_time = atoi(vec[0].c_str());
  } else if (key == "method") {
    for (size_t i = 0; i < vec.size(); ++i)
      serverConfigInfo_.methods.push_back(vec[i]);
  } else if (key == "error_page") {
    if (vec.size() != 2 || !IsNumber(vec[0])) return 1;
    int status_code = atoi(vec[0].c_str());
    serverConfigInfo_.error_pages[status_code] = vec[1];
  } else if (key == "location") {
    if (ParseLocation(iss, key, val)) return 1;
  } else
    return 1;
  return 0;
}

int ConfigParser::SetServerLocation(location& l, const std::string& key,
                                    const std::string& val) {
  std::vector<std::string> vec = Split(val, " ");
  printf("key: %-15s| val: %s\n", key.c_str(), val.c_str());

  if (key == "status_code") {
    if (vec.size() != 1 || !IsNumber(vec[0])) return 1;
    l.status_code = atoi(vec[0].c_str());
  } else if (key == "redirection") {
    if (vec.size() != 2 || !IsNumber(vec[0])) return 1;
    l.redir_status = atoi(vec[0].c_str());
    l.redir_path = vec[1];
  } else if (key == "method") {
    for (size_t i = 0; i < vec.size(); ++i) l.methods.push_back(vec[i]);
  } else if (key == "file_path") {
    for (size_t i = 0; i < vec.size(); ++i) l.file_path.push_back(vec[i]);
  } else if (key == "cgi_pass") {
    if (l.is_cgi == false || vec.size() != 1) return 1;
    l.cgi_pass = vec[0];
  }
  return 0;
}
