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

  /* ======================== Error ======================== */
  class ConfigValidationException : public std::exception {
   public:
    const char *what(void) const throw();
  };

  /* ======================== Utils ======================== */
  // Error
  void ExitConfigParseError(void) const;
  // Init
  void InitServerConfigInfo(ServerConfigInfo &info);
  void InitLocation(location &l, const std::string &uri);
  // Print
  void Print(const std::string color, const std::string str,
             int reset = 0) const;
  void PrintConfigInfos(void) const;
  void PrintConfigInfo(const ServerConfigInfo &info) const;
  void PrintLocations(const std::vector<location> &locations) const;
  void PrintLocation(const location &l) const;

  /* ======================== Parsing Server ======================== */
  void Parse(void);
  void ParseServer(std::istringstream &iss);
  void SetServerConfigInfo(std::istringstream &iss, const std::string &key,
                           const std::string &val);

  int IsWhiteLine(void) const;
  void CheckFirstLine(void) const;

  /* ======================== Parsing Location ======================== */
  void ParseLocation(std::istringstream &iss, const std::string &key,
                     const std::string &val);
  void SetLocation(location &l, const std::string &key, const std::string &val);

  /* ======================== Validation ======================== */
  void ValidationCheck(void) const;

 private:
  std::string content_;
  int line_num_;
  std::string line_;
  ServerConfigInfo serverConfigInfo_;
  std::vector<ServerConfigInfo> serverConfigInfos_;
};

int IsNumber(const std::string &str);
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