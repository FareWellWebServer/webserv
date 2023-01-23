#include "../../include/WebServ.hpp"

ConfigParser::ConfigParser(const char* file_path) {
  std::cout << "config file path: " << file_path << std::endl;

  std::ifstream fs(file_path);
  std::string line;

  if (fs.is_open()) {
    while (!fs.eof()) {
      getline(fs, line);
      if (line.size()) content_.append(line + "\n");
    }
    fs.close();
  } else {
    throw std::runtime_error("[Config Error] Config File Open Failed");
  }
}

ConfigParser::~ConfigParser(void) {}

const char* ConfigParser::ConfigValidationException::what(void) const throw() {
  return "[Config Error] Validation Check Failed";
}

void ConfigParser::Parse(void) {
  line_num_ = -1;
  if (content_.size() == 0) ExitConfigParseError();

  std::istringstream iss(content_);

  while (42) {
    ++line_num_;
    std::getline(iss, line_, '\n');

    if (iss.eof()) break;
    if (IsWhiteLine()) continue;

    Print("------------- server parse start -------------", BOLDBLUE);
    CheckFirstLine();
    InitServerConfigInfo(serverConfigInfo_);
    ParseServer(iss);
    Print("------------- server parse finish ------------", BOLDBLUE, 1);

    serverConfigInfos_.push_back(serverConfigInfo_);
  }
  Print("config parsing finish", BOLDMAGENTA, 1);
}

void ConfigParser::ParseServer(std::istringstream& iss) {
  while (42) {
    ++line_num_;
    std::getline(iss, line_, '\n');
    if (IsWhiteLine()) continue;

    std::vector<std::string> vec = Split(line_, " \t", 1);
    if (vec.size() == 1 && vec[0] == "}") break;
    if (vec.size() != 2) ExitConfigParseError();

    SetServerConfigInfo(iss, vec[0], vec[1]);
  }
}

void ConfigParser::SetServerConfigInfo(std::istringstream& iss,
                                       const std::string& key,
                                       const std::string& val) {
  std::vector<std::string> vec = Split(val, " ");
  printf("key: %-18s| val: %s\n", key.c_str(), val.c_str());

  if (key == "listen") {
    if (vec.size() != 1 || !IsNumber(vec[0])) ExitConfigParseError();
    serverConfigInfo_.port = atoi(vec[0].c_str());
  } else if (key == "body_size") {
    if (vec.size() != 1 || !IsNumber(vec[0])) ExitConfigParseError();
    serverConfigInfo_.body_size = atoi(vec[0].c_str());
  } else if (key == "root") {
    if (vec.size() != 1) ExitConfigParseError();
    serverConfigInfo_.root_path = vec[0];
  } else if (key == "server_name") {
    if (vec.size() != 1) ExitConfigParseError();
    serverConfigInfo_.server_name = val;
  } else if (key == "autoindex") {
    if (vec.size() != 1 || (vec[0] != "on" && vec[0] != "off"))
      ExitConfigParseError();
    serverConfigInfo_.autoindex = (vec[0] == "on") ? true : false;
  } else if (key == "keepalive_timeout") {
    if (!IsNumber(vec[0])) ExitConfigParseError();
    serverConfigInfo_.keep_alive_time = atoi(vec[0].c_str());
  } else if (key == "method") {
    for (size_t i = 0; i < vec.size(); ++i)
      serverConfigInfo_.methods.push_back(vec[i]);
  } else if (key == "error_page") {
    if (vec.size() != 2 || !IsNumber(vec[0])) ExitConfigParseError();
    int status_code = atoi(vec[0].c_str());
    serverConfigInfo_.error_pages[status_code] = vec[1];
  } else if (key == "location") {
    ParseLocation(iss, key, val);
  } else
    ExitConfigParseError();
}

void ConfigParser::ParseLocation(std::istringstream& iss,
                                 const std::string& key,
                                 const std::string& val) {
  Print("------------ location parse start ------------", BOLDYELLOW);
  printf("key: %-18s| val: %s\n", key.c_str(), val.c_str());

  std::vector<std::string> vec = Split(val, " \t", 1);
  if (vec.size() != 2 || vec[1] != "{") ExitConfigParseError();

  location l;
  InitLocation(l, vec[0]);
  while (42) {
    ++line_num_;
    std::getline(iss, line_, '\n');
    if (IsWhiteLine()) continue;

    std::vector<std::string> vec = Split(line_, " \t", 1);
    if (vec.size() == 1 && vec[0] == "}") break;
    if (vec.size() != 2) ExitConfigParseError();
    SetLocation(l, vec[0], vec[1]);
  }
  serverConfigInfo_.locations.push_back(l);
  Print("------------ location parse finish -----------", BOLDYELLOW, 1);
}

void ConfigParser::SetLocation(location& l, const std::string& key,
                               const std::string& val) {
  std::vector<std::string> vec = Split(val, " ");
  printf("key: %-18s| val: %s\n", key.c_str(), val.c_str());

  if (key == "status_code") {
    if (vec.size() != 1 || !IsNumber(vec[0])) ExitConfigParseError();
    l.status_code = atoi(vec[0].c_str());
  } else if (key == "redirection") {
    if (vec.size() != 2 || !IsNumber(vec[0])) ExitConfigParseError();
    l.redir_status = atoi(vec[0].c_str());
    l.redir_path = vec[1];
  } else if (key == "method") {
    for (size_t i = 0; i < vec.size(); ++i) l.methods.push_back(vec[i]);
  } else if (key == "file_path") {
    for (size_t i = 0; i < vec.size(); ++i) l.file_path.push_back(vec[i]);
  } else if (key == "cgi_pass") {
    if (l.is_cgi == false || vec.size() != 1) ExitConfigParseError();
    l.cgi_pass = vec[0];
  }
}
