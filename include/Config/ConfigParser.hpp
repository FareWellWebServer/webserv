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

#include <sys/stat.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

#include "ServerConfigInfo.hpp"
class ConfigParser {
 public:
  ConfigParser(const char *file_path);
  ~ConfigParser(void);

  /* ======================== Utils ======================== */
  // Error
  void ExitConfigParseError(std::string msg = "") const;
  void ExitConfigValidateError(std::string msg) const;
  // Init
  void InitServerConfigInfo(ServerConfigInfo &info);
  void InitLocation(location &l, const std::string &uri);
  // Parsing
  bool IsWhiteLine(void) const;
  bool IsOpenServerBracket(void) const;
  bool IsOpenLocationBracket(const std::vector<std::string> &vec) const;
  bool IsCloseBracket(const std::vector<std::string> &vec) const;
  // Print
  void Print(const std::string &color, const std::string &str,
             int reset = 0) const;
  void PrintKeyVal(const std::string &key, const std::string &val);

  void PrintConfigInfos(void) const;
  void PrintConfigInfo(const ServerConfigInfo &info) const;
  void PrintLocations(const std::vector<location> &locations) const;
  void PrintLocation(const location &l) const;

  /* ======================== Parsing Server ======================== */
  void Parse(int print_mode = 0);
  void ParseServer(void);
  void SetServerConfigInfo(const std::string &key, const std::string &val);

  void ParseListen(const std::vector<std::string> &vec);
  void ParseBodySize(const std::vector<std::string> &vec);
  void ParseRoot(const std::vector<std::string> &vec);
  void ParseServerName(const std::vector<std::string> &vec);
  void ParseAutoindex(const std::vector<std::string> &vec);
  void ParseTimeout(const std::vector<std::string> &vec);
  void ParseMethods(const std::vector<std::string> &vec);
  void ParseErrorPage(const std::vector<std::string> &vec);

  /* ======================== Parsing Location ======================== */
  void ParseLocation(const std::string &key, const std::string &val);
  void SetLocation(location &l, const std::string &key, const std::string &val);

  void ParseLocationStatusCode(location &l,
                               const std::vector<std::string> &vec);
  void ParseLocationRedirection(location &l,
                                const std::vector<std::string> &vec);
  void ParseLocationMethods(location &l, const std::vector<std::string> &vec);
  void ParseLocationFilePath(location &l, const std::vector<std::string> &vec);
  void ParseLocationCgi(location &l, const std::vector<std::string> &vec);

  /* ======================== Validation ======================== */
  void CheckValidation(void) const;
  void CheckServerConfigInfo(const ServerConfigInfo &info) const;
  void CheckLocation(const location &l) const;

 private:
  int print_mode_;
  std::istringstream config_stream_;
  int line_num_;
  std::string line_;
  ServerConfigInfo serverConfigInfo_;
  std::vector<ServerConfigInfo> serverConfigInfos_;
};

bool IsNumber(const std::string &str);
// once == 0(default) -> 싹다 split
// once == 1 -> 한번만 split함
// EX) split("a b,c d", " ,") -> ["a", "b", "c", "d"]
// EX) split("a b,c d", " ,", 1) -> ["a", "b,c d"]
std::vector<std::string> Split(const std::string &str,
                               const std::string &charset, int once = 0);

template <typename T>
void PrintVector(const std::vector<T> &vec) {
  for (size_t i = 0; i < vec.size(); ++i) std::cout << " " << vec[i];
  std::cout << std::endl;
}

#endif