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

enum METHOD { GET, POST, DELETE };

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

 public:
  MethodProcessor(void);
  ~MethodProcessor(void);
  int MethodProcessorInput(ClientMetaData* clients);
  void GETFirst(struct Data* client);
  void POSTFirst(struct Data* client);
  void DELETE(struct Data* client);
};

#endif
