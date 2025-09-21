NAME = minishell
CC = cc
CFLAGS = -Wall -Werror -Wextra 
SRC_DIR = src
SRC = main.c

OBJ = $(addprefix $(SRC_DIR)/,$(SRC:.c=.o))
INCLUDES = -lreadline

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(INCLUDES) $(CFLAGS) $(OBJ) -o $(NAME)

$(SRC_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all
.PHONY: all clean fclean re

