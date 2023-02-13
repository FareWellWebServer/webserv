NAME = farewell_webserv
RH_NAME = Req_handle
RH_D_NAME = D_Req_handle
# Compiler
CXX = c++

# Compiler flags
CXXFLAGS = -g3 -fsanitize=address -std=c++98 -Wall -Wextra -Werror -pedantic -D SERVER=1 -D REQ_HANDLER=1 -D RES_HANDLER=1 -D CONFIG=1 -D CGI=1
#  CXXFLAGS = -g3 -fsanitize=address -D DG=1



# Colors
GREEN = \033[32m
YELLOW = \033[33m
RESET = \033[0m

SERVER_SRC	=	$(wildcard ./src/*.cpp) $(wildcard ./src/*/*.cpp)
SERVER_OBJ	=	$(SERVER_SRC:.cpp=.o)

REQHANDLE_SRC = $(wildcard ./src/ReqHandler/*.cpp) $(wildcard ./src/gnl/*.cpp) ./src/rq_main.cpp
REQHANDLE_OBJ = $(REQHANDLE_SRC:.cpp=.o)

REQHANDLE_D_SRC = ./src/rq_main.cpp ./src/test_req.cpp
REQHANDLE_D_OBJ = $(REQHANDLE_D_SRC:.cpp=.o)

# Default target
all: server

# Build the server executable
server: $(SERVER_OBJ)
	@echo "$(YELLOW)Building $@...$(RESET)"
	@$(CXX) $(CXXFLAGS) $(SERVER_OBJ) -o $(NAME)
	@echo "$(GREEN)Done.$(RESET)"

rp: $(REQHANDLE_OBJ)
	@echo "$(YELLOW)Req_Handle_Building $@...$(RESET)"
	@$(CXX) $(CXXFLAGS) $(REQHANDLE_OBJ) -o $(RH_NAME)
	@echo "$(GREEN)Done.$(RESET)"

dp: $(REQHANDLE_D_OBJ)
	@echo "$(YELLOW)Req_Handle_Building $@...$(RESET)"
	@$(CXX) $(CXXFLAGS) $(REQHANDLE_D_OBJ) -o $(RH_D_NAME)
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