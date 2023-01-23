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

const char* ConfigParser::WrongConfigSyntaxException::what(void) const throw() {
  return "[Config Error] Wrong Config Syntax";
}

const char* ConfigParser::ConfigValidationException::what(void) const throw() {
  return "[Config Error] Validation Check Failed";
}

void ConfigParser::Parse(void) {
  if (content_.size() == 0) throw WrongConfigSyntaxException();

  std::istringstream iss(content_);
  std::string line;

  while (42) {
    std::getline(iss, line, '\n');
    if (line.size() == 0) break;
    if (line.find_first_not_of(" \t") == std::string::npos) continue;

    std::vector<std::string> vec = Split(line, " \t", 1);
    if (vec.size() != 2 || vec[0] != "server" || vec[1] != "{")
      throw WrongConfigSyntaxException();

    std::cout << BOLDBLUE << "------------ server parse start ------------"
              << std::endl;
    serverConfigInfo_.ClearInfo();

    if (ParseServer(iss)) throw WrongConfigSyntaxException();

    serverConfigInfos_.push_back(serverConfigInfo_);
    std::cout << BOLDBLUE << "----------- server parse finish ------------"
              << RESET << std::endl;
  }
  std::cout << BOLDMAGENTA << "config parsing finish" << std::endl << std::endl;
}
