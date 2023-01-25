#include "../../include/WebServ.hpp"

/* =========================== Parsing Server =========================== */
void Config::ParseListen(const std::vector<std::string> &vec) {
  if (vec.size() != 1 || !IsNumber(vec[0])) ExitConfigParseError();

  int port = atoi(vec[0].c_str());
  if (port < 0 || port > 65535) ExitConfigParseError();
  server_config_info_.port = port;
}

void Config::ParseBodySize(const std::vector<std::string> &vec) {
  if (vec.size() != 1 || !IsNumber(vec[0])) ExitConfigParseError();

  int body_size = atoi(vec[0].c_str());
  if (body_size < 1) ExitConfigParseError();
  server_config_info_.body_size = body_size;
}

void Config::ParseRoot(const std::vector<std::string> &vec) {
  if (vec.size() != 1) ExitConfigParseError();

  std::string root_path = vec[0];
  struct stat sb;
  if (stat(root_path.c_str(), &sb) == 0)
    server_config_info_.root_path = root_path;
  else
    ExitConfigParseError();
}

void Config::ParseServerName(const std::vector<std::string> &vec) {
  if (vec.size() != 1) ExitConfigParseError();
  server_config_info_.server_name = vec[0];
}

void Config::ParseAutoindex(const std::vector<std::string> &vec) {
  if (vec.size() != 1 || (vec[0] != "on" && vec[0] != "off"))
    ExitConfigParseError();
  server_config_info_.autoindex = (vec[0] == "on") ? true : false;
}

void Config::ParseTimeout(const std::vector<std::string> &vec) {
  if (!IsNumber(vec[0])) ExitConfigParseError();

  int time_out = atoi(vec[0].c_str());
  if (time_out <= 0) ExitConfigParseError();
  server_config_info_.timeout = time_out;
}

void Config::ParseMethods(const std::vector<std::string> &vec) {
  if (!vec.size()) ExitConfigParseError();

  std::string method;
  for (size_t i = 0; i < vec.size(); ++i) {
    method = vec[i];
    if (!(method == "GET" || method == "HEAD" || method == "POST" ||
          method == "PUT" || method == "DELETE"))
      ExitConfigParseError();
    server_config_info_.methods.push_back(method);
  }
}

void Config::ParseErrorPage(const std::vector<std::string> &vec) {
  if (vec.size() != 2 || !IsNumber(vec[0])) ExitConfigParseError();

  int error_status_code = atoi(vec[0].c_str());
  std::string error_page_path = vec[1];

  if (error_status_code < 100 || error_status_code > 511)
    ExitConfigParseError();
  std::ifstream fs(error_page_path);
  if (fs.fail()) ExitConfigParseError();

  server_config_info_.error_pages[error_status_code] = error_page_path;
}
