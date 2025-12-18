/* MEMORY ISSUES */
/* In case of cmd error such as: `cd | ls` quotes,
 * parser, new_cmd, init_environment  */
/*Eric tests
  >>>readline suppression (valgrind file;
  not take into account readline leaks) [X]
  EXIT STATUS
 *Thoughts: Need a global variable that keeps track of our exit code...
 and we try to get read of g_inchild*
 $? ON CONTROL C []
 -$?
 ctrl c
 echo $?
 exit text [X]
 exit 42 [X]
 exit -1 [X]
 exit 1 1 [X]
 exit 256 [X]
 exit 5678 [X]
 exit -09876543 [X]
 exit 765445678976543456787654345675467876564
 sleep 10 | ls
 change the printf with write_fd (libft)
 - BUILTINS (Remi)
 export test`1` [X] // should work catch integer. only first char
 SHLVL ++ as we open new instances of anyshell [X]
 - edge case for echo
 echo hi > /dev/full [X]
 should prompt: `bash: echo: write error: no space left on device`
 - REDIR combining redirections 
 double << <<
 combining << << and >
 2+ redir in
 < file < file2 cat // should read from file 2 only
 > new cat << hi << hello
 needs to exit typing
 `hi`
 `hello`
 echo hello > f1 > f2  // shoud have hello in f2 
last one should get the input, the other will be blank
 << and quotes: cat << "HI" : same rules apply here and echo 
// should exit HI
 - EXECUTION mixing builtins and binaries (Mohid)
 export | head
 ls | cd .. // should run but not change dir because 
	    // executing in the child process
 cd | ls
 ls |      // make sure the pipe fd is closed
 */
// Failed tests
// cd OLDPWD state [X] - Remi
// export with no args [x] - Remi
// $? (mohid) [X]
// builtins return value (1 on failure,
// 0 on success to match with $) [X]
// cd can take only 0 or 1 argument. [X]
// update the $PWD [X] - Remi
// update the $OLDPWD [X] Remi
// cd ~ [X]
// cd - [X]
// unset a b [X] 
// export a=1 b=2 [X]
// export identifier [X]
//Crazy heredoc tests:
// cat <<HERE <<DOC
// cat <<HERE (Inside $USER should be expanded)
// cat <minishell.c <<HERE | cat
// cat << $
/* ** COMPLETE MINISHELL.C 
** Fixed dependency order and Parser logic
*/

/*
#include "libft/libft.h"
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <signal.h>
*/

#include "minishell.h"

/*
typedef enum e_tokentype {
    TOKEN_WORD,
    TOKEN_PIPE,
    TOKEN_REDIR_IN,
    TOKEN_REDIR_OUT,
    TOKEN_REDIR_APPEND,
    TOKEN_HEREDOC 
} t_tokentype;

typedef struct s_token
{ 
    char		*value; 
    t_tokentype		type; 
    struct s_token	*next; 
} 			t_token;

typedef struct s_redir 
{ 
    struct	s_redir *next; 
    t_tokentype	type; 
    char	*filename; 
    int		heredoc_fd; 
    int		heredoc_quoted;
} 		t_redir;

typedef struct s_cmd
{ 
    struct s_cmd	*next; 
    t_redir		*redirections; 
    char		**argv; 
}			 t_cmd;

typedef struct s_env
{ 
    struct s_env	*next; 
    char		*key; 
    char		*value; 
}			t_env;

int g_exit_status = 0;
*/
int	g_exit_status;

void	handle_sigint(int sig)
{
	extern unsigned long	rl_readline_state;

	g_exit_status = 128 + sig;
	write(1, "\n", 1);
	rl_on_new_line();
	rl_replace_line("", 0);
	if (rl_readline_state & RL_STATE_READCMD)
		rl_redisplay();
}

int	init_signal(int signal, void(*f)(int s))
{
	struct sigaction	sa;

	sa.sa_handler = f;
	sa.sa_flags = SA_RESTART;
	sigemptyset(&sa.sa_mask);
	if (sigaction(signal, &sa, NULL) == -1)
		return (1);
	return (0);
}
void	handle_sigint_heredoc(int sig);
void	set_heredoc_signals(void)
{
	struct sigaction	sa;

	sa.sa_handler = handle_sigint_heredoc;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sigaction(SIGINT, &sa, NULL);
	signal(SIGQUIT, SIG_IGN);
}

void	restore_signals(void)
{
	signal(SIGINT, handle_sigint);
	signal(SIGQUIT, SIG_IGN);
}

void	handle_sigquit(int sig)
{
	(void) sig;
	g_exit_status = 128 + sig;
	if (init_signal(SIGQUIT, SIG_IGN))
		return ;
	kill(0, SIGQUIT);
	printf("Quit (core dumped)\n");
}

void	handle_sigint_heredoc(int sig)
{
	(void)sig;
	g_exit_status = -130;
	write(1, "\n", 1);
}

