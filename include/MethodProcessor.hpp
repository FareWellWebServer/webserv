#ifndef MethodProcessor_hpp_
#define MethodProcessor_hpp_

#include <sys/event.h>
#include <sys/stat.h>

#include <algorithm>
#include <ctime>
#include <exception>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <vector>

#define RESET "\033[0m"
#define BOLDRED "\033[1m\033[31m"
#define BOLDGREEN "\033[1m\033[32m"
#define BOLDYELLOW "\033[1m\033[33m"
#define BOLDBLUE "\033[1m\033[34m"
#define BOLDMAGENTA "\033[1m\033[35m"
#define BOLDCYAN "\033[1m\033[36m"
#define BOLDWHITE "\033[1m\033[37m"

typedef struct s_entity {
  char *entity_data_;
  size_t entity_length_;
  char *entity_type_;
} t_entity;

typedef struct t_req_msg {
  /// @brief first line data : method, uri
  std::string method_;
  std::string req_uri_;
  std::string protocol_;
  std::map<std::string, std::string> headers_;
  t_entity body_data_;
} s_req_msg;

typedef struct t_res_msg {
  /// @brief first line data : method, uri
  std::string method_;
  std::string req_uri_;
  std::string protocol_;
  std::map<std::string, std::string> headers_;
} s_res_msg;

struct location {
  std::string uri;       // 없을 수가 없는 변수
  int status_code;       // 필수 입력 O
  std::string file_path; // 필수 입력 O

  std::vector<std::string> methods; // 필수 입력 X

  // redir_path -> 필수 입력 X
  int redir_status;
  std::string redir_path;

  bool is_cgi;
  std::string cgi_pass;
};

class ServerConfigInfo {
public:
  ServerConfigInfo(void);
  ~ServerConfigInfo(void);

  /* ======================== Variable ======================== */
  std::string host;        // default -> 127.0.0.1
  int port;                // 필수 입력 O
  int body_size;           // 필수 입력 O
  std::string root_path;   // 필수 입력 O
  std::string file_path;   // 필수 입력 O
  std::string upload_path; // 필수 입력 O

  std::string server_name; // 필수 입력 X
  bool directory_list;     // 필수 입력 X
  int timeout;             // 필수 입력 X

  std::vector<std::string> methods;       // 필수 입력 O
  std::map<int, std::string> error_pages; // 필수 입력 O

  std::vector<location> locations; // 필수 입력 X
};

class Config {
public:
  Config(const char *file_path);
  ~Config(void);

  typedef std::vector<std::string> value;

  /* ======================== Getter ======================== */
  std::vector<ServerConfigInfo> GetServerConfigInfos(void);

  /* ======================== Parsing Server ======================== */
  void InitServerConfigInfo(ServerConfigInfo &info);

  void Parse(int print_mode = 0);
  void ParseServer(void);
  void SetServerConfigInfo(const std::string &key, const std::string &val);

  void ParseListen(const value &vec);
  void ParseBodySize(const value &vec);
  void ParseRoot(const value &vec);
  void ParseFilePath(const value &vec);
  void ParseUploadPath(const value &vec);
  void ParseServerName(const value &vec);
  void ParseDirectoryList(const value &vec);
  void ParseTimeout(const value &vec);
  void ParseMethods(const value &vec);
  void ParseErrorPage(const value &vec);

  /* ======================== Parsing Location ======================== */
  void InitLocation(location &l, const std::string &uri);

  void ParseLocation(const std::string &key, const std::string &val);
  void SetLocation(location &l, const std::string &key, const std::string &val);

  void ParseLocationStatusCode(location &l, const value &vec);
  void ParseLocationRedirection(location &l, const value &vec);
  void ParseLocationMethods(location &l, const value &vec);
  void ParseLocationFilePath(location &l, const value &vec);
  void ParseLocationCgi(location &l, const value &vec);

  /* ======================== Validation ======================== */
  bool CheckDuplicatePort(int port) const;
  void CheckValidation(void) const;
  void CheckLocation(const location &l) const;

