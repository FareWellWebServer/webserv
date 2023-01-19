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
    throw std::runtime_error("[Config Error] config file open failed");
  }
}

ConfigParser::~ConfigParser(void) {}

void ConfigParser::Parse(void) {
  if (content_.size() == 0)
    throw std::runtime_error("[Config Error] wrong config syntax");

  std::istringstream iss(content_);
  std::string line;

  while (42) {
    std::getline(iss, line, '\n');
    if (line.size() == 0) break;
    if (line.find_first_not_of(" \t") == std::string::npos) continue;

    std::vector<std::string> vec = Split(line, " \t", 1);
    if (vec.size() != 2 || vec[0] != "server" || vec[1] != "{")
      throw std::runtime_error("[Config Error] wrong config syntax");

    std::cout << BOLDBLUE << "----- server parse start -----" << std::endl;
    serverConfigInfo_.ClearInfo();

    if (ParseServer(iss))
      throw std::runtime_error("[Config Error] wrong config syntax");

    serverConfigInfos_.push_back(serverConfigInfo_);
    std::cout << BOLDBLUE << "----- server parse finish -----" << RESET
              << std::endl;
  }
  std::cout << BOLDMAGENTA << "config parsing finish" << std::endl << std::endl;
}

int ConfigParser::ParseServer(std::istringstream& iss) {
  std::string line;
  std::string key;
  std::string val;

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

  std::cout << "key: '" << key << "'" << std::endl;
  std::cout << "val: '" << val << "'" << std::endl << std::endl;

  if (key == "server_name") {
    if (vec.size() != 1) return 1;
    serverConfigInfo_.name = val;
  } else if (key == "listen") {
    if (vec.size() != 2 || !IsNumber(vec[1])) return 1;
    serverConfigInfo_.host = vec[0];
    serverConfigInfo_.port = atoi(vec[1].c_str());
  } else if (key == "body_size") {
    if (vec.size() != 1 || !IsNumber(vec[0])) return 1;
    serverConfigInfo_.body_size = atoi(vec[0].c_str());
  } else if (key == "method") {
    for (size_t i = 0; i < vec.size(); ++i)
      serverConfigInfo_.methods.push_back(vec[i]);
  } else if (key == "error_page") {
    vec = Split(val, " ");
    if (vec.size() != 2 || !IsNumber(vec[0])) return 1;
    int status_code = atoi(vec[0].c_str());
    serverConfigInfo_.error_pages[status_code] = vec[1];
  } else if (key == "location") {
    if (ParseLocation(iss, key, val)) return 1;
  } else
    return 1;
  return 0;
}

int ConfigParser::ParseLocation(std::istringstream& iss, const std::string& key,
                                const std::string& val) {
  std::cout << BOLDYELLOW << "----- location parse start -----" << std::endl;
  std::cout << "key: '" << key << "'" << std::endl;
  std::cout << "val: '" << val << "'" << std::endl << std::endl;

  std::string line;
  std::vector<std::string> vec = Split(val, " \t", 1);
  if (vec.size() != 2 || vec[1] != "{") return 1;

  location l;
  l.uri = vec[0];
  while (42) {
    std::getline(iss, line, '\n');
    if (line.find_first_not_of(" \t") == std::string::npos) continue;

    std::vector<std::string> vec = Split(line, " \t", 1);
    if (vec.size() == 1 && vec[0] == "}") break;
    if (vec.size() != 2) return 1;
    if (SetServerLocation(l, vec[0], vec[1])) return 1;
  }
  serverConfigInfo_.locations.push_back(l);
  std::cout << BOLDYELLOW << "----- location parse finish -----" << RESET
            << std::endl;
  return 0;
}

int ConfigParser::SetServerLocation(location& l, const std::string& key,
                                    const std::string& val) {
  std::vector<std::string> vec = Split(val, " ");
  std::cout << "key: '" << key << "'" << std::endl;
  std::cout << "val: '" << val << "'" << std::endl << std::endl;

  if (key == "status") {
    if (vec.size() != 1 || !IsNumber(vec[0])) return 1;
    l.status_code = atoi(vec[0].c_str());
  } else if (key == "directory_list") {
    if (vec.size() != 1 || (vec[0] != "on" && vec[0] != "off")) return 1;
    l.directory_list = (vec[0] == "on") ? true : false;
  } else if (key == "redirection") {
    if (vec.size() != 1) return 1;
    l.redirection_path = vec[0];
  } else if (key == "method") {
    for (size_t i = 0; i < vec.size(); ++i) l.methods.push_back(vec[i]);
  } else if (key == "file_path") {
    for (size_t i = 0; i < vec.size(); ++i) l.file_path.push_back(vec[i]);
  } else if (key == "cgi_pass") {
    if (vec.size() != 1) return 1;
    l.cgi_pass = vec[0];
  }
  return 0;
}

int ConfigParser::IsNumber(const std::string& str) {
  for (size_t i = 0; i < str.length() - 1; i++)
    if (!isdigit(str[i])) return 0;
  return 1;
}

void ConfigParser::PrintConfigInfo(void) {
  for (size_t i = 0; i < serverConfigInfos_.size(); ++i) {
    std::cout << BOLDGREEN << "--------- [server config info " << i
              << "] ---------" << std::endl;
    serverConfigInfos_[i].PrintInfo();
    std::cout << "------------------------------------------" << RESET
              << std::endl;
  }
}

std::vector<std::string> ConfigParser::Split(const std::string& str,
                                             const std::string& charset,
                                             int once) {
  std::vector<std::string> res;
  size_t start = str.find_first_not_of(charset);
  size_t end = str.find_first_of(charset, start);

  if (end == std::string::npos) res.push_back(str.substr(start, str.size()));

  while (end != std::string::npos) {
    res.push_back(str.substr(start, end - start));
    start = str.find_first_not_of(charset, end);
    end = str.find(charset, start);
    if (once) {
      res.push_back(str.substr(start, end - start));
      break;
    }
    if (start == std::string::npos) break;
    res.push_back(str.substr(start, end - start));
  }
  return res;
}
