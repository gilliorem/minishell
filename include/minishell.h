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

/* this is the linked list that is being given to me before the execution, the one 
 * that I need to parse it takes, the name of the command, its list of arguments and a pointer to the next node.*/

typedef struct	t_commands
{
	char	*name;
	char	**args;
	struct s_commands	*next;
}	t_commands;

typedef struct s_redir 
{ 
	char *filename;
       	t_tokentype type; 
	struct s_redir *next; 
} 	t_redir;

typedef struct s_cmd 
{ 
	char **ptr; 
	t_redir *redirs; 
	struct s_cmd *next; 
} 	t_cmd;


t_shell	*init_shell();

/*parsing */
int parse_input(char *input, char **args, int *quotetype);
t_cmd *parser(t_token *token);

/*env variables*/
char	**get_executable_path(t_shell *shell);
char	*getenv_builtin(t_shell *shell, char* env_key);
void	copy_envlist(t_shell *shell, char *env[]);
void	update_pwd_env(t_shell *shell);
void	update_old_pwd_env(t_shell *shell);

char	*ft_strcpy_mohid(const char *src, int len);
/* built-in */
char	*get_env_home(char *env[]);
int	echo_builtin(char **args);
int	pwd_builtin();
int	env_builtin(char *arg, t_shell *shell);
int	cd_builtin(char *arg, char *path, t_shell *shell);
int	unset_builtin(t_shell *shell, char **arg);
int	export_builtin(t_shell *shell, char **arg);
char	*exp_env_var(t_shell *shell, const char *input);
int	exit_builtin();

/* Lexer */
t_token *new_token(char *value, t_tokentype type);
void    add_token_back(t_token **list, t_token *new_node);
t_token *lexer(char *input);
void    free_tokens(t_token *tokens);


/* Executor */

char	**get_executable_path(t_shell *shell);
int	execute_binary(t_shell *shell, t_cmd *cmd);
int	execute_simple_command(t_shell *shell, t_cmd *cmd);
int	set_redirect(t_redir *redir);
int	execute_pipe(t_shell *shell, t_cmd *cmd);

int	execute_builtin(char **cmd, t_shell *shell);
int	execute(t_shell *shell, t_cmd *cmd);

/* Utils */
int count_words(t_token *tok);
void remove_quotes(t_cmd *cmd);

/* debug helpers */
void	print_pwds(char **shell_env);
#endif
