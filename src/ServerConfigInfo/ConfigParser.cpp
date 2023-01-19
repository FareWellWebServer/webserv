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

void ConfigParser::Parse(void) {
  std::cout << "config parsing start" << std::endl;

  if (content_.size() == 0)
    throw std::runtime_error("[Config Error] wrong config syntax");

  std::istringstream iss(content_);
  std::string line;

  while (42) {
    std::getline(iss, line, '\n');
    std::cout << "line: " << line << std::endl;
    std::vector<std::string> vec = split(line, " \t\n");

    if (vec.size() != 2 || vec[0] != "server" || vec[1] != "{")
      throw std::runtime_error("[Config Error] wrong config syntax");
    ServerConfigInfo serverConfigInfo = ParseServer(iss);
    ServerConfigInfos_.push_back(serverConfigInfo);
    break;
  }
  std::cout << "config parsing finish" << std::endl;
}

ServerConfigInfo ConfigParser::ParseServer(std::istringstream& iss) {
  ServerConfigInfo serverConfigInfo;
  std::string line;

  while (42) {
    std::getline(iss, line, '\n');
    std::cout << "line: " << line << std::endl;

    if (!line.size())
      throw std::runtime_error("[Config Error] wrong config syntax");

    std::vector<std::string> vec = split(line, " \t\n");
    if (vec.size() == 1 && vec[0] == "}") break;
  }

  return serverConfigInfo;
}

void ConfigParser::Check(std::size_t pre, std::size_t cur, std::string key) {
  if (pre != std::string::npos)
    std::cout << "pre: " << pre << " -> " << content_[pre] << std::endl;
  if (cur != std::string::npos)
    std::cout << "cur: " << cur << " -> " << content_[cur] << std::endl;
  if (key.size()) std::cout << "key: " << key << std::endl;
  std::cout << std::endl;
}

std::vector<std::string> ConfigParser::split(std::string str,
                                             std::string charset) {
  std::vector<std::string> res;
  std::string s;
  size_t start = str.find_first_not_of(charset);
  size_t end;
  str += charset[0];

  while (42) {
    end = str.find_first_of(charset, start);
    if (end == std::string::npos) {
      break;
    }
    s = str.substr(start, end - start);
    res.push_back(s);
    start = str.find_first_not_of(charset, end);
    if (start == std::string::npos) {
      break;
    }
  }
  PrintVector(res);
  return res;
}

void ConfigParser::PrintVector(std::vector<std::string> vec) {
  std::vector<std::string>::iterator start;
  std::vector<std::string>::iterator it;
  std::cout << "split result: ";
  for (it = vec.begin(); it != vec.end(); ++it) {
    std::cout << RED << *it;
    if (std::distance(vec.begin(), it) != vec.size() - 1)
      std::cout << WHITE << ", ";
  }
  std::cout << WHITE << std::endl;
}