  /* ======================== Utils ======================== */
  // Error
  void ExitConfigParseError(std::string msg = "") const;
  void ExitConfigValidateError(const std::string &msg) const;
  // Parsing
  bool IsNumber(const std::string &str) const;
  bool IsWhiteLine(void) const;
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
  void PrintLocations(const std::vector<location> &locations) const;
  void PrintLocation(const location &l) const;

private:
  int print_mode_;
  std::istringstream config_stream_;
  int line_num_;
  std::string line_;

  ServerConfigInfo server_config_info_;
  std::vector<ServerConfigInfo> server_config_infos_;
};

template <typename T> void PrintVector(const std::vector<T> &vec) {
  for (size_t i = 0; i < vec.size(); ++i)
    std::cout << " " << vec[i];
  std::cout << std::endl;
}

/**
 * @brief 임시 데이터 덩어리(client)
 *
 */
struct Data // struct로
{
  int litsen_fd_; // 어느 listen fd에 연결됐는지
  int port_; // listen fd에 bind 되어있는 port 번호. config볼 때 필요
  // int client_fd_;
  struct kevent *event_; // fd(ident), flag들
  ServerConfigInfo *config_;
  struct t_req_msg *req_message_; // HTTP 요청/응답 헤더 구분 어떻게?
  struct t_res_msg *res_message_; // 같은 클래스로? 다른 클래스로?
  int status_code_;               // 상태코드 enum 정의 필요
  t_entity *entity_;              // 응답 본문
} Data;

/**
 * @brief 데이터 노드를 합쳐둔 메타 데이터
 *
 */
class ClientMetaData {
private:
  std::map<int, struct Data> datas_;
  int current_fd_;
  void ValidCheckToAccessData();
  class WrongFd : public std::exception {
  public:
    { const char *what() const throw(); }
  };
  void InitializeData(struct Data *data);

public:
  ClientMetaData();
  ~ClientMetaData();
  // accept 된 client 의 kevent 발생시 설정
  void SetCurrentFd(const int &client_fd);
  // kevent에서 listen fd가 read 했을 때, accept할 때, listen_fd저장, port번호
  // 저장
  void AddData(const int &listen_fd, const int &client_fd, const int &port);
  void SetConfig(struct kevent &event);
  // accept 된 fd를 close할때, map에서 지워줌
  void DeleteByFd(const int &client_fd);
  // reqHandler에서 요청헤더 파싱 후
  void SetReqMessage(struct HTTPMessage *header);
  // MsgComposer에서
  void SetResMessage(struct HTTPMessage *header);
  // core에서 처리 후
  void SetEntity(char *entitiy);

  // data 통채로 원할 때
  struct Data *GetData(int current_fd_);
  // core에서 요청 헤더 데이터 필요할 때
  struct HTTPMessage *GetReqHeader();
  // core에서 응답 헤더 데이터 필요할 때
  struct HTTPMessage *GetResHeader();
  // core에서 헤더 데이터 필요할 때
  ServerConfigInfo *GetConfig();

  int GetPort();
  int GetDataCount(void); // return data_.size();
  // 루프 처음시작할때 errorHandler에 *나& 로 넣어주면서 인스턴스화 해도 될듯?
  // 그러면 한번만 호출해도 되니까
  int GetStatusCode();
  // Method 전체 리턴
  std::vector<std::string> GetMethods();
  // Method 있는지 확인
  bool FindMethods(std::string method);
  char *GetURL();
};

#define CGI ".py"
#define INSTALLPATH "/original/path"
#define INDEX "index.html"
#define PNG ".png"
#define JPG ".jpg"
#define ICO ".ico"
#define BUFFER_MAX 1024

#define TYPE_HTML "text/html"
#define TYPE_ICON "image/vnd.microsoft.icon"
#define TYPE_JPEG "image/jpeg"
#define TYPE_PNG "image/png"
#define TYPE_DEFAULT "text/plain"

#include <unistd.h>

extern char **environ;

