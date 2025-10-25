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


int parse_input(char *input, char **args);
void	pwd_builtin();
char*	get_username();
void	cd_builtin(char **env);
char	*get_env_home(char **env);
#endif