// --- LIBFT-LIKE FUNCTIONS --- //
/*
size_t ft_strlen(const char *s) 
{ 
    size_t c=0; 
    while (s[c]) c++; 
    return c; 
}

char *ft_strdup(const char *s1) 
{ 
    char *dst = malloc(strlen(s1) + 1); 
    if (!dst) 
        return NULL; 
    strcpy(dst, s1); 
    return dst; 
}

char *ft_substr(char const *s, unsigned int start, size_t len) 
{
    if (!s) return NULL; 
    if (start >= strlen(s)) 
        return ft_strdup("");
    if (len > strlen(s + start)) 
        len = strlen(s + start);
    char *sub = malloc(sizeof(char) * (len + 1)); 
    if (!sub) return NULL;
    strncpy(sub, s + start, len); sub[len] = '\0'; return sub;
}

char *ft_strjoin(const char *s1, const char *s2) {
    if (!s1 || !s2) return NULL;
    char *ptr = malloc(ft_strlen(s1) + ft_strlen(s2) + 1);
    if (!ptr) return NULL;
    strcpy(ptr, s1); strcat(ptr, s2); return ptr;
}

static int word_countandreplace(char *s, char delimiter) {
    bool new_word=false; char *temp=s; int count=0;
    while (*temp) { 
        if (*temp!=delimiter && !new_word) { new_word=true; count++; } 
        else if (*temp==delimiter) { *temp='\0'; new_word=false; } 
        temp++; 
    }
    return count;
}

static bool fill_ptr(char **ptr, char *s1, int wc) {
    int i=0; 
    while (i<wc) { 
        if (*s1) { 
            ptr[i++]=ft_strdup(s1); 
            if (!ptr[i-1]) return false; 
            while (*s1) s1++; 
        } 
        s1++; 
    } 
    ptr[i]=NULL; 
    return true;
}

char **ft_split(char const *s, char c) {
    if (!s) return NULL; 
    char *s1=ft_strdup(s); 
    if (!s1) return NULL;
    int wc=word_countandreplace(s1, c); 
    char **ptr=malloc(sizeof(char*)*(wc+1));
    if (!ptr || !fill_ptr(ptr, s1, wc)) { free(s1); return NULL; } 
    free(s1); 
    return ptr;
}
char *ft_itoa(int n) {
    int len = get_len(n);
    char *str = malloc(len + 1);
    if (!str) return NULL;
    str[len] = '\0';
    if (n == 0) str[0] = '0';
    if (n < 0) { str[0] = '-'; n = -n; } 
    while (n) { str[--len] = (n % 10) + '0'; n /= 10; }
    return str;
}
*/

void	free_split(char **arr)
{
	int	i;

	i = 0;
	if (!arr)
		return ;
	while (arr[i])
		free(arr[i++]);
	free(arr);
}

static int	get_len(int n)
{
	int	len;

	len = 0;
	if (n <= 0)
		len++;
	while (n)
	{
		n /= 10;
		len++;
	}
	return (len);
}

int	is_number(char *s)
{
	int	i;

	i = 0;
	while (s[i])
	{
		if (i == 0 && (s[0] == '-' || s[0] == '+'))
			i++;
		else if (s[i] < '0' || s[i] > '9')
			return (0);
		i++;
	}
	return (1);
}

int	ft_isalpha_underscore(int c)
{
	if ((c >= 'a' && c <= 'z')
		|| (c >= 'A' && c <= 'Z') || (c == '_'))
		return (1);
	return (0);
}

int	ft_isdigit(int c)
{
	if (c >= '0' && c <= '9')
		return (1);
    return (0);
}
/* END OF LIBFT BLOCK */

t_env	*new_env_node(char *key, char *value)
{
	t_env	*node;

	node = malloc(sizeof(t_env));
	node->key = key;
	node->value = value;
	node->next = NULL;
	return (node);
}

void	free_env_list(t_env *env) 
{
	t_env	*tmp;
	while (env)
	{
		tmp = env->next;
		free(env->key);
		free(env->value);
		free(env);
		env = tmp;
	}
}

void    increment_shelvl(t_env **envp)
{
	int	shlvl;

	shlvl = 0;
	t_env *node;

	node = *envp;
	while (node->next != NULL)
	{
		if (strcmp(node->key, "SHLVL") == 0)
		{
			shlvl = atoi(node->value) + 1;
			free(node->value);
			node->value = ft_itoa(shlvl);
		}
		node = node->next;
	}
}

t_env	*init_environment(char **envp)
{
	int	i;
	char	*key;
	char	*value;
	char	*eq_pos;
	t_env	*head;
	t_env	*tail;

	head = NULL;
	tail = NULL;
	i = 0;
	if (!envp)
	{
		printf("empty env list\n");
		return (NULL);
	}
	while (envp[i])
	{
		eq_pos = strchr(envp[i], '=');
		if (eq_pos)
		{
			key = ft_substr(envp[i], 0, eq_pos - envp[i]);
			value = ft_strdup(eq_pos + 1);
			t_env *new_node = new_env_node(key, value);
			if (!head)
			{
				head = new_node;
				tail = head;
			}
			else
			{
				tail->next = new_node;
				tail = new_node;
			}
		}
		i++;
	}
	return (head);
}

char	*get_env_val(t_env *env, char *key)
{
	while (env)
	{
		if (strcmp(env->key, key) == 0)
			return (ft_strdup(env->value));
		env = env->next;
	}
	return (NULL);
}

char	**env_list_to_tab(t_env *env)
{
	char	**tab;
	char	*temp;
	int	count;
	int	i;
	t_env	*tmp;

	count = 0;
	tmp = env;
	while (tmp)
	{
		count++;
		tmp = tmp->next;
	}
	tab = malloc(sizeof(char *) * (count + 1));
	i = 0;
	tmp = env;
	while (tmp)
	{
		temp = ft_strjoin(tmp->key, "=");
		tab[i++] = ft_strjoin(temp, tmp->value);
		free(temp);
		tmp = tmp->next;
	}
	tab[i] = NULL;
	return (tab);
}

t_env	*populate_empty_envlist()
{
	char	cwd[1024];
	char	*key;
	char	*value;
	t_env	*node;

	key = ft_strdup("PWD");
	value = ft_strdup(getcwd(cwd, sizeof(cwd)));
	node = new_env_node(key, value);
	return (node);
}

