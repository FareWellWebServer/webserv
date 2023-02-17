NAME = farewell_webserv
SHELL = bash
# Compiler
CXX = c++

# Compiler flags
CXXFLAGS = -std=c++98 -Wall -Wextra -Werror -pedantic
# CXXFLAGS = -g3 -std=c++98 -Wall -Wextra -Werror -pedantic -D SERVER=1 -D REQ_HANDLER=1 -D RES_HANDLER=1 -D CONFIG=1 -D CGI=1

# =================CGIManger==================
SERVER_SRC	+= ./src/CGIManager/CGIMannager.cpp

# =================Logger=====================
SERVER_SRC	+= ./src/Logger/Logger.cpp

# =================MetaData===================
SERVER_SRC	+= ./src/MetaData/ClientMetaData.cpp
SERVER_SRC	+= ./src/MetaData/Data.cpp

# =================MsgComposer================
SERVER_SRC	+= ./src/MsgComposer/MsgComposer.cpp

# =================ReqHandler=================
SERVER_SRC	+= ./src/ReqHandler/reqhandler_utils.cpp
SERVER_SRC	+= ./src/ReqHandler/ReqHandler.cpp

# =================Server=====================
SERVER_SRC	+= ./src/Server/Server.cpp
SERVER_SRC	+= ./src/Server/ServerExecuteLog.cpp
SERVER_SRC	+= ./src/Server/ServerExecuteProcess.cpp
SERVER_SRC	+= ./src/Server/ServerExecuteRead.cpp
SERVER_SRC	+= ./src/Server/ServerExecuteTimer.cpp
SERVER_SRC	+= ./src/Server/ServerExecuteWrite.cpp
SERVER_SRC	+= ./src/Server/ServerMethod.cpp
SERVER_SRC	+= ./src/Server/ServerUtils.cpp


# =================ServerConfigInfo=====================
SERVER_SRC	+= ./src/ServerConfigInfo/Config.cpp
SERVER_SRC	+= ./src/ServerConfigInfo/parse_location.cpp
SERVER_SRC	+= ./src/ServerConfigInfo/parse_server.cpp
SERVER_SRC	+= ./src/ServerConfigInfo/ServerConfigInfo.cpp
SERVER_SRC	+= ./src/ServerConfigInfo/utils.cpp

# =================Session====================
SERVER_SRC	+= ./src/Session/Session.cpp

# =================Utils======================
SERVER_SRC	+= ./src/Utils/decode.cpp
SERVER_SRC	+= ./src/Utils/directory_list.cpp
SERVER_SRC	+= ./src/Utils/ErrorHandler.cpp
SERVER_SRC	+= ./src/Utils/time.cpp
SERVER_SRC	+= ./src/Utils/tostring.cpp
SERVER_SRC	+= ./src/Utils/WebServException.cpp

# main.cpp
SERVER_SRC	+= ./src/main.cpp

SERVER_OBJ	=	$(SERVER_SRC:.cpp=.o)

# Default target
all: server

# Build the server executable
server: $(SERVER_OBJ)
	@./build.sh
	@$(CXX) $(CXXFLAGS) $(SERVER_OBJ) -o $(NAME)
	@echo "$(GREEN)Done.$(RESET)"

# Clean up
clean:
	@rm -rf $(SERVER_OBJ)
	@echo "clean Done."

fclean:
	make clean
	@rm -f $(NAME)

re:
	make fclean
	make all

# Phony targets
.PHONY: all clean fclean re 