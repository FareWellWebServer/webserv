#include "../../include/ServerConfigInfo.hpp"

ServerConfigInfo::ServerConfigInfo(const char* file_path) {
  std::cout << "config file path: " << file_path << std::endl;

  std::ifstream fs(file_path);
  std::string line;

  if (fs.is_open()) {
    while (!fs.eof()) {
      getline(fs, line);
      content.append(line + "\n");
    }
    fs.close();
  } else {
    throw std::runtime_error("[Config Error] config file open failed");
  }
}

ServerConfigInfo::~ServerConfigInfo(void) {}