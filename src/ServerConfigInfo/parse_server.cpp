#include "../../include/WebServ.hpp"

/* ======================= ServerConfigInfo Init ======================= */
void Config::InitServerConfigInfo(ServerConfigInfo &info) {
  info.host_ = "127.0.0.1";
  info.port_ = -1;
  info.body_size_ = 0;
  info.root_path_ = "";

  info.server_name_ = "";
  info.directory_list_ = false;
  info.timeout_ = 0;

  info.methods_.clear();
  info.error_pages_.clear();
  info.locations_.clear();
}

/* =========================== Parsing Server =========================== */
void Config::ParseListen(const std::vector<std::string> &vec) {
  if (vec.size() != 1 || !IsNumber(vec[0])) ExitConfigParseError();

  int port = atoi(vec[0].c_str());
  if (port < 0 || port > 65535) ExitConfigParseError();
  server_config_info_.port_ = port;
}

void Config::ParseBodySize(const std::vector<std::string> &vec) {
  if (vec.size() != 1 || !IsNumber(vec[0])) ExitConfigParseError();

  int body_size = atoi(vec[0].c_str());
  if (body_size < 1) ExitConfigParseError();
  server_config_info_.body_size_ = body_size;
}

void Config::ParseRoot(const std::vector<std::string> &vec) {
  if (vec.size() != 1) ExitConfigParseError();

  std::string root_path = vec[0];
  struct stat sb;
  if (stat(root_path.c_str(), &sb) == 0)
    server_config_info_.root_path_ = root_path;
  else
    ExitConfigParseError();
}

void Config::ParseFilePath(const std::vector<std::string> &vec) {
  if (vec.size() != 1) ExitConfigParseError();

  std::string file_path = vec[0];
  struct stat sb;
  if (stat(file_path.c_str(), &sb) == 0)
    server_config_info_.file_path_ = file_path;
  else
    ExitConfigParseError();
}

void Config::ParseUploadPath(const std::vector<std::string> &vec) {
  if (vec.size() != 1) ExitConfigParseError();

  std::string upload_path = vec[0];
  struct stat sb;
  if (stat(upload_path.c_str(), &sb) == 0)
    server_config_info_.upload_path_ = upload_path;
  else
    ExitConfigParseError();
}

void Config::ParseServerName(const std::vector<std::string> &vec) {
  if (vec.size() != 1) ExitConfigParseError();
  server_config_info_.server_name_ = vec[0];
}

void Config::ParseDirectoryList(const std::vector<std::string> &vec) {
  if (vec.size() != 1 || (vec[0] != "on" && vec[0] != "off"))
    ExitConfigParseError();
  server_config_info_.directory_list_ = (vec[0] == "on") ? true : false;
}

void Config::ParseTimeout(const std::vector<std::string> &vec) {
  if (!IsNumber(vec[0])) ExitConfigParseError();

  int time_out = atoi(vec[0].c_str());
  if (time_out <= 0) ExitConfigParseError();
  server_config_info_.timeout_ = time_out;
}

void Config::ParseMethods(const std::vector<std::string> &vec) {
  if (!vec.size()) ExitConfigParseError();

  std::string method;
  for (size_t i = 0; i < vec.size(); ++i) {
    method = vec[i];
    if (!(method == "GET" || method == "HEAD" || method == "POST" ||
          method == "PUT" || method == "DELETE"))
      ExitConfigParseError();
    server_config_info_.methods_.push_back(method);
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

  server_config_info_.error_pages_[error_status_code] = error_page_path;
}
