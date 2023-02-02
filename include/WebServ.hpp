#ifndef WEBSERV_HPP
#define WEBSERV_HPP

#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define WHITE "\033[37m"
#define RESET "\033[0m"
#define BOLDRED "\033[1m\033[31m"
#define BOLDGREEN "\033[1m\033[32m"
#define BOLDYELLOW "\033[1m\033[33m"
#define BOLDBLUE "\033[1m\033[34m"
#define BOLDMAGENTA "\033[1m\033[35m"
#define BOLDCYAN "\033[1m\033[36m"
#define BOLDWHITE "\033[1m\033[37m"

#define BUFFER_SIZE 1024

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <fstream>
#include <iostream>
#include <map>
#include <string>

#include "HTTPMassage.hpp"
#include "ClientMetaData.hpp"
#include "ServerConfigInfo.hpp"
#include "Config.hpp"
#include "ErrorHandler.hpp"
#include "GetNextLine.hpp"
#include "Logger.hpp"
#include "Server.hpp"
#include "ReqHandler.hpp"
#include "WebServException.hpp"
#include "MethodProcessor.hpp"
#include "MsgComposer.hpp"


// #include "Data.hpp"
#endif