void	ft_env(t_env *env)
{
	while (env)
	{
		if (strcmp(env->value, "") != 0)
			printf("%s=%s\n", env->key, env->value);
		env = env->next;
	}
}

int	update_env(t_env *env, char *key, char *new_value)
{
	while (env)
	{
		if (strcmp(env->key, key) == 0)
		{
			free(env->value);
			env->value = ft_strdup(new_value);
			return (1);
		}
		env = env->next;
	}
	return (0);
}

t_token	*new_token(char *value, t_tokentype type)
{
	t_token *token;

	token = malloc(sizeof(t_token));
	token->value = value;
	token->type = type;
	token->next = NULL;
	return (token);
}

void	add_token_back(t_token **list, t_token *new)
{
	t_token	*c;

	if (!*list)
		*list = new;
	else
	{
		c = *list;
		while (c->next)
			c = c->next;
		c->next = new;
	}
}

static int	lex_redirpipe(char *input, int i, t_token **head)
{
	if (input[i] == '|')
	{
		add_token_back(head, new_token(ft_strdup("|"), TOKEN_PIPE));
		return (i + 1);
	}
	if (input[i] == '>')
	{
		if (input[i + 1] == '>')
		{
			add_token_back(head, new_token(ft_strdup(">>"),
						TOKEN_REDIR_APPEND));
			return (i + 2);
		}
		add_token_back(head, new_token(ft_strdup(">"), TOKEN_REDIR_OUT));
		return (i + 1);
	}
	if (input[i] == '<' && input[i + 1] == '<')
	{
		add_token_back(head, new_token(ft_strdup("<<"), TOKEN_HEREDOC));
		return (i + 2);
	}
	add_token_back(head, new_token(ft_strdup("<"), TOKEN_REDIR_IN));
	return (i + 1);
}

static int	lex_word(char *input, int i, t_token **head)
{
	int	start;
	char	q;

	start = i;
	while (input[i] && !isspace(input[i]) && !strchr("|<>", input[i]))
	{
		if (input[i] == '\'' || input[i] == '"')
		{
			q = input[i++];
			while (input[i] && input[i] != q)
				i++;
			if (input[i])
				i++;
		}
		else
			i++;
	}
	add_token_back(head, new_token(ft_substr(input, start, i - start), TOKEN_WORD));
	return (i);
}

t_token	*lexer(char *input)
{
	t_token	*head;
	int		i;

	head = NULL;
	i = 0;
	while (input[i])
	{
		if (ft_isspace(input[i]))
		{
			i++;
			continue ;
		}
		if (strchr("|<>", input[i]))
		{
			i = lex_redirpipe(input, i, &head);
			continue ;
		}
		i = lex_word(input, i, &head);
	}
	return (head);
}

void	free_tokens(t_token *t)
{
	t_token	*tmp;

	while (t)
	{
		tmp = t->next;
		free(t->value);
		free(t);
		t = tmp;
	}
}

t_cmd	*new_cmd(void)
{
	t_cmd	*c;

	c = malloc(sizeof(t_cmd));
	c->argv = NULL;
	c->redirections = NULL;
	c->next = NULL;
	return (c);
}

int	count_words_argv(t_token *t)
{
	int	c;

	c = 0;
	while (t && t->type != TOKEN_PIPE)
	{
		if (t->type == TOKEN_WORD)
			c++;
		else if (t->type >= TOKEN_REDIR_IN)
		{
			if (t->next)
				t = t->next;
		}
		t = t->next;
	}
	return (c);
}

void	add_redir_back(t_redir **list, t_redir *new)
{
	t_redir	*c;

	if (!*list)
	{
		*list = new;
		new->next = NULL;
	}
	else
	{
		c = *list;
		while (c->next)
			c = c->next;
		c->next = new;
		new->next = NULL;
	}
}

void	free_redirs(t_redir *r)
{
	t_redir	*tmp;

	while (r)
	{
		tmp = r->next;
		free(r->filename);
		free(r);
		r = tmp;
	}
}

void	free_cmd_list(t_cmd *c)
{
	int	i;
	t_cmd	*tmp;

	while (c)
	{
		tmp = c->next;
		i = 0;
		if (c->argv)
		{
			while (c->argv[i])
			{
				if (c->argv[i])
					free(c->argv[i++]);
			}
			free(c->argv);
		}
		if (c->redirections)
			free_redirs(c->redirections);
		free(c);
		c = tmp;
	}
}

static int	init_cmd_argv(t_cmd *cmd, t_token *tok)
{
	int	wc;

	wc = count_words_argv(tok);
	cmd->argv = malloc(sizeof(char *) * (wc + 1));
	if (!cmd->argv)
		return (0);
	cmd->argv[0] = NULL;
	return (1);
}

static int	start_pipeline_cmd(t_cmd **cur, t_token *tok)
{
	t_cmd	*next;

	next = new_cmd();
	if (!next)
		return (0);
	if (!init_cmd_argv(next, tok))
	{
		free(next);
		return (0);
	}
	(*cur)->next = next;
	*cur = next;
	return (1);
}

static int	handle_parser_redir(t_cmd *cur, t_token **tok)
{
	t_redir	*r;

	r = malloc(sizeof(t_redir));
	if (!r)
		return (0);
	r->type = (*tok)->type;
	r->heredoc_fd = -1;
	r->heredoc_quoted = 0;
	r->next = NULL;
	*tok = (*tok)->next;
	if (!*tok || (*tok)->type != TOKEN_WORD)
	{
		printf("minishell: syntax error near unexpected token\n");
		free(r);
		return (-1);
	}
	r->filename = ft_strdup((*tok)->value);
	add_redir_back(&cur->redirections, r);
	*tok = (*tok)->next;
	return (1);
}

