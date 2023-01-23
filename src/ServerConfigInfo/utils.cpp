#include "../../include/Config/ConfigParser.hpp"

/* =========================== Utils Error =========================== */
void ConfigParser::ExitConfigParseError(void) const {
  std::cerr << BOLDRED << "[Config Error] ";
  if (line_num_ == -1)
    std::cerr << "Empty Config File" << std::endl;
  else
    std::cerr << "Line " << line_num_ << " -> " << line_ << std::endl;
  std::cerr << RESET;
  exit(EXIT_FAILURE);
}

/* =========================== Utils Init =========================== */
void ConfigParser::InitLocation(location& l, const std::string& uri) {
  l.uri = uri;
  l.status_code = -1;
  l.file_path.clear();

  l.methods = serverConfigInfo_.methods;

  l.redir_status = -1;
  l.redir_path = "";

  l.is_cgi = false;
  l.cgi_pass = "";
}

void ConfigParser::InitServerConfigInfo(ServerConfigInfo& info) {
  info.host = "127.0.0.1";
  info.port = -1;
  info.body_size = 0;
  info.root_path = "";

  info.server_name = "";
  info.autoindex = false;
  info.keep_alive_time = 0;

  info.methods.clear();
  info.error_pages.clear();
  info.locations.clear();
}

/* =========================== Utils Print =========================== */
void ConfigParser::PrintLocation(const location& l) const {
  if (l.is_cgi) {
    std::cout << "uri: " << l.uri << std::endl;
    std::cout << "cgi_pass: " << l.cgi_pass << std::endl;
  } else {
    std::cout << "uri: " << l.uri << std::endl;
    std::cout << "status_code: " << l.status_code << std::endl;
    std::cout << "redir status : " << l.redir_status << std::endl;
    std::cout << "redirection_path: " << l.redir_path << std::endl;
    std::cout << "methods:";
    for (size_t i = 0; i < l.methods.size(); ++i)
      std::cout << " " << l.methods[i];
    std::cout << std::endl;
    std::cout << "file_path:";
    for (size_t i = 0; i < l.file_path.size(); ++i)
      std::cout << " " << l.file_path[i];
    std::cout << std::endl;
  }
}

void ConfigParser::PrintLocations(
    const std::vector<location>& locations) const {
  for (size_t i = 0; i < locations.size(); ++i) {
    std::cout << std::endl;
    std::cout << "------ [locations " << i << " -> cgi ";
    (locations[i].is_cgi) ? std::cout << "O" : std::cout << "X";
    std::cout << "] ------" << std::endl;
    PrintLocation(locations[i]);
  }
}

void ConfigParser::PrintConfigInfo(const ServerConfigInfo& info) const {
  std::cout << "------ [server info] ------" << std::endl;
  std::cout << "host: " << info.host << std::endl;
  std::cout << "port: " << info.port << std::endl;
  std::cout << "body_size: " << info.body_size << std::endl;
  std::cout << "root_path: " << info.root_path << std::endl;

  std::cout << "server_name: " << info.server_name << std::endl;
  std::cout << "autoindex: " << info.autoindex << std::endl;
  std::cout << "keep_alive_time: " << info.keep_alive_time << std::endl;

  std::cout << "methods:";
  PrintVector(info.methods);
  std::cout << "error_pages:";
  std::map<int, std::string>::const_iterator it;
  for (it = info.error_pages.begin(); it != info.error_pages.end(); ++it)
    std::cout << " [" << it->first << " -> " << it->second << "]";
  std::cout << std::endl;
  PrintLocations(info.locations);
}

void ConfigParser::PrintConfigInfos(void) const {
  for (size_t i = 0; i < serverConfigInfos_.size(); ++i) {
    std::cout << BOLDGREEN << "---------- [server config info " << i
              << "] ----------" << std::endl;
    PrintConfigInfo(serverConfigInfos_[i]);
    std::cout << "--------------------------------------------" << RESET
              << std::endl;
  }
}

/* =========================== Utils =========================== */
int IsNumber(const std::string& str) {
  for (size_t i = 0; i < str.length(); i++)
    if (!isdigit(str[i])) return 0;
  return 1;
}

std::vector<std::string> Split(const std::string& str,
                               const std::string& charset, int once) {
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
