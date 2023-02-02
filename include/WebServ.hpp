#ifndef WEBSERV_HPP
#define WEBSERV_HPP

#define BUFFER_SIZE 1024

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <fstream>
#include <iostream>
#include <map>
#include <string>

#include "Color.hpp"
#include "Types.hpp"
#include "HTTPMessage.hpp"
#include "Data.hpp"
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

#endif
