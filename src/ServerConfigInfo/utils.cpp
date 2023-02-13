#include "../../include/Config.hpp"

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

bool Config::IsLogPath(void) {
  std::vector<std::string> vec = Split(line_, " \t", 1);
  if (vec.size() != 2 || vec[0] != "log_path") {
    return 0;
  }

  std::string log_path = vec[1];
  struct stat sb;
  if (stat(log_path.c_str(), &sb) != 0) {
    std::string mkdir = "mkdir ";
    std::string cmd = mkdir + vec[1];
    system(cmd.c_str());
  }
  log_path_ = log_path;
  return 1;
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

  if (end == std::string::npos) {
    res.push_back(str.substr(start, str.size()));
    return res;
  }
  res.push_back(str.substr(start, end - start));
  while (end != std::string::npos) {
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
  if (!print_mode_) return;
  if (reset)
    std::cout << color << str << RESET << std::endl;
  else
    std::cout << color << str << std::endl;
}

void Config::PrintKeyVal(const std::string& key, const std::string& val) const {
  if (!print_mode_) return;
  std::cout << "line: " << std::left << std::setw(3) << line_num_
            << "| key: " << std::left << std::setw(12) << key.c_str()
            << "| val: " << val.c_str() << std::endl;
}

void Config::PrintLocation(const t_location& l) const {
  if (l.is_cgi_) {
    std::cout << "cgi_pass:";
    for (size_t i = 0; i < l.cgi_path_.size(); ++i) {
      std::cout << " " << l.cgi_path_[i];
    }
    std::cout << std::endl;
  } else {
    std::cout << "file_path:";
    for (size_t i = 0; i < l.file_path_.size(); ++i) {
      std::cout << " " << l.file_path_[i];
    }
    std::cout << std::endl;

    std::cout << "directory_list: " << l.directory_list_ << std::endl;
    if (!l.root_path_.empty())
      std::cout << "root_path: " << l.root_path_ << std::endl;
    if (!l.redir_path_.empty())
      std::cout << "redirection_path: " << l.redir_path_ << std::endl;
  }
  std::cout << "methods:";
  for (size_t i = 0; i < l.methods_.size(); ++i)
    std::cout << " " << l.methods_[i];
  std::cout << std::endl;
}

void Config::PrintLocations(
    const std::map<std::string, t_location>& locations) const {
  std::map<std::string, t_location>::const_iterator it = locations.begin();
  int i = 0;
  for (; it != locations.end(); ++it) {
    std::cout << std::endl;
    std::cout << "------ [locations " << i++ << " -> cgi ";
    (it->second.is_cgi_) ? std::cout << "O" : std::cout << "X";
    std::cout << "] ------" << std::endl;
    std::cout << "uri: " << it->first << std::endl;
    PrintLocation(it->second);
  }
}

void Config::PrintConfigInfo(const ServerConfigInfo& info) const {
  std::cout << "------ [server info] ------" << std::endl;
  std::cout << "host: " << info.host_ << std::endl;
  std::cout << "port: " << info.port_ << std::endl;
  std::cout << "body_size: " << info.body_size_ << std::endl;
  std::cout << "root_path: " << info.root_path_ << std::endl;
  std::cout << "upload_path: " << info.upload_path_ << std::endl;
  std::cout << "timeout: " << info.timeout_ << std::endl;

  std::cout << "methods:";
  PrintVector(info.methods_);
  std::cout << "error_pages:";
  std::map<int, std::string>::const_iterator it;
  for (it = info.error_pages_.begin(); it != info.error_pages_.end(); ++it)
    std::cout << " [" << it->first << " -> " << it->second << "]";
  std::cout << std::endl;

  std::cout << "server_name: " << info.server_name_ << std::endl;
  PrintLocations(info.locations_);
}

void Config::PrintConfigInfos(void) const {
  for (size_t i = 0; i < server_config_infos_.size(); ++i) {
    std::cout << BOLDCYAN << "---------- [server config info " << i
              << "] ----------" << std::endl;
    std::cout << "log_path: " << log_path_ << std::endl;
    PrintConfigInfo(server_config_infos_[i]);
    std::cout << "--------------------------------------------" << RESET
              << std::endl;
  }
}
