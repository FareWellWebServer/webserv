#ifndef CONFIGPARSER_HPP
#define CONFIGPARSER_HPP

#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define WHITE "\033[37m"

#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

#include "ServerConfigInfo.hpp"

class ConfigParser {
 public:
  ConfigParser(const char* file_path);
  ~ConfigParser(void);

  void Parse(void);
  ServerConfigInfo ParseServer(std::istringstream &iss);

  void PrintVector(std::vector<std::string> vec);
  void Check(std::size_t pre, std::size_t cur, std::string key);
  std::vector<std::string> split(std::string line, std::string charset);

 private:
  std::string content_;

  std::vector<ServerConfigInfo> ServerConfigInfos_;
};

#endif