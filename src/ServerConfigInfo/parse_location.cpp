#include "../../include/WebServ.hpp"

/* ========================== Parsing Location ========================== */
void Config::ParseLocation(const std::string& key, const std::string& val) {
  Print("-------------- location parse start --------------", BOLDYELLOW);
  PrintKeyVal(key, val);

  std::vector<std::string> vec = Split(val, " \t", 1);
  if (!IsOpenLocationBracket(vec)) ExitConfigParseError();

  location l;
  InitLocation(l, vec[0]);
  while (true) {
    ++line_num_;
    std::getline(config_stream_, line_, '\n');
    if (IsWhiteLine()) continue;

    vec = Split(line_, " \t", 1);
    if (IsCloseBracket(vec)) break;
    if (vec.size() != 2) ExitConfigParseError();

    SetLocation(l, vec[0], vec[1]);
  }
  server_config_info_.locations.push_back(l);
  Print("-------------- location parse finish -------------", BOLDYELLOW, 1);
}

void Config::SetLocation(location& l, const std::string& key,
                         const std::string& val) {
  std::vector<std::string> vec = Split(val, " ");
  PrintKeyVal(key, val);

  if (key == "status_code") {
    ParseLocationStatusCode(l, vec);
  } else if (key == "redirection") {
    ParseLocationRedirection(l, vec);
  } else if (key == "method") {
    ParseLocationMethods(l, vec);
  } else if (key == "file_path") {
    ParseLocationFilePath(l, vec);
  } else if (key == "cgi_pass") {
    ParseLocationCgi(l, vec);
  } else
    ExitConfigParseError();
}

void Config::ParseLocationStatusCode(location& l,
                                     const std::vector<std::string>& vec) {
  if (vec.size() != 1 || !IsNumber(vec[0])) ExitConfigParseError();

  int status_code = atoi(vec[0].c_str());
  if (status_code < 100 || status_code > 511) ExitConfigParseError();
  l.status_code = status_code;
}

void Config::ParseLocationRedirection(location& l,
                                      const std::vector<std::string>& vec) {
  if (vec.size() != 2 || !IsNumber(vec[0])) ExitConfigParseError();

  int redir_status_code = atoi(vec[0].c_str());
  if (redir_status_code < 100 || redir_status_code > 511)
    ExitConfigParseError();
  l.redir_status = redir_status_code;

  std::string redir_path = vec[1];
  struct stat sb;
  if (stat(redir_path.c_str(), &sb) == 0)
    l.redir_path = redir_path;
  else
    ExitConfigParseError();
}

void Config::ParseLocationMethods(location& l,
                                  const std::vector<std::string>& vec) {
  if (!vec.size()) ExitConfigParseError();
  l.methods.clear();

  std::string method;
  for (size_t i = 0; i < vec.size(); ++i) {
    method = vec[i];
    if (!(method == "GET" || method == "HEAD" || method == "POST" ||
          method == "PUT" || method == "DELETE"))
      ExitConfigParseError();
    l.methods.push_back(method);
  }
}

void Config::ParseLocationFilePath(location& l,
                                   const std::vector<std::string>& vec) {
  if (!vec.size()) ExitConfigParseError();

  for (size_t i = 0; i < vec.size(); ++i) {
    std::ifstream fs(vec[i]);
    if (fs.fail()) ExitConfigParseError();
    l.file_path.push_back(vec[i]);
  }
}

void Config::ParseLocationCgi(location& l,
                              const std::vector<std::string>& vec) {
  if (l.is_cgi == false || vec.size() != 1) ExitConfigParseError();
  std::string cgi_pass = vec[0];
  std::ifstream fs(cgi_pass);
  if (fs.fail()) ExitConfigParseError();
  l.cgi_pass = cgi_pass;
}