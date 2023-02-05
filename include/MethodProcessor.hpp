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
  void GETSecondCgi(struct Data* client);
  void GETSecondFile(struct Data* client);
  void GETSecond(struct Data* client);
  void POSTSecond(struct Data* client);
  void POSTThird(struct Data* client);
  int FileSize(const char* filepath);
  void ChangeEvents(std::vector<struct kevent> change_list, uintptr_t ident,
                    int16_t filter, uint16_t flags, uint32_t fflags,
                    intptr_t data, void* udata);

 public:
  MethodProcessor(const std::vector<ServerConfigInfo>& server_list);
  ~MethodProcessor(void);
  void GETFirst(int curfd, ClientMetaData* clients,
                std::vector<struct kevent>& change_list);
  void POSTFirst(int curfd, ClientMetaData* clients);
  void DELETE(int curfd, ClientMetaData* clients);
};

#endif
