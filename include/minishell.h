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

typedef struct s_shell
{
	char	**env_list;
	char	*env_value;
	char	*pwd;
	char	*old_pwd;
}	t_shell;

/*parsing */
int parse_input(char *input, char **args, int *quotetype);

/*env variables*/
void copy_envlist(t_shell *shell, char *env[]);


/* built-in */
char	*get_env_home(char *env[]);
void	pwd_builtin();
int	cd_builtin(char *arg, char *path, char *env[]);
#endif
