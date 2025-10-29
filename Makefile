NAME = minishell
CC = gcc
CFLAGS =  -Wall -Werror -Wextra
SRC_DIR = src
SRC = basicshell.c builtin.c envlist.c
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