static int	process_parser_token(t_cmd **cur, t_token **tok, int *i)
{
	int	status;

	if ((*tok)->type == TOKEN_PIPE)
	{
		(*cur)->argv[*i] = NULL;
		*tok = (*tok)->next;
		if (!start_pipeline_cmd(cur, *tok))
			return (0);
		*i = 0;
		return (1);
	}
	if ((*tok)->type >= TOKEN_REDIR_IN && (*tok)->type <= TOKEN_HEREDOC)
	{
		status = handle_parser_redir(*cur, tok);
		return (status > 0);
	}
	(*cur)->argv[(*i)++] = ft_strdup((*tok)->value);
	*tok = (*tok)->next;
	return (1);
}

static t_cmd	*create_initial_cmd(t_token *tok)
{
	t_cmd	*cmd;

	cmd = new_cmd();
	if (!cmd)
		return (NULL);
	if (!init_cmd_argv(cmd, tok))
	{
		free(cmd);
		return (NULL);
	}
	return (cmd);
}

t_cmd	*parser(t_token *tok)
{
	t_cmd	*head;
	t_cmd	*cur;
	int		i;

	if (!tok)
		return (NULL);
	head = create_initial_cmd(tok);
	if (!head)
		return (NULL);
	cur = head;
	i = 0;
	while (tok)
	{
		if (!process_parser_token(&cur, &tok, &i))
			return (free_cmd_list(head), NULL);
	}
	cur->argv[i] = NULL;
	return (head);
}

char	*get_env_val_wrapper(char *key, t_env *env)
{
	char	*v;

	v = get_env_val(env, key);
	if (!v)
		return (ft_strdup(""));
	return (v);
}

static void append_str(char **res, const char *suffix);
static void append_char(char **res, char c);
static char *extract_key(char *str, int *i);
static void append_env_value(char **res, char *key, t_env *env);
static int handle_dollar(char *str, int *i, char **res, t_env *env);

char	*expand_heredoc_line(char *str, t_env *env)
{
	char	*res;
	int	i;

	res = ft_strdup("");
	i = 0;
	while (str[i])
	{
		if (handle_dollar(str, &i, &res, env))
			continue ;
		append_char(&res, str[i++]);
	}
	free(str);
	return (res);
}

static void	append_literal_segment(char *s, int *i, char **res)
{
	int	start;
	char	*sub;

	start = ++(*i);
	while (s[*i] && s[*i] != '\'')
		(*i)++;
	sub = ft_substr(s, start, *i - start);
	append_str(res, sub);
	free(sub);
	if (s[*i] == '\'')
		(*i)++;
}

static char	*extract_expand_key(char *s, int *i)
{
	int	start;

	if (s[*i] == '?')
	{
		(*i)++;
		return (ft_strdup("?"));
	}
	start = *i;
	while (s[*i] && (isalnum(s[*i]) || s[*i] == '_'))
		(*i)++;
	return (ft_substr(s, start, *i - start));
}

static void	append_plain_char(char **res, char c)
{
	char	buf[2];

	buf[0] = c;
	buf[1] = '\0';
	append_str(res, buf);
}

static int	handle_expand_dollar(char *s, int *i, char **res, t_env *env)
{
	char	*key;
	char	*val;

	if (s[*i] != '$')
		return (0);
	if (!(s[*i + 1] == '?' || isalpha(s[*i + 1]) || s[*i + 1] == '_'))
		return (0);
	(*i)++;
	key = extract_expand_key(s, i);
	if (strcmp(key, "?") == 0)
		val = ft_itoa(g_exit_status);
	else
		val = get_env_val_wrapper(key, env);
	append_str(res, val);
	free(key);
	free(val);
	return (1);
}
/* END OF EXPAND HEREDOCS */

char	*expand_str(char *s, t_env *env)
{
	char	*res;
	int	i;

	res = ft_strdup("");
	i = 0;
	while (s[i])
	{
		if (s[i] == '\'')
			append_literal_segment(s, &i, &res);
		else if (s[i] == '$' &&
			handle_expand_dollar(s, &i, &res, env))
			continue ;
		else
			append_plain_char(&res, s[i++]);
	}
	free(s);
	return (res);
}

static void	append_str(char **res, const char *suffix)
{
	char	*tmp;

	tmp = ft_strjoin(*res, suffix);
	free(*res);
	*res = tmp;
}

static void	append_char(char **res, char c)
{
	char	buff[2];

	buff[0] = c;
	buff[1] = '\0';
	append_str(res, buff);
}

static char	*extract_key(char *str, int *i)
{
	int	start;

	if (str[*i] == '?')
	{
		(*i)++;
		return (ft_strdup("?"));
	}
	start = *i;
	while (str[*i] && (isalnum(str[*i]) || str[*i] == '_'))
		(*i)++;
	return (ft_substr(str, start, *i - start));
}

static void	append_env_value(char **res, char *key, t_env *env)
{
	char	*val;

	if (strcmp(key, "?") == 0)
		val = ft_itoa(g_exit_status);
	else
		val = get_env_val_wrapper(key, env);
	append_str(res, val);
	free(val);
}

static int	handle_dollar(char *str, int *i, char **res, t_env *env)
{
	char	*key;

	if (str[*i] != '$')
		return (0);
	if (!(str[*i + 1] == '?' || isalpha(str[*i + 1])
	|| str[*i + 1] == '_'))
	{
		append_char(res, str[(*i)++]);
		return (1);
	}
	(*i)++;
	key = extract_key(str, i);
	append_env_value(res, key, env);
	free(key);
	return (1);
}

