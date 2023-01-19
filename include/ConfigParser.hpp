#ifndef CONFIGPARSER_HPP
#define CONFIGPARSER_HPP

#define RESET "\033[0m"
#define BOLDRED "\033[1m\033[31m"
#define BOLDGREEN "\033[1m\033[32m"
#define BOLDYELLOW "\033[1m\033[33m"
#define BOLDBLUE "\033[1m\033[34m"
#define BOLDMAGENTA "\033[1m\033[35m"
#define BOLDCYAN "\033[1m\033[36m"
#define BOLDWHITE "\033[1m\033[37m"

#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

#include "ServerConfigInfo.hpp"

class ConfigParser {
 public:
  ConfigParser(const char *file_path);
  ~ConfigParser(void);

  int IsNumber(const std::string &str);
  // split("a b,c", " ,") -> ["a", "b", "c"]
  // once == 0(default) -> 싹다 split함
  // once == 1 -> 한번만 split함
  std::vector<std::string> Split(const std::string &line,
                                 const std::string &charset, int once = 0);
  void PrintConfigInfo(void);

  void Parse(void);
  int ParseServer(std::istringstream &iss);
  int SetServerConfigInfo(std::istringstream &iss, const std::string &key,
                          const std::string &val);
  int ParseLocation(std::istringstream &iss, const std::string &key,
                    const std::string &val);
  int SetServerLocation(location &l, const std::string &key,
                        const std::string &val);

 private:
  std::string content_;
  ServerConfigInfo serverConfigInfo_;
  std::vector<ServerConfigInfo> serverConfigInfos_;
};

#endif