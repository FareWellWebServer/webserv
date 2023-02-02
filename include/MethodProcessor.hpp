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

#include "HTTPMessage.hpp"
#include "Data.hpp"
#include "ClientMetaData.hpp"
#include <unistd.h>

extern char **environ;

class MethodProcessor {
public:
  MethodProcessor(void);
  ~MethodProcessor(void);
  void MethodProcessorInput(ClientMetaData *clients); 

private:
  std::map<int, t_entity *> cache_entity_;
  void MakeErrorStatus(Data &client, int code);
  void FetchOiginalPath(std::string &uri);
  bool IsFetched(std::string &uri);
  bool IsExistFile(std::string &uri);
  bool IsCgi(std::string &uri);
  bool IsFile(std::string &uri, const char *identifier);
  char *CopyCstr(const char *cstr, size_t length);
  void MethodGETCgi(Data *client);
  void MethodGETFile(Data *client);
  void MethodGET(Data *client);
  void MethodHEAD(Data *client);
  void MethodPOST(Data *client);
  void MethodPUT(Data *client);
  void MethodDELETE(Data *client);

};

#endif
