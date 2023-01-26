#include "../../include/WebServ.hpp"

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

    Print("--------------- server parse start ---------------", BOLDBLUE);
    if (!IsOpenServerBracket()) ExitConfigParseError();
    InitServerConfigInfo(server_config_info_);
    ParseServer();
    Print("--------------- server parse finish --------------", BOLDBLUE, 1);

    if (!CheckDuplicatePort(server_config_info_.port))
      server_config_infos_.push_back(server_config_info_);
  }
  Print("Config parsing finish", BOLDMAGENTA, 1);
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
  std::cout << BOLDCYAN
            << "======== Validation Check Start ========" << std::endl;

  for (size_t i = 0; i < server_config_infos_.size(); ++i) {
    ServerConfigInfo info = server_config_infos_[i];
    if (info.port == -1 || info.body_size == 0 || info.root_path.empty() ||
        info.upload_path.empty() || info.methods.empty() ||
        info.error_pages.empty())
      ExitConfigValidateError("Missing Server Elements");

    for (size_t i = 0; i < info.locations.size(); ++i)
      CheckLocation(info.locations[i]);
  }
  std::cout << "======= Validation Check Finish ========" << RESET << std::endl;
}

bool Config::CheckDuplicatePort(int port) const {
  for (size_t i = 0; i < server_config_infos_.size(); ++i) {
    if (port == server_config_infos_[i].port) return true;
  }
  return false;
}

void Config::CheckLocation(const location& l) const {
  if (!l.is_cgi) {  // cgi가 아닌 경우
    if (l.status_code == -1 || l.file_path.empty())
      ExitConfigValidateError(
          "Missing Location Elements(status_code or file_path)");
  } else {  // cgi인 경우
    if (l.status_code == -1 || l.cgi_pass.empty())
      ExitConfigValidateError(
          "Missing Location Elements(status_code or cgi_pass)");
  }
}

/* ======================== Getter ======================== */
std::vector<ServerConfigInfo> Config::GetServerConfigInfos(void) {
  return server_config_infos_;
}