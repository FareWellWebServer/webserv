NAME = testdebug
# Compiler
CC = c++

# Compiler flags
CFLAGS = -std=c++98 -Wall -Wextra -Werror -pedantic

# Colors
GREEN = \033[32m
YELLOW = \033[33m
RESET = \033[0m

SRC	=	debugingtest.cpp
OBJ	=	$(SRC:.cpp = .o)

# Build the test executable
.cpp.o: 
	@$(CC) $(CFLAGS) -c -o $@ $<

# Default target
all: $(NAME)

$(NAME): $(OBJ)
	@$(CC) $(CFLAGS) -o $(NAME) $(OBJ)

# Clean up
clean:
	@echo "$(YELLOW)Cleaning up...$(RESET)"
	@rm -rf *.o
	@rm -rf $(NAME).dSYM
	@echo "$(GREEN)Done.$(RESET)"

fclean:
	make clean
	@rm -f $(NAME)

re:
	make fclean
	make all

debug:
	$(CC) $(CFLAGS) -g3 -D DG=1 -o $(NAME) $(OBJ)

# Phony targets
.PHONY: all clean fclean re 