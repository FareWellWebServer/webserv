#include "../../include/ReqHandler.hpp"

std::vector<std::string> s_split(std::string& s, const std::string& delimiter,
                                 int cnt) {
  std::vector<std::string> tokens;
  std::string token;
  std::string::size_type pos = 0;
  while ((pos = s.find(delimiter)) != std::string::npos) {
    token = s.substr(0, pos);
    tokens.push_back(token);
    s.erase(0, pos + delimiter.length());
    if (cnt == 1) break;
  }
  tokens.push_back(s);
  return tokens;
}

void RemoveTabSpace(std::string& str) {
  for (size_t i = 0; i < str.length(); i++) {
    if (str[i] == '\t' || str[i] == ' ') {
      str.erase(i, 1);
      --i;
    }
  }
}

void ReduceSlash(std::string& tmp) {
  size_t pos;

  while ((pos = tmp.find("//")) != std::string::npos) {
    tmp.replace(pos, 2, "/");
  }
}

bool CheckValidPath(const std::string& file_path) {
  struct stat sb;
  if (stat(file_path.c_str(), &sb) == 0) {
    return true;
  } else {
    return false;
  }
}

std::vector<std::string> split(const std::string& s, char delimiter, int cnt) {
  std::vector<std::string> tokens;
  std::string token;
  std::stringstream tokenStream(s);

  while (std::getline(tokenStream, token, delimiter)) {
    tokens.push_back(token);
    if (cnt == 1) {
      token = tokenStream.str();
      tokens.push_back(token);
      break;
    }
  }
  return tokens;
}

void PrintMap(std::map<std::string, std::string>& map) {
  std::cout << RED << "[[PRINT MAP]]" << RESET << std::endl;
  for (std::map<std::string, std::string>::iterator it = map.begin();
       it != map.end(); ++it) {
    std::cout << "key : " << it->first << " || value : " << it->second
              << std::endl;
  }
}
