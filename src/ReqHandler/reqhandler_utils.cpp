#include "../../include/ReqHandler.hpp"

std::vector<std::string> s_split(std::string& s, const std::string& delimiter,
                                 int cnt) {
  std::vector<std::string> tokens;
  std::string token;
  std::string::size_type pos = 0;
  while ((pos = s.find(delimiter)) != std::string::npos) {
    token = s.substr(0, pos);
    tokens.push_back(token);
    s.erase(0, pos + delimiter.length());
    if (cnt == 1) break;
  }
  tokens.push_back(s);
  return tokens;
}
