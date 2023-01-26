#include "../../include/Config/Config.hpp"

/* =========================== Utils Error =========================== */
void Config::ExitConfigParseError(std::string msg) const {
  std::cerr << BOLDRED << "[Config Parsing Error] ";
  if (msg.size())
    std::cerr << msg << std::endl;
  else
    std::cerr << "Line " << line_num_ << " -> " << line_ << std::endl;
  std::cerr << RESET;
  exit(EXIT_FAILURE);
}

void Config::ExitConfigValidateError(const std::string& msg) const {
  std::cerr << BOLDRED << "[Config Validate Error] " << msg << RESET
            << std::endl;
  exit(EXIT_FAILURE);
}

/* ========================== Utils Parsing ========================== */
bool Config::IsNumber(const std::string& str) const {
  for (size_t i = 0; i < str.length(); i++)
    if (!isdigit(str[i])) return false;
  return true;
}

bool Config::IsWhiteLine(void) const {
  if (line_.find_first_not_of(" \t") == std::string::npos) return true;
  return false;
}

bool Config::IsOpenServerBracket(void) const {
  std::vector<std::string> vec = Split(line_, " \t", 1);
  if (vec.size() != 2 || vec[0] != "server" || vec[1] != "{") return false;
  return true;
}

bool Config::IsOpenLocationBracket(const std::vector<std::string>& vec) const {
  if (vec.size() != 2 || vec[1] != "{") return false;
  return true;
}

bool Config::IsCloseBracket(const std::vector<std::string>& vec) const {
  if (vec.size() == 1 && vec[0] == "}") return true;
  return false;
}

std::vector<std::string> Config::Split(const std::string& str,
                                       const std::string& charset,
                                       int once) const {
  std::vector<std::string> res;
  size_t start = str.find_first_not_of(charset);
  size_t end = str.find_first_of(charset, start);

  if (end == std::string::npos) res.push_back(str.substr(start, str.size()));

  while (end != std::string::npos) {
    res.push_back(str.substr(start, end - start));
    start = str.find_first_not_of(charset, end);
    end = str.find(charset, start);
    if (start == std::string::npos) break;
    if (once) {
      res.push_back(str.substr(start, end - start));
      break;
    }
    res.push_back(str.substr(start, end - start));
  }
  return res;
}

/* =========================== Utils Print =========================== */
void Config::Print(const std::string& str, const std::string& color,
                   int reset) const {
  if (parse_mode_ == NOT_VISIBLE) return;
  if (reset)
    std::cout << color << str << RESET << std::endl;
  else
    std::cout << color << str << std::endl;
}

void Config::PrintKeyVal(const std::string& key, const std::string& val) const {
  if (parse_mode_ == NOT_VISIBLE) return;
  std::cout << "line: " << std::left << std::setw(3) << line_num_
            << "| key: " << std::left << std::setw(12) << key.c_str()
            << "| val: " << val.c_str() << std::endl;
}

void Config::PrintLocation(const location& l) const {
  if (l.is_cgi) {
    std::cout << "uri: " << l.uri << std::endl;
    std::cout << "cgi_pass: " << l.cgi_pass << std::endl;
  } else {
    std::cout << "uri: " << l.uri << std::endl;
    std::cout << "status_code: " << l.status_code << std::endl;
    std::cout << "file_path: " << l.file_path << std::endl;
    std::cout << "redir status : " << l.redir_status << std::endl;
    std::cout << "redirection_path: " << l.redir_path << std::endl;
    std::cout << "methods:";
    for (size_t i = 0; i < l.methods.size(); ++i)
      std::cout << " " << l.methods[i];
    std::cout << std::endl;
  }
}

void Config::PrintLocations(const std::vector<location>& locations) const {
  for (size_t i = 0; i < locations.size(); ++i) {
    std::cout << std::endl;
    std::cout << "------ [locations " << i << " -> cgi ";
    (locations[i].is_cgi) ? std::cout << "O" : std::cout << "X";
    std::cout << "] ------" << std::endl;
    PrintLocation(locations[i]);
  }
}

void Config::PrintConfigInfo(const ServerConfigInfo& info) const {
  std::cout << "------ [server info] ------" << std::endl;
  std::cout << "host: " << info.host << std::endl;
  std::cout << "port: " << info.port << std::endl;
  std::cout << "body_size: " << info.body_size << std::endl;
  std::cout << "root_path: " << info.root_path << std::endl;
  std::cout << "file_path: " << info.file_path << std::endl;
  std::cout << "upload_path: " << info.upload_path << std::endl;

  std::cout << "server_name: " << info.server_name << std::endl;
  std::cout << "directory_list: " << info.directory_list << std::endl;
  std::cout << "timeout: " << info.timeout << std::endl;

  std::cout << "methods:";
  PrintVector(info.methods);
  std::cout << "error_pages:";
  std::map<int, std::string>::const_iterator it;
  for (it = info.error_pages.begin(); it != info.error_pages.end(); ++it)
    std::cout << " [" << it->first << " -> " << it->second << "]";
  std::cout << std::endl;
  PrintLocations(info.locations);
}

void Config::PrintConfigInfos(void) const {
  for (size_t i = 0; i < server_config_infos_.size(); ++i) {
    std::cout << BOLDGREEN << "---------- [server config info " << i
              << "] ----------" << std::endl;
    PrintConfigInfo(server_config_infos_[i]);
    std::cout << "--------------------------------------------" << RESET
              << std::endl;
  }
}