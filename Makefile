SRCS = srcs/philosopher.c srcs/utils.c

NAME = philo
CC = cc
CFLAGS = # -Wall -Werror -Wextra -g



OBJS = $(SRCS:srcs/%.c=$(OBJS_DIR)/%.o)

OBJS_DIR = ./objs

B_BLUE = \033[1;36m
B_GREEN = \033[1;32m
B_WHITE = \033[1;37m
RESET = \033[0m


all: $(NAME)

$(OBJS_DIR)/%.o: srcs/%.c
	@mkdir -p $(OBJS_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(NAME): $(OBJS)
	@if [ ! -f $(NAME) ]; \
	then \
		echo "\n$(B_BLUE)Compiling $(NAME)...$(B_WHITE)\n"; \
		echo "$(CC) $(OBJS) -o $(NAME)"; \
		$(CC) $(OBJS) -o $(NAME); \
		echo "\n$(B_GREEN)$(NAME) compiled successfully!$(B_WHITE)\n"; \
	fi

clean:
	@echo "\n$(B_BLUE)Cleaning objs and objs directory...$(B_WHITE)\n"
	rm -rf $(OBJS_DIR)
	@echo "\n$(B_GREEN)Objs files was removed successfully!$(B_WHITE)\n"
	@echo "$(RESET)"

fclean: clean
	@echo "\n$(B_BLUE)Removing executables...$(B_WHITE)\n"
	rm -rf $(NAME)
	@echo "\n$(B_GREEN)All files was removed successfully!$(B_WHITE)\n"
	@echo "$(RESET)"

re: fclean all

.PHONY: all clean fclean re