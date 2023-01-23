#include "../../include/WebServ.hpp"

ConfigParser::ConfigParser(const char* file_path) {
  std::cout << "config file path: " << file_path << std::endl;

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

ConfigParser::~ConfigParser(void) {}

const char* ConfigParser::ConfigValidationException::what(void) const throw() {
  return "[Config Error] Validation Check Failed";
}

/* ======================== Parsing Server ======================== */
void ConfigParser::Parse(void) {
  line_num_ = 0;
  while (42) {
    ++line_num_;
    std::getline(config_stream_, line_, '\n');

    if (config_stream_.eof()) break;
    if (IsWhiteLine()) continue;

    Print("------------- server parse start -------------", BOLDBLUE);
    if (!IsOpenServerBracket()) ExitConfigParseError();
    InitServerConfigInfo(serverConfigInfo_);
    ParseServer();
    Print("------------- server parse finish ------------", BOLDBLUE, 1);

    serverConfigInfos_.push_back(serverConfigInfo_);
  }
  Print("config parsing finish", BOLDMAGENTA, 1);
}

void ConfigParser::ParseServer(void) {
  while (42) {
    ++line_num_;
    std::getline(config_stream_, line_, '\n');
    if (IsWhiteLine()) continue;

    std::vector<std::string> vec = Split(line_, " \t", 1);
    if (IsCloseBracket(vec)) break;
    if (vec.size() != 2) ExitConfigParseError();

    SetServerConfigInfo(vec[0], vec[1]);
  }
}

void ConfigParser::SetServerConfigInfo(const std::string& key,
                                       const std::string& val) {
  std::vector<std::string> vec = Split(val, " ");
  printf("key: %-18s| val: %s\n", key.c_str(), val.c_str());

  if (key == "listen") {
    ParseListen(vec);
  } else if (key == "root") {
    ParseRoot(vec);
  } else if (key == "body_size") {
    ParseBodySize(vec);
  } else if (key == "server_name") {
    ParseServerName(vec);
  } else if (key == "autoindex") {
    ParseAutoindex(vec);
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
void ConfigParser::CheckLocation(const location& l) const {
  if (!l.is_cgi) {  // cgi가 아닌 경우
    std::cout << "Not Cgi Location Check" << std::endl;
    if (l.status_code == -1 || !l.file_path.size())
      ExitConfigValidateError(
          "Missing Location Elements(status_code or file_path)");
  } else {  // cgi인 경우
    std::cout << "Cgi Location Check" << std::endl;
    if (l.status_code == -1 || !l.cgi_pass.size())
      ExitConfigValidateError(
          "Missing Location Elements(status_code or cgi_pass)");
  }
}

void ConfigParser::CheckServerConfigInfo(const ServerConfigInfo& info) const {
  std::cout << "Server Essential Check" << std::endl;
  if (info.port == -1 || info.body_size == 0 || info.root_path.size() == 0 ||
      !info.methods.size() || !info.error_pages.size())
    ExitConfigValidateError("Missing Server Elements");

  std::cout << "====== Location Check Start ======" << std::endl;
  for (size_t i = 0; i < info.locations.size(); ++i)
    CheckLocation(info.locations[i]);
  std::cout << "====== Location Check Finish =====" << std::endl;
}

void ConfigParser::CheckValidation(void) const {
  Print("===== Validation Check Start =====", BOLDCYAN);
  for (size_t i = 0; i < serverConfigInfos_.size(); ++i)
    CheckServerConfigInfo(serverConfigInfos_[i]);
  Print("===== Validation Check Finish =====", BOLDCYAN, 1);
}
