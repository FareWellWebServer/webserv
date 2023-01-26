#ifndef CONFIG_HPP
#define CONFIG_HPP

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
#include <iomanip>
#include <iostream>
#include <set>
#include <sstream>
#include <vector>

#include "ServerConfigInfo.hpp"
class Config {
 public:
  Config(const char *file_path);
  ~Config(void);

  /* ======================== Utils ======================== */
  // Error
  void ExitConfigParseError(std::string msg = "") const;
  void ExitConfigValidateError(const std::string &msg) const;
  // Parsing
  bool IsNumber(const std::string &str) const;
  bool IsWhiteLine(void) const;
  bool IsOpenServerBracket(void) const;
  bool IsOpenLocationBracket(const std::vector<std::string> &vec) const;
  bool IsCloseBracket(const std::vector<std::string> &vec) const;
  std::vector<std::string> Split(const std::string &str,
                                 const std::string &charset,
                                 int once = 0) const;
  // Print
  void Print(const std::string &color, const std::string &str,
             int reset = 0) const;
  void PrintKeyVal(const std::string &key, const std::string &val) const;

  void PrintConfigInfos(void) const;
  void PrintConfigInfo(const ServerConfigInfo &info) const;
  void PrintLocations(const std::vector<location> &locations) const;
  void PrintLocation(const location &l) const;

  /* ======================== Parsing Server ======================== */
  void InitServerConfigInfo(ServerConfigInfo &info);

  void Parse(int print_mode = 0);
  void ParseServer(void);
  void SetServerConfigInfo(const std::string &key, const std::string &val);

  void ParseListen(const std::vector<std::string> &vec);
  void ParseBodySize(const std::vector<std::string> &vec);
  void ParseRoot(const std::vector<std::string> &vec);
  void ParseFilePath(const std::vector<std::string> &vec);
  void ParseUploadPath(const std::vector<std::string> &vec);
  void ParseServerName(const std::vector<std::string> &vec);
  void ParseDirectoryList(const std::vector<std::string> &vec);
  void ParseTimeout(const std::vector<std::string> &vec);
  void ParseMethods(const std::vector<std::string> &vec);
  void ParseErrorPage(const std::vector<std::string> &vec);

  /* ======================== Parsing Location ======================== */
  void InitLocation(location &l, const std::string &uri);

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
  bool CheckDuplicatePort(int port) const;
  void CheckValidation(void) const;
  void CheckLocation(const location &l) const;

 private:
  int print_mode_;
  std::istringstream config_stream_;
  int line_num_;
  std::string line_;

  ServerConfigInfo server_config_info_;
  std::vector<ServerConfigInfo> server_config_infos_;
};

template <typename T>
void PrintVector(const std::vector<T> &vec) {
  for (size_t i = 0; i < vec.size(); ++i) std::cout << " " << vec[i];
  std::cout << std::endl;
}

#endif