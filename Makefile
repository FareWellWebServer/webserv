NAME = farewell_webserv
RH_NAME = Req_handle
RH_D_NAME = D_Req_handle
# Compiler
CXX = c++

# Compiler flags
# CXXFLAGS = -std=c++98 -Wall -Wextra -Werror -pedantic -D SERVER=1 -D REQ_HANDLER=1 -D RES_HANDLER=1 -D CONFIG=1 -D CGI=1
CXXFLAGS = -g3 -std=c++98 -Wall -Wextra -Werror -pedantic -D SERVER=1 -D REQ_HANDLER=1 -D RES_HANDLER=1 -D CONFIG=1 -D CGI=1
#  CXXFLAGS = -g3 -fsanitize=address -D DG=1



# Colors
GREEN = \033[32m
YELLOW = \033[33m
RESET = \033[0m

SERVER_SRC	+= ./src/CGIManager/CGIMannager.cpp
SERVER_SRC	+= ./src/gnl/GetNextLine.cpp
SERVER_SRC	+= ./src/gnl/GetNextLineUtils.cpp
SERVER_SRC	+= ./src/Logger/Logger.cpp
SERVER_SRC	+= ./src/MetaData/ClientMetaData.cpp
SERVER_SRC	+= ./src/MetaData/Data.cpp
SERVER_SRC	+= ./src/MsgComposer/MsgComposer.cpp
SERVER_SRC	+= ./src/ReqHandler/reqhandler_utils.cpp
SERVER_SRC	+= ./src/ReqHandler/ReqHandler.cpp
SERVER_SRC	+= ./src/ResHandler/ResHandler.cpp
SERVER_SRC	+= ./src/Server/Server.cpp
SERVER_SRC	+= ./src/ServerConfigInfo/Config.cpp
SERVER_SRC	+= ./src/ServerConfigInfo/parse_location.cpp
SERVER_SRC	+= ./src/ServerConfigInfo/parse_server.cpp
SERVER_SRC	+= ./src/ServerConfigInfo/ServerConfigInfo.cpp
SERVER_SRC	+= ./src/ServerConfigInfo/utils.cpp
SERVER_SRC	+= ./src/Session/Session.cpp
SERVER_SRC	+= ./src/Utils/decode.cpp
SERVER_SRC	+= ./src/Utils/directory_list.cpp
SERVER_SRC	+= ./src/Utils/ErrorHandler.cpp
SERVER_SRC	+= ./src/Utils/time.cpp
SERVER_SRC	+= ./src/Utils/tostring.cpp
SERVER_SRC	+= ./src/Utils/WebServException.cpp
SERVER_SRC	+= ./src/main.cpp

SERVER_OBJ	=	$(SERVER_SRC:.cpp=.o)

# Default target
all: server

# Build the server executable
server: $(SERVER_OBJ)
	@echo "$(YELLOW)Building $@...$(RESET)"
	@$(CXX) $(CXXFLAGS) $(SERVER_OBJ) -o $(NAME)
	@echo "$(GREEN)Done.$(RESET)"

# Clean up
clean:
	@echo "$(YELLOW)Cleaning up...$(RESET)"
	@rm -rf $(SERVER_OBJ)
	@rm -rf $(REQHANDLE_OBJ)
	@echo "$(GREEN)Done.$(RESET)"

fclean:
	make clean
	@rm -f $(NAME)
	@rm -f $(RH_NAME)

re:
	make fclean
	make all

# Phony targets
.PHONY: all clean fclean re 