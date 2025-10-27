#ifndef MINISHELL_H
# define MINISHELL_H
# include "../libft/libft.h"
# include <stdlib.h>
# include <stdio.h>
# include <string.h>
# include <sys/wait.h>
# include <readline/readline.h>
# include <readline/history.h>
# include <unistd.h>
# include <signal.h>
# include <stdbool.h>


int parse_input(char *input, char **args, int *quotetype);
void	pwd_builtin();
char*	get_username();
int	cd_builtin(char *arg, char *path, char *env[]);
char	*get_env_home(char **env);
#endif
