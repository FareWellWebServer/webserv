#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <sys/stat.h>

#include <fstream>
#include <iomanip>
#include <iostream>
#include <set>
#include <sstream>
#include <vector>

#include "Color.hpp"
#include "ServerConfigInfo.hpp"

class Config {
 public:
  Config(const char *file_path);
  ~Config(void);

  typedef std::vector<std::string> value;

  /* ======================== Getter ======================== */
  std::vector<ServerConfigInfo> GetServerConfigInfos(void) const;
  std::string GetLogPath(void) const;

  /* ======================== Parsing Server ======================== */
  void InitServerConfigInfo(ServerConfigInfo &info);

  void Parse(int print_mode = 0);
  void ParseServer(void);
  void SetServerConfigInfo(const std::string &key, const std::string &val);

  void ParseListen(const value &vec);
  void ParseBodySize(const value &vec);
  void ParseRoot(const value &vec);
  void ParseUploadPath(const value &vec);
  void ParseTimeout(const value &vec);
  void ParseMethods(const value &vec);
  void ParseErrorPage(const value &vec);
  void ParseServerName(const value &vec);

  /* ======================== Parsing Location ======================== */
  void InitLocation(t_location &l);

  void ParseLocation(const std::string &key, const std::string &val);
  void SetLocation(t_location &l, const std::string &key,
                   const std::string &val);

  void ParseLocationFilePath(t_location &l, const value &vec);
  void ParseLocationMethods(t_location &l, const value &vec);
  void ParseLocationDirectoryList(t_location &l, const value &vec);
  void ParseLocationRoot(t_location &l, const value &vec);
  void ParseLocationRedirection(t_location &l, const value &vec);

  void ParseLocationIsCgi(t_location &l, const value &vec);
  void ParseLocationCgiPass(t_location &l, const value &vec);

  // void ParseLocationStatusCode(t_location &l, const value &vec);

  /* ======================== Validation ======================== */
  bool CheckDuplicatePort(int port) const;
  bool CheckValidPath(const std::string &file_path) const;
  void CheckValidation(void);
  void CheckLocation(t_location &loc);

  /* ======================== Utils ======================== */
  // Error
  void ExitConfigParseError(std::string msg = "") const;
  void ExitConfigValidateError(const std::string &msg) const;
  // Parsing
  bool IsNumber(const std::string &str) const;
  bool IsWhiteLine(void) const;
  bool IsLogPath(void);
  bool IsOpenServerBracket(void) const;
  bool IsOpenLocationBracket(const value &vec) const;
  bool IsCloseBracket(const value &vec) const;
  value Split(const std::string &str, const std::string &charset,
              int once = 0) const;
  // Print
  void Print(const std::string &color, const std::string &str,
             int reset = 0) const;
  void PrintKeyVal(const std::string &key, const std::string &val) const;
  void PrintConfigInfos(void) const;
  void PrintConfigInfo(const ServerConfigInfo &info) const;
  void PrintLocations(const std::map<std::string, t_location> &locations) const;
  void PrintLocation(const t_location &l) const;

 private:
  int print_mode_;
  std::istringstream config_stream_;
  int line_num_;
  std::string line_;

  std::string log_path_;
  ServerConfigInfo server_config_info_;
  std::vector<ServerConfigInfo> server_config_infos_;
};

template <typename T>
void PrintVector(const std::vector<T> &vec) {
  for (size_t i = 0; i < vec.size(); ++i) {
    std::cout << " " << vec[i];
  }
  std::cout << std::endl;
}

#endif