static int	handle_quotes(char c, bool *in_sq, bool *in_dq, int *quote_found)
{
	if (c == '\'' && !*in_dq)
	{
		*in_sq = !*in_sq;
		if (quote_found)
			*quote_found = 1;
		return (1);
	}
	if (c == '\"' && !*in_sq)
	{
		*in_dq = !*in_dq;
		if (quote_found)
			*quote_found = 1;
		return (1);
	}
	return (0);
}

char	*process_token(char *str, t_env *env, int *quote_found)
{
	char	*res;
	int	i;
	bool	in_sq;
	bool	in_dq;

	res = ft_strdup("");
	i = 0;
	in_sq = false;
	in_dq = false;
	while (str[i])
	{
		if (handle_quotes(str[i], &in_sq, &in_dq, quote_found))
			i++;
		else if (!in_sq && handle_dollar(str, &i, &res, env))
			continue ;
		else
			append_char(&res, str[i++]);
	}
	free(str);
	return (res);
}

void	expander(t_cmd *cmd, t_env *env)
{
	int	quote_found;
	int	i;
	t_redir	*r;

	while (cmd)
	{
		i = 0;
		while (cmd->argv && cmd->argv[i])
		{
			cmd->argv[i] = process_token(cmd->argv[i], env, NULL);
			i++;
		}
		r = cmd->redirections;
		while (r)
		{
			quote_found = 0;
			r->filename = process_token(r->filename, env, &quote_found);
			if (r->type == TOKEN_HEREDOC)
				r->heredoc_quoted = quote_found;
			r = r->next;
		}
		cmd = cmd->next;
	}
}

static char	*readline_heredocs(void)
{
	char	*line;
	size_t	len;
	char	c;
	ssize_t	bytes_read;

	line = malloc(256);
	len = 0;
	while (1)
	{
		bytes_read = read(1, &c, 1);
		if (bytes_read == 0)
			break ;
		if (bytes_read < 0)
			return (free(line), NULL);
		if (c == '\n')
			break ;
		line[len] = c;
		len++;
	}
	if (len == 0 && bytes_read == 0)
		return (free(line), NULL);
	line[len] = '\0';
	return (line);
}

int	handle_pipes_on_heredocs(int fd[2])
{
	if (g_exit_status < 0)
	{
		restore_signals();
		close(fd[0]);
		close(fd[1]);
		return (0);
	}
	return (1);
}

void	write_to_stdout(int fd[2], char *line)
{
	write(fd[1], line, ft_strlen(line));
	write(fd[1], "\n", 1);
}

static char	*read_heredocs(void)
{
	char	*line;

	write(1, "> ", 2);
	line = readline_heredocs();
	return (line);
}

int	run_heredocs(char *delimiter, int quoted, t_env *env)
{
	int	fd[2];
	char	*line;

	if (pipe(fd) == -1)
		return (-1);
	set_heredoc_signals();
	while (1)
	{
		line = readline_heredocs();
		if (!handle_pipes_on_heredocs(fd))
			return (-1);
		if (!line)
			break ;
		if (ft_strcmp(line, delimiter) == 0 && (free(line), 1))
			break ;
		if (!quoted)
			line = expand_heredoc_line(line, env);
		write_to_stdout(fd, line);
		free(line);
	}
	restore_signals();
	close(fd[1]);
	return (fd[0]);
}

void	handle_heredocs(t_cmd *cmd_list, t_env *env)
{
	t_cmd	*cmd;
	t_redir	*r;

	cmd = cmd_list;
	while (cmd)
	{
		r = cmd->redirections;
		while (r)
		{
			if (r->type == TOKEN_HEREDOC)
			{
				r->heredoc_fd =
				run_heredocs(r->filename, r->heredoc_quoted, env);
				if (r->heredoc_fd == -1)
					return ;
			}
			r = r->next;
		}
		cmd = cmd->next;
	}
}

void	close_unused_heredoc_fds(t_cmd *cmd_list, t_cmd *current_cmd)
{
	t_cmd	*tmp;
	t_redir	*r;

	tmp = cmd_list;
	while (tmp)
	{
		if (tmp != current_cmd)
		{
			r = tmp->redirections;
			while (r)
			{
				if (r->type == TOKEN_HEREDOC && r->heredoc_fd != -1)
				{
					close(r->heredoc_fd);
				}
				r = r->next;
			}
		}
		tmp = tmp->next;
	}
}

void	cleanup_heredocs(t_cmd *cmd_list)
{
	t_cmd	*cmd;
	t_redir	*r;

	cmd = cmd_list;
	while (cmd)
	{
		r = cmd->redirections;
		while (r)
		{
			if (r->type == TOKEN_HEREDOC && r->heredoc_fd != -1)
			{
				close(r->heredoc_fd);
				r->heredoc_fd = -1;
			}
			r = r->next;
		}
		cmd = cmd->next;
	}
}

void	ft_pwd(void)
{
	char	cwd[1024];

	if (getcwd(cwd, sizeof(cwd)))
		printf("%s\n", cwd);
	else
		perror("pwd");
}

void	update_pwd(t_env *env, char *pwd)
{
	char	cwd[1024];

	getcwd(cwd, sizeof(cwd));
	update_env(env, pwd, cwd);
}

/* CD BLOCK STARTING HERE */

static int	cd_argument(t_cmd *cmd)
{
	if (cmd->argv[1] && cmd->argv[2])
	{
		ft_putstr_fd("minishell: cd: too many arguments\n", 2);
		return (0);
	}
	return (1);
}