class MethodProcessor {
private:
  static std::map<int, t_entity *> cache_entity_;
  void MakeErrorStatus(struct Data &client, int code) {
    client.status_code_ = code;
    if (client.entity_->entity_data_)
      delete[] client.entity_->entity_data_;
    client.entity_->entity_data_ = NULL;
    client.entity_->entity_type_ = NULL;
    client.entity_->entity_length_ = 0;
  }
  void FetchOiginalPath(std::string &uri) {
    uri.erase(0);
    uri.insert(0, INSTALLPATH);
    return;
  }
  bool IsFetched(std::string &uri) {
    size_t i = 0;

    while (INSTALLPATH[i]) {
      if (INSTALLPATH[i] == uri[i]) {
        i++;
      } else {
        return (false);
      }
    }
    return (true);
  }
  bool IsExistFile(std::string &uri) {
    const char *temp = uri.c_str();

    int ret = access(temp, F_OK);
    delete[] temp;

    if (ret != 0)
      return (false);
    return (true);
  }
  bool IsCgi(std::string &uri) {
    std::string::reverse_iterator it = uri.rbegin();
    if (it[0] == CGI[2]) {
      if (it[1] == CGI[1]) {
        if (it[2] == CGI[0])
          return (true);
      }
    } else
      return (false);
  }
  bool IsFile(std::string &uri, const char *identifier) {
    std::string::reverse_iterator it = uri.rbegin();

    size_t len = strlen(identifier);

    for (size_t i = 0; i < len; i++) {
      if (it[i] == identifier[len - i])
        continue;
      else
        return (false);
    }
    return (true);
  }
  char *CopyCstr(const char *cstr, size_t length) {
    char *ret = new char[length + 1];
    if (ret == NULL) {
      /*error handling*/;
    }
    ret[length] = '\0';
    for (size_t i = 0; i < length; i += 2) {
      ret[i] = cstr[i];
      if (i + 1 < length)
        ret[i + 1] = cstr[i + 1];
    }
    return ret;
  }
  void MethodGETCgi(struct Data *client) {
    int cgi_stream[2];
    int pid = 0;

    if (pipe(cgi_stream) == -1) {
      /*error handling */
    }
    pid = fork();
    if (pid == 0) {
      /* CGI handling*/
      close(cgi_stream[0]);
      dup2(cgi_stream[1], 1);

      char **cgi_argv;
      size_t cgi_length = client->req_message_->req_uri_.size();

      cgi_argv = new char *[2];
      cgi_argv[0] = new char[cgi_length + 1];
      cgi_argv[0][cgi_length] = '\0';
      for (size_t i = 0; i < cgi_length; i += 2) {
        cgi_argv[0][i] = client->req_message_->req_uri_.at(i);
        if (i + 1 < cgi_length)
          cgi_argv[0][i + 1] = client->req_message_->req_uri_.at(i + 1);
      }
      cgi_argv[1] = new char[1];
      cgi_argv[1][0] = '\0';
      execve(cgi_argv[0], cgi_argv, environ);
      // TODO : error handling
    } else {
      /* Parent handling*/
      close(cgi_stream[1]);

      int waitloc;
      if (wait(&waitloc) == -1) { /*error handling*/
      } else {
        if (WIFEXITED(waitloc)) {
          /* normal handling*/
          char *buf;
          size_t len = 0;

          buf = new char[BUFFER_MAX + 1];
          std::string temp_data;
          while (true) {
            len = read(cgi_stream[0], buf, BUFFER_MAX);
            if (len == -1) {
              /* error handling */
            }
            buf[BUFFER_MAX] = '\0';
            temp_data.append(buf);
            if (len != BUFFER_MAX) {
              delete[] buf;
              break;
            }
            size_t temp_length = temp_data.size();
            client->entity_->entity_length_ = temp_length;
            client->entity_->entity_data_ =
                CopyCstr(temp_data.c_str(), temp_length);
          }
          else if (WIFSIGNALED(waitloc)) {
            /* error handling */;
          }
          close(cgi_stream[0]);
          return;
        }
      }
    }
  }
  void MethodGETFile(struct Data *client) {
    struct entity *ret;
    ret = new struct entity();
    if (ret == NULL) {
      /* error handling */
    }
    std::ifstream entityFile;

    entityFile.open(client->req_message_->req_uri_, std::ifstream::in);
    entityFile.seekg(0, entityFile.end);
    ret->entity_length_ = entityFile.tellg();
    entityFile.seekg(0, entityFile.beg);

    ret->entity_data_ = new char[ret->entity_length_ + 1];
    ret->entity_data_[ret->entity_length_] = '\0';

    entityFile.read(ret->entity_data_, ret->entity_length_);

    client->entity_->entity_data_ = ret->entity_data_;
    return;
  }
  void MethodGET(struct Data *client) {
    if (!IsFetched(client->req_message_->req_uri_))
      FetchOiginalPath(client->req_message_->req_uri_);
    if (!IsExistFile(client->req_message_->req_uri_)) {
      MakeErrorStatus(*client, 404);
      return;
    }
    if (IsCgi(client->req_message_->req_uri_)) {
      MethodGETCgi(client);
      return;
    }
    if (IsFile(client->req_message_->req_uri_, PNG) ||
        IsFile(client->req_message_->req_uri_, JPG) ||
        IsFile(client->req_message_->req_uri_, ICO)) {
      MethodGETFile(client);
      if (IsFile(client->req_message_->req_uri_, PNG))
        client->entity_->entity_type_ = TYPE_PNG;
      else if (IsFile(client->req_message_->req_uri_, JPG))
        client->entity_->entity_type_ = TYPE_JPEG;
      else if (IsFile(client->req_message_->req_uri_, ICO))
        client->entity_->entity_type_ = TYPE_ICON;
      else
        client->entity_->entity_type_ = TYPE_DEFAULT;
      return;
    }
    std::map<int, t_entity *>::iterator check_cache =
        cache_entity_.find(client->port_);
    if (check_cache != cache_entity_.end()) {
      client->entity_->entity_length_ = check_cache->second->entity_length_;
      client->entity_->entity_data_ = CopyCstr(
          check_cache->second->entity_data_, client->entity_->entity_length_);
      client->entity_->entity_type_ = check_cache->second->entity_type_;
      return;
    }

    t_entity *ret;
    ret = new t_entity();
    if (ret == NULL) {
      MakeErrorStatus(*client, 500);
      return;
    }
    std::ifstream entityFile;

    entityFile.open(client->req_message_->req_uri_, std::ifstream::in);
    entityFile.seekg(0, entityFile.end);
    ret->entity_length_ = entityFile.tellg();
    entityFile.seekg(0, entityFile.beg);

    ret->entity_data_ = new char[ret->entity_length_];
    if (!ret->entity_data_) {
      MakeErrorStatus(*client, 500);
      return;
    }

    entityFile.read(ret->entity_data_, ret->entity_length_);

    client->entity_->entity_data_ = ret->entity_data_;
    client->entity_->entity_type_ = TYPE_HTML;
    cache_entity_.emplace(client->port_, ret);
    return;
  }
  void MethodHEAD(struct Data *client) {
    MethodGET(client);
    delete[] client->entity_->entity_data_;
    client->entity_->entity_data_ = NULL;
    return;
  }
  void MethodPOST(struct Data *client);
  void MethodPUT(struct Data *client);
  void MethodDELETE(struct Data *client);

public:
  MethodProcessor(void);
  ~MethodProcessor(void);
  void MethodProcessorInput(ClientMetaData *clients) {
    struct Data *client;
    client = clients->GetData(clients->GetPort());
    if (client->req_message_->method_ == "GET")
      MethodGET(client);
    else if (client->req_message_->method_ == "HEAD")
      MethodHEAD(client);
    else if (client->req_message_->method_ == "POST")
      MethodPOST(client);
    else if (client->req_message_->method_ == "PUT")
      MethodPUT(client);
    else
      MethodDELETE(client);
  }
};
#endif