#include "../../include/Config.hpp"

Config::Config(const char* file_path) {
  std::ifstream fs(file_path);
  std::string content;
  std::string line;

  if (fs.is_open()) {
    while (!fs.eof()) {
      getline(fs, line);
      content.append(line + "\n");
    }
    fs.close();
  } else
    ExitConfigParseError("Config File Open Failed");
  if (content.size() == 0) ExitConfigParseError("Empty Config File");
  config_stream_.str(content);
}

Config::~Config(void) {}

/* ======================== Parsing Server ======================== */
void Config::Parse(int print_mode) {
  print_mode_ = print_mode;
  line_num_ = 0;

  while (true) {
    ++line_num_;
    std::getline(config_stream_, line_, '\n');
    if (IsWhiteLine()) continue;
    if (!IsLogPath()) {
      ExitConfigParseError("Log path not exist");
    } else {
      break;
    }
  }

  while (true) {
    ++line_num_;
    std::getline(config_stream_, line_, '\n');

    if (config_stream_.eof()) break;
    if (IsWhiteLine()) continue;

#if CONFIG
    Print("--------------- server parse start ---------------", BOLDBLUE);
#endif
    if (!IsOpenServerBracket()) ExitConfigParseError();
    InitServerConfigInfo(server_config_info_);
    ParseServer();
#if CONFIG
    Print("--------------- server parse finish --------------", BOLDBLUE, 1);
#endif
    if (!CheckDuplicatePort(server_config_info_.port_))
      server_config_infos_.push_back(server_config_info_);
  }
#if CONFIG
  Print("Config parsing finish", BOLDMAGENTA, 1);
#endif
}

void Config::ParseServer(void) {
  while (true) {
    ++line_num_;
    std::getline(config_stream_, line_, '\n');
    if (IsWhiteLine()) continue;

    std::vector<std::string> vec = Split(line_, " \t", 1);
    if (IsCloseBracket(vec)) break;
    if (vec.size() != 2) ExitConfigParseError();

    SetServerConfigInfo(vec[0], vec[1]);
  }
}

void Config::SetServerConfigInfo(const std::string& key,
                                 const std::string& val) {
  std::vector<std::string> vec = Split(val, " ");
  PrintKeyVal(key, val);

  if (key[0] == '#') {
    return;
  } else if (key == "listen") {
    vec = Split(val, ":");
    ParseListen(vec);
  } else if (key == "body_size") {
    ParseBodySize(vec);
  } else if (key == "root") {
    ParseRoot(vec);
  } else if (key == "upload_path") {
    ParseUploadPath(vec);
  } else if (key == "timeout") {
    ParseTimeout(vec);
  } else if (key == "method") {
    ParseMethods(vec);
  } else if (key == "error_page") {
    ParseErrorPage(vec);
  } else if (key == "server_name") {
    ParseServerName(vec);
  } else if (key == "location") {
    ParseLocation(key, val);
  } else
    ExitConfigParseError();
}

/* ======================== Validate Server ======================== */
bool Config::CheckDuplicatePort(int port) const {
  for (size_t i = 0; i < server_config_infos_.size(); ++i) {
    if (port == server_config_infos_[i].port_) return true;
  }
  return false;
}

bool Config::CheckValidPath(const std::string& file_path) const {
  struct stat sb;
  if (stat(file_path.c_str(), &sb) == 0) {
    return true;
  } else {
    return false;
  }
}

void Config::CheckValidation(void) {
  for (size_t i = 0; i < server_config_infos_.size(); ++i) {
    ServerConfigInfo& info = server_config_infos_[i];
    if (info.port_ == -1 || info.body_size_ == 0 || info.root_path_.empty() ||
        info.upload_path_.empty() || info.timeout_ == 0 ||
        info.methods_.empty() || info.error_pages_.empty()) {
      ExitConfigValidateError("Missing Server Elements");
    }

    std::map<int, std::string>::iterator error_it = info.error_pages_.begin();
    for (; error_it != info.error_pages_.end(); ++error_it) {
      int error_page_status_code = error_it->first;
      std::string error_page_path = info.root_path_ + error_it->second;

      if (CheckValidPath(error_page_path)) {
        info.error_pages_[error_page_status_code] = error_page_path;
      } else {
        ExitConfigValidateError("Wrong error_page path");
      }
    }

    std::map<std::string, t_location>::iterator loc_it =
        info.locations_.begin();
    for (; loc_it != info.locations_.end(); ++loc_it) {
      CheckLocation(loc_it->second);
    }
  }
}

void Config::CheckLocation(t_location& loc) {
  // cgi가 아닌 경우
  if (!loc.is_cgi_) {
    // file path가 없는 경우
    if (loc.file_path_.empty()) {
      ExitConfigValidateError("Location must has file_path");
    }

    for (size_t i = 0; i < loc.file_path_.size(); ++i) {
      std::string file_path = loc.root_path_ + loc.file_path_[i];
      if (CheckValidPath(file_path)) {
        loc.file_path_[i] = file_path;
      } else {
        ExitConfigValidateError("Wrong file path\n-> " + file_path);
      }
    }
  } else {  // cgi인 경우
    // cgi path가 없는 경우
    if (loc.cgi_path_.empty()) {
      ExitConfigValidateError("Cgi location must has file_path");
    }

    // cgi인데 리다이랙션이 있는 경우
    if (!loc.redir_path_.empty()) {
      ExitConfigValidateError("Cgi location must not has redirection");
    }

    // cgi인데 디렉토리 리스트가 있는 경우
    if (loc.directory_list_) {
      ExitConfigValidateError("Cgi location must not has directory_list");
    }

    for (size_t i = 0; i < loc.cgi_path_.size(); ++i) {
      std::string cgi_path = loc.root_path_ + loc.cgi_path_[i];
      if (CheckValidPath(cgi_path)) {
        loc.cgi_path_[i] = cgi_path;
      }
      // cgi path가 잘못된 경우
      else {
        ExitConfigValidateError("Wrong cgi file path\n-> " + cgi_path);
      }
    }
  }
}

/* ======================== Getter ======================== */
std::vector<ServerConfigInfo> Config::GetServerConfigInfos(void) const {
  return server_config_infos_;
}

std::string Config::GetLogPath(void) const { return log_path_; }
