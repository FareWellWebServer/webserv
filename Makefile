NAME = farewell_webserv
# Compiler
CXX = c++

# Compiler flags
#CXXFLAGS = -std=c++98 -Wall -Wextra -Werror -pedantic
 CXXFLAGS = -g3 -fsanitize=address -D DG=1

# Colors
GREEN = \033[32m
YELLOW = \033[33m
RESET = \033[0m

SERVER_SRC	=	$(wildcard ./src/*.cpp) $(wildcard ./src/*/*.cpp)
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
	@echo "$(GREEN)Done.$(RESET)"

fclean:
	make clean
	@rm -f $(NAME)

re:
	make fclean
	make all

# Phony targets
.PHONY: all clean fclean re 