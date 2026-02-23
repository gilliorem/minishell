/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: regillio <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/21 00:59:09 by regillio          #+#    #+#             */
/*   Updated: 2025/12/21 00:59:11 by regillio         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H
# include "../libft/libft.h"
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
	struct s_token	*next;
	t_tokentype		type;
	char			*value;
}	t_token;

typedef struct s_redir
{
	struct s_redir	*next;
	t_tokentype		type;
	char			*filename;
	int				heredoc_fd;
	int				heredoc_quoted;
}	t_redir;

typedef struct s_cmd
{
	struct s_cmd	*next;
	t_redir			*redirections;
	char			**argv;
}	t_cmd;

typedef struct s_env
{
	struct s_env	*next;
	char			*key;
	char			*value;
}	t_env;

/*signal*/
void	handle_sigint(int sig);
int		init_signal(int signal, void (*f)(int s));
void	set_heredoc_signals(void);
void	restore_signals(void);
void	handle_sigquit(int sig);
void	handle_sigint_heredoc(int sig);

/*utils*/
int		get_len(int n);
int		is_number(char *s);
int		ft_isalpha_underscore(int c);
int		ft_isdigit(int c);
char	*get_env_val_wrapper(char *key, t_env *env);

/*free*/
void	free_split(char **arr);
void	free_env_list(t_env *env);
void	free_envp(char **envp);
void	free_tokens(t_token *t);
void	free_redirs(t_redir *r);

/*free 2*/
void	free_cmd_list(t_cmd *c);
void	clean_on_exit(t_cmd *cmds, t_env *env_list);

/*init 1*/
t_env	*new_env_node(char *key, char *value);
void	increment_shelvl(t_env **envp);
int		check_envp(char **envp);
t_env	*env_node_from_str(char *env_line);
void	env_add_back(t_env **head, t_env **tail, t_env *new_node);

/*init 2*/
char	*get_env_val(t_env *env, char *key);
char	**env_list_to_tab(t_env *env);
t_env	*populate_empty_envlist(void);
void	ft_env(t_env *env);
int		update_env(t_env *env, char *key, char *new_value);

/*init 3*/
t_env	*init_environment(char **envp);
t_env	*init_shell_env(char **envp);

/*lexer*/
t_token	*new_token(char *value, t_tokentype type);
void	add_token_back(t_token **list, t_token *new);
int		lex_redirpipe(char *input, int i, t_token **head);
int		lex_word(char *input, int i, t_token **head);
t_token	*lexer(char *input);

/*parser 1*/
int		handle_parser_redir(t_cmd *cur, t_token **tok);
int		process_parser_token(t_cmd **cur, t_token **tok, int *i);
t_cmd	*create_initial_cmd(t_token *tok);
int		check_tokentype(t_token *tok);
t_cmd	*parser(t_token *tok);

/*parser 2*/
t_cmd	*new_cmd(void);
int		count_words_argv(t_token *t);
void	add_redir_back(t_redir **list, t_redir *new);
int		init_cmd_argv(t_cmd *cmd, t_token *tok);
int		start_pipeline_cmd(t_cmd **cur, t_token *tok);

/*heredocs 1*/
char	*readline_heredocs(void);
int		handle_pipes_on_heredocs(int fd[2]);
void	write_to_stdout(int fd[2], char *line);
char	*read_heredocs(void);
int		run_heredocs(char *delimiter, int quoted, t_env *env);

/*heredocs 2*/
void	handle_heredocs(t_cmd *cmd_list, t_env *env);
void	close_unused_heredoc_fds(t_cmd *cmd_list, t_cmd *current_cmd);
void	cleanup_heredocs(t_cmd *cmd_list);

/*expander str 1*/
char	*expand_str(char *s, t_env *env);
void	append_str(char **res, const char *suffix);
void	append_char(char **res, char c);
char	*extract_key(char *str, int *i);
void	append_env_value(char **res, char *key, t_env *env);

/*expander str 2*/
int		handle_dollar(char *str, int *i, char **res, t_env *env);
int		handle_quotes(char c, bool *in_sq, bool *in_dq, int *quote_found);
char	*process_token(char *str, t_env *env, int *quote_found);
char	*process_heredoc_delim(char *str, int *quote_found);
void	expander(t_cmd *cmd, t_env *env);

/*expander str 3*/
void	expand_argv(t_cmd *cmd, t_env *env);

/*expander heredocs*/
char	*expand_heredoc_line(char *str, t_env *env);
void	append_literal_segment(char *s, int *i, char **res);
char	*extract_expand_key(char *s, int *i);
void	append_plain_char(char **res, char c);
int		handle_expand_dollar(char *s, int *i, char **res, t_env *env);

/*executor 1*/
char	*absolute_cmd(char *cmd);
char	**get_directories(t_env *env);
char	*get_command_path(char *cmd, t_env *env);
int		on_fork_error(pid_t last_pid);
void	close_pipes(int pipe_fd[2]);

/*executor 2*/
void	setup_childs_fds(t_cmd *cmd_list, int prev_fd);
void	check_first_cmd(t_cmd *cmd, t_env *env);
void	execute_parent_builtin(t_cmd *cmd, t_env *env);
int		execute_children_builtin(t_cmd *cmd, t_env *env);
void	exit_on_command_not_found(t_cmd *cmd, t_env *env);

/*executor 3*/
void	execute_child(t_cmd *head, t_cmd *cmd, t_env *env);
void	wait_for_child(pid_t last_pid);
int		get_redir_fd(t_redir *r);
void	apply_redirection(t_cmd *cmd, t_env *env);
void	process_child(t_cmd *head, t_cmd *cmd, t_env *env);

/*executor 4*/
void	cleanup_parent(t_cmd **cmd, int pipe_fd[2], int *prev_fd);
pid_t	process_cmd(t_cmd *cmd_list, t_env *env, int prev_fd);
int		is_parent_builtin(t_cmd *cmd, t_env *env);
void	executor(t_cmd *cmd_list, t_env *env);

/*cd*/
void	update_pwd(t_env *env, char *pwd);
int		cd_argument(t_cmd *cmd);
char	*cd_home(t_env *env);
char	*cd_oldpwd(t_env *env);
int		ft_cd(t_cmd *cmd, t_env *env);

/*echo*/
int		is_new_line(char *arg);
int		ft_echo(t_cmd *cmd);

/*exit*/
void	ft_exit(t_cmd *cmd, t_env *env);

/*export 1*/
void	print_env_list(t_env **env_head, t_cmd *cmd);
int		check_first_char(char c);
int		check_key(char *key);
t_env	*add_envback(t_env **env_head, t_env *new);
int		export_key_as_var(char *keyvar, t_env **env_head);

/*export 2*/
int		export_value(char *value, char *key, t_env **env_head);
int		ft_export(t_cmd *cmd, t_env **env_head);

/*unset*/
void	delete_node(char *argv, t_env **env_head);
int		ft_unset(t_cmd *cmd, t_env **env_head);

/*pwd*/
void	ft_pwd(void);

/*main*/
char	*get_input(t_env *env);
t_cmd	*make_cmds(char *input);
int		on_execution(t_cmd *cmds, t_env *env);

#endif
