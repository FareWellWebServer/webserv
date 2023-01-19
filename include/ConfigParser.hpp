#ifndef CONFIGPARSER_HPP
#define CONFIGPARSER_HPP

#include <fstream>
#include <iostream>
#include <vector>

#include "ServerConfigInfo.hpp"

class ConfigParser {
 public:
  ConfigParser(const char* file_path);
  ~ConfigParser(void);

  void parse(void);

 private:
  std::string content_;

  std::vector<ServerConfigInfo> ServerConfigInfos_;
};

#endif