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

/* ======================== Parsing Server ======================== */
void ConfigParser::Parse(int print_mode) {
  print_mode_ = print_mode;
  line_num_ = 0;
  while (42) {
    ++line_num_;
    std::getline(config_stream_, line_, '\n');

    if (config_stream_.eof()) break;
    if (IsWhiteLine()) continue;

    Print("--------------- server parse start ---------------", BOLDBLUE);
    if (!IsOpenServerBracket()) ExitConfigParseError();
    InitServerConfigInfo(serverConfigInfo_);
    ParseServer();
    Print("--------------- server parse finish --------------", BOLDBLUE, 1);

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
  PrintKeyVal(key, val);

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
void ConfigParser::CheckValidation(void) const {
  std::cout << BOLDCYAN
            << "======== Validation Check Start ========" << std::endl;
  std::set<int> port_list;
  std::vector<int> duplicate_port_list;

  for (size_t i = 0; i < serverConfigInfos_.size(); ++i) {
    ServerConfigInfo info = serverConfigInfos_[i];
    if (info.port == -1 || info.body_size == 0 || info.root_path.size() == 0 ||
        !info.methods.size() || !info.error_pages.size())
      ExitConfigValidateError("Missing Server Elements");

    if (port_list.empty() || port_list.find(info.port) == port_list.end())
      port_list.insert(info.port);
    else
      duplicate_port_list.push_back(i);

    for (size_t i = 0; i < info.locations.size(); ++i)
      CheckLocation(info.locations[i]);
  }

  if (duplicate_port_list.empty())
    std::cout << "중복된 port 없음" << std::endl;
  else {
    std::cout << "중복된 port ServerConfigInfo index:";
    for (std::size_t i = 0; i < duplicate_port_list.size(); ++i)
      std::cout << " " << duplicate_port_list[i];
    std::cout << std::endl;
  }

  std::cout << "======= Validation Check Finish ========" << RESET << std::endl;
}

void ConfigParser::CheckLocation(const location& l) const {
  if (!l.is_cgi) {  // cgi가 아닌 경우
    if (l.status_code == -1 || !l.file_path.size())
      ExitConfigValidateError(
          "Missing Location Elements(status_code or file_path)");
  } else {  // cgi인 경우
    if (l.status_code == -1 || !l.cgi_pass.size())
      ExitConfigValidateError(
          "Missing Location Elements(status_code or cgi_pass)");
  }
}
