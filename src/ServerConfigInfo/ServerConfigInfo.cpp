#include "../../include/ServerConfigInfo.hpp"

ServerConfigInfo::ServerConfigInfo(const char* file_path)
    : file_path_(file_path) {
  std::cout << "config file path: " << file_path_ << std::endl;
}

ServerConfigInfo::~ServerConfigInfo(void) {}