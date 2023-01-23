#include "../../include/WebServ.hpp"

void ConfigParser::ClearLocation(location& l) {
  l.status_code = -1;
  l.methods = serverConfigInfo_.methods;
  l.file_path.clear();

  l.redir_status = -1;
  l.redir_path = "";

  l.is_cgi = false;
  l.cgi_pass = "";
}

int ConfigParser::ParseLocation(std::istringstream& iss, const std::string& key,
                                const std::string& val) {
  std::cout << BOLDYELLOW << "----------- location parse start -----------"
            << std::endl;
  printf("key: %-15s| val: %s\n", key.c_str(), val.c_str());

  std::string line;
  std::vector<std::string> vec = Split(val, " \t", 1);
  if (vec.size() != 2 || vec[1] != "{") return 1;

  location l;
  ClearLocation(l);
  l.uri = vec[0];
  if (vec[0] == ".py") l.is_cgi = true;
  while (42) {
    std::getline(iss, line, '\n');
    if (line.find_first_not_of(" \t") == std::string::npos) continue;

    std::vector<std::string> vec = Split(line, " \t", 1);
    if (vec.size() == 1 && vec[0] == "}") break;
    if (vec.size() != 2) return 1;
    if (SetServerLocation(l, vec[0], vec[1])) return 1;
  }
  serverConfigInfo_.locations.push_back(l);
  std::cout << BOLDYELLOW << "---------- location parse finish -----------"
            << RESET << std::endl;
  return 0;
}