static char	*cd_home(t_cmd *cmd, t_env *env)
{
	char	*home;

	home = get_env_val(env, "HOME");
	if (!home)
	{
		ft_putstr_fd("minishell: cd: HOME not set\n", 2);
		return (NULL);
	}
	return (home);
}

static char	*cd_oldpwd(t_env *env)
{
	char	*oldpwd;

	oldpwd = get_env_val(env, "OLDPWD");
	if (!oldpwd)
	{
		ft_putstr_fd("minishell: cd: OLDPWD not set\n", 2);
		return (NULL);
	}
	printf("%s\n", oldpwd);
	return (oldpwd);
}

int ft_cd(t_cmd *cmd, t_env *env)
{
	char	*target_path;

	if (!cd_argument(cmd))
		return (1);
	if (!cmd->argv[1] || strcmp(cmd->argv[1], "~") == 0)
		target_path = cd_home(cmd, env);
	else if (ft_strcmp(cmd->argv[1], "-") == 0)
		target_path = cd_oldpwd(env);
	else
		target_path = ft_strdup(cmd->argv[1]);
	if (!target_path)
		return (1);
	update_pwd(env, "OLDPWD");
	if (chdir(target_path) != 0)
	{
		perror("minishell: cd");
		free(target_path);
		return (1);
	}
	update_pwd(env, "PWD");
	free(target_path);
	return (0);
}
/* FT_CD ENDING HERE */

void	clean_on_exit(t_cmd *cmds, t_env *env_list)
{
	if (cmds)
	{
		cleanup_heredocs(cmds);
		free_cmd_list(cmds);
	}
	if (env_list)
		free_env_list(env_list);
}

void	ft_exit(t_cmd *cmd, t_env *env)
{
	int	exit_code;

	printf("exit\n");
	if (!cmd->argv[1])
	{
		clean_on_exit(cmd, env);
		exit (g_exit_status);
	}
	if (!is_number(cmd->argv[1]))
	{
		printf("minishell: exit: %s: numeric argument required\n",
		cmd->argv[1]);
		clean_on_exit(cmd, env);
		exit (2);
	}
	if (cmd->argv[2])
	{
		printf("minishell: exit: too many arugments\n");
		return ;
	}
	exit_code = atoi(cmd->argv[1]);
	clean_on_exit(cmd, env);
	exit (exit_code % 256);
}

/* Echo block */
static int	is_new_line(char *arg)
{
	int	i;

	i = 1;
	if (arg[0] != '-' || arg[1] == '\0')
		return (0);
	while (arg[i] == 'n')
		i++;
	if (arg[i] != '\0')
		return (0);
	return (1);
}

int	ft_echo(t_cmd *cmd)
{
	bool	newline;
	int	i;

	newline = true;
	i = 1;
	while (cmd->argv[i])
	{
		if (!is_new_line(cmd->argv[i]))
			break ;
		newline = false;
		i++;
	}
	while (cmd->argv[i])
	{
		ft_putstr_fd(cmd->argv[i], 1);
		if (cmd->argv[i + 1])
			write(1, " ", 1);
		i++;
	}
	if (newline)
		ft_putchar_fd('\n', 1);
	return (0);
}

/* EXPORT FUNCTIONS START HERE */
static void	print_env_list(t_env **env_head, t_cmd *cmd)
{
	t_env	*tmp;

	if (cmd->argv[1])
		return ;
	tmp = *env_head;
	while (tmp)
	{
		if (ft_strcmp(tmp->value, "") == 0)
			printf("declare -x %s=\"\"\n", tmp->key);
		else
			printf("declare -x %s=\"%s\"\n", tmp->key, tmp->value);
		tmp = tmp->next;
	}
}

static int	check_first_char (char c)
{
	if ((c >= 'a' && c <= 'z')
		|| (c >= 'A' && c <= 'Z') || (c == '_'))
		return (1);
	ft_putstr_fd("minishell: export: not a valid identifier:", 2);
	ft_putchar_fd(c, 2);
	ft_putchar('\n');
	return (0);
}

static int	check_key(char *key)
{
	int	i;

	i = 0;
	while (key[i])
	{
		if (ft_isdigit(key[i]))
		{
			i++;
			continue ;
		}
		if (ft_isalpha_underscore(key[i]) == 0)
		{
			printf("minishell: export: '%s': not a valid identifier\n", key);
			return (0);
		}
		i++;
	}
	return (1);
}

t_env	*add_envback(t_env **env_head, t_env *new)
{
	t_env	*tmp;

	if (!*env_head)
		*env_head = new;
	else
	{
		tmp = *env_head;
		while (tmp->next)
			tmp = tmp->next;
		tmp->next = new;
	}
	return (*env_head);
}

static int	export_key_as_var(char *keyvar, t_env **env_head)
{
	int	i;
	t_env	*no_value_node;

	i = 0;
	if (!check_key(keyvar))
		return (0);
	no_value_node = new_env_node(keyvar, ft_strdup(""));
	add_envback(env_head, no_value_node);
	return (1);
}

static int	export_value(char *value, char *key, t_env **env_head)
{
	t_env	*new_node;

	if (!check_key(key))
		return (0);
	if (!update_env(*env_head, key, value))
	{
		new_node = new_env_node(key, ft_strdup(value));
		add_envback(env_head, new_node);
	}
	return (1);
}

int	ft_export(t_cmd *cmd, t_env **env_head)
{
	char	*eq_pos;
	char	*key;
	char	*value;
	int	i;

	i = 1;
	print_env_list(env_head, cmd);
	while (cmd->argv[i])
	{
		if (!check_first_char(cmd->argv[i][0]))
			return (1);
		eq_pos = strchr(cmd->argv[i], '=');
		if (!eq_pos && !export_key_as_var(ft_strdup(cmd->argv[i]), env_head))
			return (1);
		if (eq_pos)
		{
			key = ft_substr(cmd->argv[i], 0, eq_pos - cmd->argv[i]);
			if (!check_key(key))
				return (1);
			value = eq_pos + 1;
			export_value(value, key, env_head);
		}
		i++;
	}
	return (0);
}

