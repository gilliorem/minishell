NAME = minishell
CC = cc
CFLAGS = -Wall -Werror -Wextra -Iinclude
SRC_DIR = src
SRC = signal.c signal_two.c utils.c free.c free_two.c init.c init_two.c init_three.c\
      lexer.c parser.c parser_two.c heredocs.c heredocs_two.c \
	  expander_str.c expander_str_two.c expander_str_three.c expander_heredocs.c \
	  executor.c executor_two.c executor_three.c executor_four.c \
	  ft_cd.c ft_echo.c ft_exit.c ft_export.c ft_export_two.c ft_unset.c ft_pwd.c \
	  main.c
OBJ = $(addprefix $(SRC_DIR)/,$(SRC:.c=.o))
INCLUDES =  -Iinclude -Ilibft 
LIBFT_DIR = libft
LIBFT = $(LIBFT_DIR)/libft.a

all: $(NAME)

$(NAME): $(OBJ) $(LIBFT)
	$(CC) $(INCLUDES) $(CFLAGS) $(OBJ) $(LIBFT) -o $(NAME) -lreadline

$(SRC_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(LIBFT):
	$(MAKE) -C $(LIBFT_DIR)

clean:
	rm -f $(OBJ)
	$(MAKE) -C $(LIBFT_DIR) fclean

fclean: clean
	rm -f $(NAME)
	$(MAKE) -C $(LIBFT_DIR) fclean

re: fclean all
.PHONY: all clean fclean re

