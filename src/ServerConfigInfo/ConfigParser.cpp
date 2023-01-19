#include "../../include/ConfigParser.hpp"

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

void ConfigParser::parse(void) {
  std::cout << "config parsing start" << std::endl;
}