/* END OF EXPORT FUNCTIONS */

void	delete_node(char *argv, t_env **env_head)
{
	t_env	*curr;
	t_env	*prev;

	curr = *env_head;
	prev = NULL;
	while (curr)
	{
		if (strcmp(curr->key, argv) == 0)
		{
			if (prev)
				prev->next = curr->next;
			else 
				*env_head = curr->next;
			free(curr->key);
			free(curr->value);
			free(curr);
			break ;
		}
		prev = curr;
		curr = curr->next;
	}
}

int	ft_unset(t_cmd *cmd, t_env **env_head)
{
	int	i;

	i = 1;
	if (!cmd->argv[1])
		return (0);
	while (cmd->argv[i])
	{
		delete_node(cmd->argv[i], env_head);
		i++;
	}
	return (0);
}

char	*absolute_cmd(char *cmd)
{
	if (ft_strchr(cmd, '/'))
	{
		if (access(cmd, X_OK) == 0)
			return (ft_strdup(cmd));
	}
	return (NULL);
}

/* EXEC BLOCK STARTS HERE */

static char	**get_directories(t_env *env)
{
	char	*path_env;
	char	**directories;

	path_env = get_env_val(env, "PATH");
	if (!path_env)
		return (NULL);
	directories = ft_split(path_env, ':');
	return (directories);
}

static char	*get_command_path(char *cmd, t_env *env)
{
	char	*command_path;
	char	**directories;
	char	*temp;
	int	i;

	command_path = absolute_cmd(cmd);
	if (command_path)
		return (command_path);
	directories = get_directories(env);
	i = 0;
	while (directories && directories[i])
	{
		temp = ft_strjoin(directories[i], "/");
		command_path = ft_strjoin(temp, cmd);
		free(temp);
		if (access(command_path, X_OK) == 0)
		{
			free_split(directories);
			return (command_path);
		}
		free(command_path);
		i++;
	}
	free_split(directories);
	return (NULL);
}

static void	execute_parent_builtin(t_cmd *cmd, t_env *env)
{
	if (cmd && !cmd->next && cmd->argv && cmd->argv[0])
	{
		if (ft_strcmp(cmd->argv[0], "exit") == 0)
			ft_exit(cmd, env);
		else if (ft_strcmp(cmd->argv[0], "export") == 0 && cmd->argv[1])
			g_exit_status = ft_export(cmd, &env);
		else if (ft_strcmp(cmd->argv[0], "unset") == 0)
			g_exit_status = ft_unset(cmd, &env);
		else if (ft_strcmp(cmd->argv[0], "cd") == 0)
			g_exit_status = ft_cd(cmd, env);
	}
}

static int	on_fork_error(pid_t last_pid)
{
	if (last_pid == -1)
	{
		perror("fork");
		return (0);
	}
	return (1);
}

static void	close_pipes(int pipe_fd[2])
{
	close(pipe_fd[0]);
	dup2(pipe_fd[1], 1);
	close(pipe_fd[1]);
}

static void	setup_childs_fds(pid_t last_pid, t_cmd *cmd_list,
		int pipe_fd[2], int prev_fd)
{
	t_cmd	*cmd;

	cmd = cmd_list;
	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);
	close_unused_heredoc_fds(cmd_list, cmd);
	if (prev_fd != -1)
	{
		dup2(prev_fd, 0);
		close(prev_fd);
	}
}

static void	check_first_cmd(t_cmd *cmd, t_env *env)
{
	if (!cmd->argv[0])
	{
		clean_on_exit(cmd, env);
		exit(0);
	}
}

static void	on_neg_ret(int ret, t_cmd *cmd, t_env *env)
{
	g_exit_status = ret;
	clean_on_exit(cmd, env);
	exit(g_exit_status);
}

static int	execute_children_builtin(t_cmd *cmd, t_env *env)
{
	int	ret;

	ret = -1;
	if (strcmp(cmd->argv[0], "env") == 0)
	{
		ret = 0;
		ft_env(env);
	}
	else if (strcmp(cmd->argv[0], "pwd") == 0)
	{
		ret = 0;
		ft_pwd();
	}
	else if (strcmp(cmd->argv[0], "echo") == 0)
		ret = ft_echo(cmd);
	else if (strcmp(cmd->argv[0], "cd") == 0)
		ret = ft_cd(cmd, env);
	else if (strcmp(cmd->argv[0], "export") == 0)
		ret = ft_export(cmd, &env);
	else if (strcmp(cmd->argv[0], "unset") == 0)
		ret = ft_unset(cmd, &env);
	return (ret);
}

static void	exit_on_command_not_found(t_cmd *cmd, t_env *env)
{
	write(2, "minishell: command not found: ", 30);
	write(2, cmd->argv[0], strlen(cmd->argv[0]));
	write(2, "\n", 1);
	clean_on_exit(cmd, env);
	exit(127);
}

static void	execute_child(t_cmd *cmd, t_env *env)
{
	char	*path;
	char	**env_arr;

	path = get_command_path(cmd->argv[0], env);
	if (!path)
		exit_on_command_not_found(cmd, env);
	env_arr = env_list_to_tab(env);
	execve(path, cmd->argv, env_arr);
	perror(cmd->argv[0]);
	clean_on_exit(cmd, env);
	free(path);
	free_split(env_arr);
	exit(127);
}

