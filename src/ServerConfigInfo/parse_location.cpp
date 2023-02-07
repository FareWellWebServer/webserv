#include "../../include/Config.hpp"

/* ========================== Location Init ========================== */
void Config::InitLocation(t_location& l, const std::string& uri) {
  l.is_cgi_ = (uri == ".py") ? true : false;
  l.cgi_pass_ = "";

  l.root_path_ = server_config_info_.root_path_;
  l.file_path_.clear();

  l.methods_ = server_config_info_.methods_;

  l.redir_path_ = "";
}

/* ========================== Parsing Location ========================== */
void Config::ParseLocation(const std::string& key, const std::string& val) {
  Print("-------------- location parse start --------------", BOLDYELLOW);
  PrintKeyVal(key, val);

  std::vector<std::string> vec = Split(val, " \t", 1);
  if (!IsOpenLocationBracket(vec)) ExitConfigParseError();

  t_location l;
  std::string location_path = vec[0];
  InitLocation(l, location_path);
  while (true) {
    ++line_num_;
    std::getline(config_stream_, line_, '\n');
    if (IsWhiteLine()) continue;

    vec = Split(line_, " \t", 1);
    if (IsCloseBracket(vec)) break;
    if (vec.size() != 2) ExitConfigParseError();

    SetLocation(l, vec[0], vec[1]);
  }
  server_config_info_.locations_[location_path] = l;
  Print("-------------- location parse finish -------------", BOLDYELLOW, 1);
}

void Config::SetLocation(t_location& l, const std::string& key,
                         const std::string& val) {
  std::vector<std::string> vec = Split(val, " ");
  PrintKeyVal(key, val);

  if (key == "redirection") {
    ParseLocationRedirection(l, vec);
  } else if (key == "method") {
    ParseLocationMethods(l, vec);
  } else if (key == "root") {
    ParseLocationRoot(l, vec);
  } else if (key == "file_path") {
    ParseLocationFilePath(l, vec);
  } else if (key == "cgi_pass") {
    ParseLocationCgi(l, vec);
  } else
    ExitConfigParseError();
}

void Config::ParseLocationRedirection(t_location& l,
                                      const std::vector<std::string>& vec) {
  if (vec.size() != 1) ExitConfigParseError();

  std::string redir_path = vec[0];

  struct stat sb;
  if (stat(("." + redir_path).c_str(), &sb) == 0) {
    l.redir_path_ = redir_path;
  } else
    ExitConfigParseError();
}

void Config::ParseLocationMethods(t_location& l,
                                  const std::vector<std::string>& vec) {
  if (!vec.size()) ExitConfigParseError();
  l.methods_.clear();

  std::string method;
  for (size_t i = 0; i < vec.size(); ++i) {
    method = vec[i];
    if (!(method == "GET" || method == "HEAD" || method == "POST" ||
          method == "PUT" || method == "DELETE")) {
      ExitConfigParseError();
    }
    l.methods_.push_back(method);
  }
}

void Config::ParseLocationRoot(t_location& l,
                               const std::vector<std::string>& vec) {
  if (vec.size() != 1) ExitConfigParseError();

  std::string root_path = vec[0];
  struct stat sb;
  if (stat(("." + root_path).c_str(), &sb) == 0)
    l.root_path_ = vec[0];
  else
    ExitConfigParseError();
}

void Config::ParseLocationFilePath(t_location& l,
                                   const std::vector<std::string>& vec) {
  if (vec.size() != 1) ExitConfigParseError();

  l.file_path_ = vec[0];
}

void Config::ParseLocationCgi(t_location& l,
                              const std::vector<std::string>& vec) {
  if (l.is_cgi_ == false || vec.size() != 1) ExitConfigParseError();
  std::string cgi_pass = vec[0];
  std::ifstream fs(cgi_pass);
  if (fs.fail()) ExitConfigParseError();
  l.cgi_pass_ = cgi_pass;
}
