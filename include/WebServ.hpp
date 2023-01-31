#ifndef WEBSERV_HPP
#define WEBSERV_HPP

#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define WHITE "\033[37m"

#define BUFFER_SIZE 1024

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <fstream>
#include <iostream>
#include <map>
#include <string>

#include "ClientMetaData.hpp"
#include "Config/Config.hpp"
#include "Config/ServerConfigInfo.hpp"
#include "ErrorHandler.hpp"
#include "GetNextLine.hpp"
#include "Logger.hpp"
#include "Server.hpp"
#include "ReqHandler.hpp"
#include "WebServException.hpp"
#endif
