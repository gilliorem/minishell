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

// TODO: upgrade the structure for the current args[0]
// we treat a cmd as a whole unit by itself even if there are spaces in between its options.

typedef enum e_tokentype 
{
	TOKEN_WORD,
	TOKEN_PIPE,
	TOKEN_REDIR_IN,
	TOKEN_REDIR_OUT,
	TOKEN_REDIR_APPEND,
	TOKEN_HEREDOC
}	t_tokentype;

typedef struct s_token
{
	char	*value;
	t_tokentype	type;
	struct s_token	*next;
}	t_token;

typedef struct s_shell
{
	char	**args;
	char	*arg;
	char	**env_list;
	char	*env_value;
	char	*pwd;
	char	*oldpwd;
}	t_shell;

/*parsing */
int parse_input(char *input, char **args, int *quotetype);

/*env variables*/
char	*getenv_builtin(t_shell *shell, char* env_key);
void	copy_envlist(t_shell *shell, char *env[]);
void	update_pwd_env(t_shell *shell);
void	update_old_pwd_env(t_shell *shell);

/* built-in */
char	*get_env_home(char *env[]);
int	echo_builtin(char **args);
void	pwd_builtin();
int	env_builtin(char *arg, t_shell *shell);
int	cd_builtin(char *arg, char *path, t_shell *shell);
int	unset_builtin(t_shell *shell, char **arg);
int	export_builtin(t_shell *shell, char *arg, char *var);

/* Lexer */
t_token *new_token(char *value, t_tokentype type);
void    add_token_back(t_token **list, t_token *new_node);
t_token *lexer(char *input);
void    free_tokens(t_token *tokens);


/* debug helpers */
void	print_pwds(char **shell_env);
#endif
