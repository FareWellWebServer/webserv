#include "../../include/Config.hpp"

Config::Config(const char* file_path) {
  std::cout << "Config file path: " << file_path << std::endl;

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

  if (key == "listen") {
    vec = Split(val, ":");
    ParseListen(vec);
  } else if (key == "body_size") {
    ParseBodySize(vec);
  } else if (key == "root") {
    ParseRoot(vec);
  } else if (key == "file_path") {
    ParseFilePath(vec);
  } else if (key == "upload_path") {
    ParseUploadPath(vec);
  } else if (key == "server_name") {
    ParseServerName(vec);
  } else if (key == "directory_list") {
    ParseDirectoryList(vec);
  } else if (key == "timeout") {
    ParseTimeout(vec);
  } else if (key == "method") {
    ParseMethods(vec);
  } else if (key == "error_page") {
    ParseErrorPage(vec);
  } else if (key == "location") {
    ParseLocation(key, val);
  } else
    ExitConfigParseError();
}

/* ======================== Validate Server ======================== */
void Config::CheckValidation(void) const {
#if CONFIG
  std::cout << BOLDCYAN
            << "======== Validation Check Start ========" << std::endl;
#endif
  for (size_t i = 0; i < server_config_infos_.size(); ++i) {
    ServerConfigInfo info = server_config_infos_[i];
    if (info.port_ == -1 || info.body_size_ == 0 || info.root_path_.empty() ||
        info.upload_path_.empty() || info.methods_.empty() ||
        info.error_pages_.empty())
      ExitConfigValidateError("Missing Server Elements");

    for (size_t i = 0; i < info.locations_.size(); ++i)
      CheckLocation(info.locations_[i]);
  }
#if CONFIG
  std::cout << "======= Validation Check Finish ========" << RESET << std::endl;
#endif
}

bool Config::CheckDuplicatePort(int port) const {
  for (size_t i = 0; i < server_config_infos_.size(); ++i) {
    if (port == server_config_infos_[i].port_) return true;
  }
  return false;
}

void Config::CheckLocation(const t_location& l) const {
  if (!l.is_cgi_) {  // cgi가 아닌 경우
    if (l.file_path_.empty())
      ExitConfigValidateError(
          "Missing Location Elements(status_code or file_path)");
  } else {  // cgi인 경우
    if (l.cgi_pass_.empty())
      ExitConfigValidateError(
          "Missing Location Elements(status_code or cgi_pass)");
  }
}

/* ======================== Getter ======================== */
std::vector<ServerConfigInfo> Config::GetServerConfigInfos(void) {
  return server_config_infos_;
}
