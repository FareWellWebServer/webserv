#include "../../include/WebServ.hpp"

int ConfigParser::IsWhiteLine(void) const {
  if (line_.find_first_not_of(" \t") == std::string::npos) return 1;
  return 0;
}

void ConfigParser::CheckFirstLine(void) const {
  std::vector<std::string> vec = Split(line_, " \t", 1);
  if (vec.size() != 2 || vec[0] != "server" || vec[1] != "{")
    ExitConfigParseError();
}