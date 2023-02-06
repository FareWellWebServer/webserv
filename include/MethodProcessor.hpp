#ifndef METHOD_PROCESSOR_HPP
#define METHOD_PROCESSOR_HPP

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

#define CGI ".py"
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
extern char** environ;

class MethodProcessor {
 private:
  std::map<int, t_entity*> cache_entity_;
  void MakeErrorStatus(struct Data& client, int code);
  void FetchOiginalPath(std::string& uri, struct Data& client);
  bool IsFetched(std::string& uri, struct Data& client);
  bool IsExistFile(std::string& uri);
  bool IsCgi(std::string& uri);
  bool IsFile(std::string& uri, const char* identifier);
  char* CopyCstr(const char* cstr, size_t length);
  void GETSecondCgi(int curfd, ClientMetaData* clients,
                    struct kevent& cur_event);
  void GETSecondFile(int curfd, ClientMetaData* clients,
                     struct kevent& cur_event);
  void GETSecond(int curfd, ClientMetaData* clients, struct kevent& cur_event);
  void POSTSecond(int curfd, ClientMetaData* clients, struct kevent& cur_event);
  void POSTThird(int curfd, ClientMetaData* clients, struct kevent& cur_event);
  int FileSize(const char* filepath);
  void ChangeEvents(uintptr_t ident, int16_t filter, uint16_t flags,
                    uint32_t fflags, intptr_t data, void* udata);

 public:
  /**
   * @brief Construct a new Method Processor:: Method Processor object
   *
   * @param server_list 해당 config 를 활용해서, 등록된 설정 전체를 순회하며
   * default index.html 문서를 읽습니다. index.html 문서는 read 작업을 거치지
   * 않습니다.
   */
  MethodProcessor(const std::vector<ServerConfigInfo>& server_list);
  ~MethodProcessor(void);
  /**
   * @brief
   * @param curfd
   * @param clients
   * @param change_list
   */
  void GETFirst(int curfd, ClientMetaData* clients, struct kevent& cur_event);

  /**
   * @brief
   * @param curfd
   * @param clients
   */
  void POSTFirst(int curfd, ClientMetaData* clients, struct kevent& cur_event);

  /**
   * @brief

   * @param curfd
   * @param clients
   */
  void DELETE(int curfd, ClientMetaData* clients, struct kevent& cur_event);

};

#endif
