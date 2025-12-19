#ifndef MINISHELL_H
# define MINISHELL_H
# include "libft/libft.h"
# include <errno.h>
# include <stdbool.h>
# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <string.h>
# include <readline/readline.h>
# include <readline/history.h>
# include <ctype.h>
# include <sys/types.h>
# include <sys/wait.h>
# include <fcntl.h>
# include <signal.h>

extern int	g_exit_status;

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
	char		*value;
	t_tokentype	type;
	struct s_token	*next;
}	t_token;

typedef struct s_redir
{
	char		*filename;
	int		heredoc_fd;
	int		heredoc_quoted;
	t_tokentype	type;
	struct s_redir	*next;
}	t_redir;

typedef struct s_cmd
{
	char		**argv;
	t_redir		*redirections;
	struct s_cmd	*next;
}	t_cmd;

typedef struct s_env
{
	char		*key;
	char		*value;
	struct s_env	*next;
}	t_env;

#endif