static void	wait_for_child(pid_t last_pid)
{
	pid_t		wpid;
	int		status;

//	while ((wpid = wait(&status)) > 0)
	wpid = wait(&status);
	while (wpid > 0)
	{
		if (wpid == last_pid)
		{
			if (WIFEXITED(status))
				g_exit_status = WEXITSTATUS(status);
		}
		if (WIFSIGNALED(status))
		{
			g_exit_status = WTERMSIG(status) + 128;
			if (WCOREDUMP(status))
				write(2, "Quit (core dumped)\n", 19);
		}
		wpid = wait(&status);
	}
}

static int	get_redir_fd(t_redir *r)
{
	if (r->type == TOKEN_REDIR_OUT)
		return (open(r->filename, O_WRONLY | O_CREAT | O_TRUNC, 0644));
	else if (r->type == TOKEN_REDIR_APPEND)
		return (open(r->filename, O_WRONLY | O_CREAT | O_APPEND, 0644));
	else if (r->type == TOKEN_REDIR_IN)
		return (open(r->filename, O_RDONLY));
	else if (r->type == TOKEN_HEREDOC)
		return (r->heredoc_fd);
	return (-1);
}

static void	apply_redirection(t_cmd *cmd, t_env *env)
{
	int	fd;
	t_redir	*r;

	r = cmd->redirections;
	fd = -1;
	while (r)
	{
		fd = get_redir_fd(r);
		if (fd < 0)
		{
			perror(r->filename);
			clean_on_exit(cmd, env);
			exit(1);
		}
		if (r->type == TOKEN_REDIR_IN || r->type == TOKEN_HEREDOC)
			dup2(fd, STDIN_FILENO);
		else
			dup2(fd, STDOUT_FILENO);
		if (r->type != TOKEN_HEREDOC)
			close(fd);
		r = r->next;
	}
}

static void	process_child(t_cmd *cmd, t_env *env)
{
	int	ret;

	apply_redirection(cmd, env);
	check_first_cmd(cmd, env);
	ret = execute_children_builtin(cmd, env);
	if (ret != -1)
		on_neg_ret(ret, cmd, env);
	execute_child(cmd, env);
}

static void	cleanup_parent(t_cmd **cmd, int pipe_fd[2], int *prev_fd)
{
	if (*prev_fd != -1)
		close(*prev_fd);
	if ((*cmd)->next)
	{
		close(pipe_fd[1]);
		*prev_fd = pipe_fd[0];
	}
	*cmd = (*cmd)->next;
}

static pid_t	process_cmd(t_cmd *cmd_list, t_env *env, int prev_fd)
{
	int	pipe_fd[2];
	t_cmd	*cmd;
	pid_t	last_pid;

	cmd = cmd_list;
	last_pid = 0;
	while (cmd)
	{
		if (cmd->next)
			pipe(pipe_fd);
		last_pid = fork();
		if (on_fork_error(last_pid) == 0)
			return (0);
		if (last_pid == 0)
		{
			setup_childs_fds(last_pid, cmd_list, pipe_fd, prev_fd);
			if (cmd->next)
				close_pipes(pipe_fd);
			process_child(cmd, env);
		}
		else
			cleanup_parent(&cmd, pipe_fd, &prev_fd);
	}
	return (last_pid);
}

void	executor(t_cmd *cmd_list, t_env *env)
{
	int		prev_fd;
	t_cmd	*cmd;
	pid_t	last_pid;

	prev_fd = -1;
	cmd = cmd_list;
	execute_parent_builtin(cmd_list, env);
	last_pid = process_cmd(cmd_list, env, prev_fd);
	if (prev_fd != -1)
		close(prev_fd);
	wait_for_child(last_pid);
}
/* EXECUTOR BLOCK ENDS HERE */

/* MAIN BLOCK STARTS */
t_env	*init_shell_env(char **envp)
{
	t_env	*env_list;

	env_list = init_environment(envp);
	if (env_list)
		increment_shelvl(&env_list);
	else
		env_list = populate_empty_envlist();
	return (env_list);
}

char	*get_input(t_env *env)
{
	char	*input;

	input = readline("MOGILLIO> ");
	if (!input)
	{
		free_env_list(env);
		printf("exit\n");
		exit (g_exit_status);
	}
	if (*input != '\0')
		add_history(input);
	return (input);
}

t_cmd	*make_cmds(char *input)
{
	t_token	*token_list;
	t_cmd	*cmds;

	token_list = lexer(input);
	if (!token_list)
		return (NULL);
	cmds = parser(token_list);
	free_tokens(token_list);
	return (cmds);
}

int	on_execution(t_cmd *cmds, t_env *env)
{
	expander(cmds, env);
	handle_heredocs(cmds, env);
	if (g_exit_status < 0)
	{
		g_exit_status = 130;
		cleanup_heredocs(cmds);
		return (0);
	}
	executor(cmds, env);
	cleanup_heredocs(cmds);
	return (1);
}

int	main(int argc, char **argv, char **envp)
{
	t_env	*env_list;
	char	*input;
	t_cmd	*cmd_list;

	(void) argc;
	(void) argv;
	env_list = init_shell_env(envp);
	signal(SIGINT, handle_sigint);
	signal(SIGQUIT, SIG_IGN);
	while (1)
	{
		input = get_input(env_list);
		if (!*input && (free(input), 1))
			continue ;
		cmd_list = make_cmds(input);
		free(input);
		if (!cmd_list)
			continue ;
		on_execution(cmd_list, env_list);
		free_cmd_list(cmd_list);
	}
	free_env_list(env_list);
	return (0);
}
