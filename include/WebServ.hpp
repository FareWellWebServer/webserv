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

#include "ClientMetaData.hpp"
#include "Color.hpp"
#include "Config.hpp"
#include "Data.hpp"
#include "ErrorHandler.hpp"
#include "GetNextLine.hpp"
#include "HTTPMessage.hpp"
#include "Logger.hpp"
#include "MethodProcessor.hpp"
#include "MsgComposer.hpp"
#include "ReqHandler.hpp"
<<<<<<< HEAD
#include "ReqHandler_d.hpp"
=======
#include "Server.hpp"
#include "ServerConfigInfo.hpp"
#include "Stage.hpp"
>>>>>>> develop
#include "WebServException.hpp"

#endif
