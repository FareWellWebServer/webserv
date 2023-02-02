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
  std::map<int, t_entity *> cache_entity_;
  void MakeErrorStatus(struct Data &client, int code);
  void FetchOiginalPath(std::string &uri);
  bool IsFetched(std::string &uri);
  bool IsExistFile(std::string &uri);
  bool IsCgi(std::string &uri);
  bool IsFile(std::string &uri, const char *identifier);
  char *CopyCstr(const char *cstr, size_t length);
  void MethodGETCgi(struct Data *client);
  void MethodGETFile(struct Data *client);
  void MethodGET(struct Data *client);
  void MethodHEAD(struct Data *client);
  void MethodPOST(struct Data *client);
  void MethodPUT(struct Data *client);
  void MethodDELETE(struct Data *client);

public:
  MethodProcessor(void);
  ~MethodProcessor(void);
  void MethodProcessorInput(ClientMetaData *clients); 
};

#endif
