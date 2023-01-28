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

struct location {
  std::string uri;                     // 없을 수가 없는 변수
  int status_code;                     // 필수 입력 O
  std::vector<std::string> file_path;  // 필수 입력 O

  std::vector<std::string> methods;  // 필수 입력 X

  // redir_path -> 필수 입력 X
  int redir_status;
  std::string redir_path;

  bool is_cgi;
  std::string cgi_pass;
};

class ServerConfigInfo {
  // private가 없는 이유
  // 값 저장하려면 변수 별로 setter 함수를 만들어야 되서 일단 public으로
  // 진행했습니다
 public:
  ServerConfigInfo(void);
  ~ServerConfigInfo(void);

  /* ======================== Variable ======================== */
  std::string host;       // default -> 127.0.0.1
  int port;               // 필수 입력 O
  int body_size;          // 필수 입력 O
  std::string root_path;  // 필수 입력 O

  std::string server_name;  // 필수 입력 X

  // 이름 변경된 변수 -> directory_list -> autoindex
  bool autoindex;  // 필수 입력 X

  // 논의 필요한 변수 keep_alive_time
  // default를 정해준다고 치면 필수는 아닌데 정의는 되어야함
  int timeout;  // 필수 입력 X

  std::vector<std::string> methods;        // 필수 입력 O
  std::map<int, std::string> error_pages;  // 필수 입력 O
  std::vector<location> locations;         // 필수 입력 X
};

class Config {
 public:
  Config(const char *file_path);
  ~Config(void);

  /* ======================== Utils ======================== */
  // Error
  void ExitConfigParseError(std::string msg = "") const;
  void ExitConfigValidateError(const std::string &msg) const;
  // Init
  void InitServerConfigInfo(ServerConfigInfo &info);
  void InitLocation(location &l, const std::string &uri);
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

/**
 * @brief 임시 데이터 덩어리(client)
 *
 */
struct Data  // struct로
{
  int litsen_fd_;  // 어느 listen fd에 연결됐는지
  int port_;  // listen fd에 bind 되어있는 port 번호. config볼 때 필요
  // int client_fd_;
  struct kevent *event_;  // fd(ident), flag들
  ServerConfigInfo *config_;
  struct HTTPMessage *req_message_;  // HTTP 요청/응답 헤더 구분 어떻게?
  struct HTTPMessage *res_message_;  // 같은 클래스로? 다른 클래스로?
  int status_code_;                  // 상태코드 enum 정의 필요
  const struct entity_t *entity_;    // 응답 본문
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
  struct Data &GetData(int current_fd_);
  // core에서 요청 헤더 데이터 필요할 때
  struct HTTPMessage *GetReqHeader();
  // core에서 응답 헤더 데이터 필요할 때
  struct HTTPMessage *GetResHeader();
  // core에서 헤더 데이터 필요할 때
  ServerConfigInfo *GetConfig();

  int GetPort();
  int GetDataCount(void);  // return data_.size();
  // 루프 처음시작할때 errorHandler에 *나& 로 넣어주면서 인스턴스화 해도 될듯?
  // 그러면 한번만 호출해도 되니까
  int GetStatusCode();
  // Method 전체 리턴
  std::vector<std::string> GetMethods();
  // Method 있는지 확인
  bool FindMethods(std::string method);
  char *GetURL();
};

struct entity {
  char *entity_;
  size_t entity_length_;
} entity_t;

class MethodProcessor {
 private:
  bool isExistFile(std::string filePath_);
  static std::map<int, struct entity_t *> cache_Entity_;

  struct entity_t *GET(std::string filePath_) {
    struct entity_t *ret;

    // 들어오는 URI 방식
    // default page => / (root)
    // default page 의 절대 경로를 알아야 한다.
    // 들어오는 그 다음 페이지는 => /src/~~ 이런식으로 붙음. 즉, uri 에서 핵심은
    // default 로 들어오는 루트 구별, 거기서 핵심은 포트일듯, 이후 이를 다
    // 합쳐서 구분하는 것이 핵심이다.
    // 즉 단순 client meta data 뿐만 아니라 서버 config 도 들어와서 이를 동시에
    // 확인해야함.

    /* 전체 로직 정리 */
    // 파일 존재 여부 확인

    // NO : 404 file notfound?
    //// entity char * = NULL
    //// entity_length_ = -1;
    //// error code  = setting like HTTP

    // YES : std::map <std::string, struct entity_t *> 에 등록된 URI인지 확인.

    //// Hit : 해당 데이터 만 옮기고 리턴

    //// miss : 해당 데이터 찾기 로직으로 들어감
    ////// 파일 열기
    ////// 파일 읽기 로직 진행 + content length 넣기
    ////// 일반적인 index.html 문서의 경우 이에 대한 캐싱 작업(std::map.insert)
    ////// 반환

    return (ret);
  }
  /// @brief GET 과 동일하게 진행하며, 단순 메서드로 구분한다.
  struct entity_t *HEAD(std::string filePath_);
  void POST();
  void DELETE();
  void PUT();

 public:
  MethodProcessor(void);
  ~MethodProcessor(void);
  void MethodProcessorInput(ClientMetaData *clients) {
    // 요청 메서드 구분
    // switch 로 구분하여 들어가야할 메서드 처리 함수 메서드로 진입
    // 정리가 마무리 되면 그대로 반환
  }
};
#endif