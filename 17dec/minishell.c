// env -i ./a.out : need to be able to export variable [X]
// add the _/last/cmd/ran variable

//atoi from C library - change to ft_atoi

/* MEMORY ISSUES */
/* In case of cmd error such as: `cd | ls` quotes, parser, new_cmd, init_environment  */

/*Eric tests
  >>>readline suppression (valgrind file; not take into account readline leaks) [X]

  EXIT STATUS
 *Thoughts: Need a global variable that keeps track of our exit code... and we try to get read of g_inchild*
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

 echo hello > f1 > f2  // shoud have hello in f2 last one should get the input, the other will be blank

 << and quotes: cat << "HI" : same rules apply here and echo // should exit HI

 - EXECUTION mixing builtins and binaries (Mohid)

 export | head
 ls | cd .. // should run but not change dir because executing in the child process
 cd | ls
 ls |      // make sure the pipe fd is closed
 */


// Failed tests
// cd OLDPWD state [X] - Remi
// export with no args [x] - Remi
// $? (mohid) [X]
// builtins return value (1 on failure, 0 on success to match with $) [X]
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

typedef enum e_tokentype { 
    TOKEN_WORD, TOKEN_PIPE, TOKEN_REDIR_IN, TOKEN_REDIR_OUT, TOKEN_REDIR_APPEND, TOKEN_HEREDOC 
} t_tokentype;

typedef struct s_token { 
    char *value; 
    t_tokentype type; 
    struct s_token *next; 
} t_token;

typedef struct s_redir { 
    char *filename; 
    t_tokentype type; 
    int heredoc_fd; 
    int heredoc_quoted;
    struct s_redir *next; 
} t_redir;

typedef struct s_cmd { 
    char **argv; 
    t_redir *redirections; 
    struct s_cmd *next; 
} t_cmd;

typedef struct s_env { 
    char *key; 
    char *value; 
    struct s_env *next; 
} t_env;

int g_exit_status = 0;

void handle_sigint(int sig)
{
	extern unsigned long	rl_readline_state;
 	g_exit_status = 128 + sig;
	write(1, "\n", 1);
	rl_on_new_line();
	rl_replace_line("", 0);
	if (rl_readline_state & RL_STATE_READCMD)
	{
		rl_redisplay();
	}
}

int	init_signal(int signal, void(*f)(int s))
{
	struct sigaction sa;

	sa.sa_handler = f;
	sa.sa_flags = SA_RESTART;
	sigemptyset(&sa.sa_mask);
	if (sigaction(signal, &sa, NULL) == -1)
		return (1);
	return (0);
}
void handle_sigint_heredoc(int sig);
void set_heredoc_signals(void)
{
    struct sigaction sa;

    sa.sa_handler = handle_sigint_heredoc;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, NULL);

    signal(SIGQUIT, SIG_IGN);
}

void restore_signals(void)
{
    signal(SIGINT, handle_sigint);
    signal(SIGQUIT, SIG_IGN);
}

void handle_sigquit(int sig)
{
	(void) sig;
	g_exit_status = 128 + sig;
	if (init_signal(SIGQUIT, SIG_IGN))
		return ;
	kill(0, SIGQUIT);
	printf("Quit (core dumped)\n");
}

void handle_sigint_heredoc(int sig)
{
	(void)sig;
	g_exit_status = -130;
	write(1, "\n", 1);
}

// --- LIBFT-LIKE FUNCTIONS --- //

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

void free_split(char **arr) { 
    int i=0; if (!arr) return; 
    while(arr[i]) free(arr[i++]); 
    free(arr); 
}

static int get_len(int n) {
    int len=0; if(n<=0) len++; while(n){ n/=10; len++; } return len;
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

int is_number(char *s)
{
    int i;
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

int ft_isalpha_underscore(int c)
{
    if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c == '_'))
        return (1);
    return (0);
}

int ft_isdigit(int c)
{
    if (c >= '0' && c <= '9') return (1);
    return (0);
}

// from line 188 upto here is LIBFT

t_env *new_env_node(char *key, char *value) 
{
    t_env *node = malloc(sizeof(t_env));
    node->key = key; node->value = value; node->next = NULL;
    return node;
}

void free_env_list(t_env *env) 
{
    t_env *tmp;
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
    int shlvl = 0;
    t_env *node = *envp;
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

t_env *init_environment(char **envp) 
{
       	t_env *head = NULL;
       	t_env *tail = NULL;
       	int i = 0;
	if (!envp)
	{
		printf("empty env list\n");
		return NULL;
	}
       	while (envp[i]) {
	       	char *eq_pos = strchr(envp[i], '=');
	       	if (eq_pos) 
		{
		       	char *key = ft_substr(envp[i], 0, eq_pos - envp[i]);
		       	char *value = ft_strdup(eq_pos + 1);
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
       	return head;
}

char *get_env_val(t_env *env, char *key) 
{
    while (env) 
    { 
        if (strcmp(env->key, key) == 0) 
            return ft_strdup(env->value); 
        env = env->next; 
    }
    return NULL;
}

char **env_list_to_tab(t_env *env) {
    int count = 0; t_env *tmp = env; while (tmp) { count++; tmp = tmp->next; }
    char **tab = malloc(sizeof(char *) * (count + 1)); int i = 0; tmp = env;
    while (tmp) {
        char *temp = ft_strjoin(tmp->key, "="); tab[i++] = ft_strjoin(temp, tmp->value); free(temp); tmp = tmp->next;
    }
    tab[i] = NULL; return tab;
}

t_env *populate_empty_envlist()
{
	t_env *node;
	char cwd[1024];

	char *key = ft_strdup("PWD");
	char *value = ft_strdup(getcwd(cwd, sizeof(cwd)));
	node = new_env_node(key, value);
	return (node);
}

void ft_env(t_env *env) 
{
       	while (env)
       	{
	       	if (strcmp(env->value, "") != 0) 
			printf("%s=%s\n", env->key, env->value);
	       	env = env->next;
       	}
}

int update_env(t_env *env, char *key, char *new_value) 
{
    while (env) 
    { 
        if (strcmp(env->key, key) == 0) 
        { 
            free(env->value);
            env->value = ft_strdup(new_value);
            return 1;
        }
        env = env->next; 
    }
    return 0;
}

t_token *new_token(char *value, t_tokentype type) {
    t_token *token = malloc(sizeof(t_token)); token->value = value; token->type = type; token->next = NULL; return token;
}

void add_token_back(t_token **list, t_token *new) 
{
	if (!*list)
		*list = new;
	else 
	{
		t_token *c = *list;
		while (c->next)
			c = c->next;
		c->next = new;
	}
}

t_token *lexer(char *input) {
    t_token *head = NULL; int i = 0;
    while (input[i]) {
        if (isspace(input[i])) { i++; continue; }
        if (strchr("|<>", input[i])) {
            if (input[i]=='|') { add_token_back(&head, new_token(ft_strdup("|"), TOKEN_PIPE)); i++; }
            else if (input[i]=='>') { if (input[i+1]=='>') { add_token_back(&head, new_token(ft_strdup(">>"), TOKEN_REDIR_APPEND)); i+=2; } else { add_token_back(&head, new_token(ft_strdup(">"), TOKEN_REDIR_OUT)); i++; } }
            else if (input[i]=='<') { if (input[i+1]=='<') { add_token_back(&head, new_token(ft_strdup("<<"), TOKEN_HEREDOC)); i+=2; } else { add_token_back(&head, new_token(ft_strdup("<"), TOKEN_REDIR_IN)); i++; } }
        } else {
            int start = i;
            while (input[i] && !isspace(input[i]) && !strchr("|<>", input[i])) {
                if (input[i]=='\''||input[i]=='"') { char q=input[i++]; while(input[i]&&input[i]!=q) i++; if(input[i]) i++; } else i++;
            }
            add_token_back(&head, new_token(ft_substr(input, start, i-start), TOKEN_WORD));
        }
    }
    return head;
}

void free_tokens(t_token *t) { t_token *tmp; while(t) { tmp=t->next; free(t->value); free(t); t=tmp; } }

t_cmd *new_cmd(void) { t_cmd *c = malloc(sizeof(t_cmd)); c->argv=NULL; c->redirections=NULL; c->next=NULL; return c; }

int count_words_argv(t_token *t) { 
    int c=0; 
    while(t&&t->type!=TOKEN_PIPE) { 
        if(t->type==TOKEN_WORD)c++; 
        else if(t->type>=TOKEN_REDIR_IN) { 
            if(t->next) t=t->next; 
        } 
        t=t->next; 
    } 
    return c; 
}

void add_redir_back(t_redir **list, t_redir *new) 
{
    if (!*list) 
    {
        *list = new;
        new->next = NULL;
    } else 
    {
        t_redir *c = *list;
        while (c->next)
            c = c->next;
        c->next = new;
        new->next = NULL;
    }
}

void free_redirs(t_redir *r) { 
    t_redir *tmp; 
    while(r) { 
        tmp=r->next; 
        free(r->filename); 
        free(r); 
        r=tmp; 
    } 
}

void free_cmd_list(t_cmd *c) { 
    t_cmd *tmp; 
    while(c) { 
        tmp=c->next; 
        int i=0; 
        if(c->argv){
            while(c->argv[i])
	    {
		    if (c->argv[i])
			    free(c->argv[i++]);
	    }
            free(c->argv);
        } 
        if(c->redirections) free_redirs(c->redirections); 
        free(c); 
        c=tmp; 
    } 
}

t_cmd   *parser(t_token *tok)
{
    t_cmd   *head;
    t_cmd   *cur;
    int     wc;
    int     i;

    if (!tok)
        return (NULL);
    head = new_cmd();
    cur = head;
    wc = count_words_argv(tok);
    cur->argv = malloc(sizeof(char *) * (wc + 1));
    if (!cur->argv)
        return (NULL);
    i = 0;
    while (tok)
    {
        if (tok->type == TOKEN_PIPE)
        {
            cur->argv[i] = NULL;
            cur->next = new_cmd();
            cur = cur->next;
            tok = tok->next;
            
            wc = count_words_argv(tok);
            cur->argv = malloc(sizeof(char *) * (wc + 1));
            i = 0;
            continue ;
        }

        if (tok->type >= TOKEN_REDIR_IN && tok->type <= TOKEN_HEREDOC)
        {
            t_redir *r = malloc(sizeof(t_redir));
            r->type = tok->type;
            r->heredoc_fd = -1;
            r->next = NULL;
            tok = tok->next;
            if (!tok || tok->type != TOKEN_WORD)
            {
                printf("minishell: syntax error near unexpected token\n");
                free(r);
		        return NULL; //relook at this
                free_cmd_list(head); // to check for `<<`
                return (NULL);
            }

            r->filename = ft_strdup(tok->value);
            add_redir_back(&cur->redirections, r);
            
            tok = tok->next;
            continue ; 
        }

        cur->argv[i++] = ft_strdup(tok->value);
        tok = tok->next;
    }
    cur->argv[i] = NULL;
    return (head);
}

char *get_env_val_wrapper(char *key, t_env *env) { char *v=get_env_val(env, key); return v ? v : ft_strdup(""); }

char *expand_heredoc_line(char *str, t_env *env)
{
    char *res = ft_strdup("");
    int i = 0;

    while (str[i])
    {
        if (str[i] == '$')
        {
            if (str[i+1] == '?' || isalpha(str[i+1]) || str[i+1] == '_')
            {
                i++;
                char *key;
                
                if (str[i] == '?') {
                    key = ft_strdup("?");
                    i++;
                } else {
                    int start = i;
                    while (str[i] && (isalnum(str[i]) || str[i] == '_'))
                        i++;
                    key = ft_substr(str, start, i - start);
                }
                
                char *val;
                if (strcmp(key, "?") == 0)
                    val = ft_itoa(g_exit_status);
                else
                    val = get_env_val_wrapper(key, env);
                
                char *tmp = ft_strjoin(res, val);
                free(res);
                free(key);
                free(val);
                res = tmp;
            }
            else
            {
                char one_char[2] = {str[i], 0};
                char *tmp = ft_strjoin(res, one_char);
                free(res);
                res = tmp;
                i++;
            }
        }
        else
        {
            char one_char[2] = {str[i], 0};
            char *tmp = ft_strjoin(res, one_char);
            free(res);
            res = tmp;
            i++;
        }
    }
    free(str);
    return res;
}

char *expand_str(char *s, t_env *env) {
    char *res=ft_strdup(""); int i=0;
    while(s[i]) {
        if(s[i]=='\'') { 
            i++; int st=i; while(s[i]&&s[i]!='\'') i++; 
            char *sub=ft_substr(s,st,i-st); 
            char *t=ft_strjoin(res,sub); free(res); free(sub); res=t; 
            if(s[i])i++; 
        }
        else if(s[i]=='$') { 
            i++;
            int st=i;
            if (s[i] == '?') {
                i++;
            } else {
                while(isalnum(s[i])||s[i]=='_') i++; 
            }
            char *k=ft_substr(s,st,i-st); 
            char *v;
            if (k[0] == '?' && k[1] == '\0')
                v = ft_itoa(g_exit_status);
            else
                v=get_env_val_wrapper(k,env); 
            free(k); char *t=ft_strjoin(res,v); free(res); free(v); res=t; 
        }
        else { 
            char c[2]={s[i++],0}; char *t=ft_strjoin(res,c); free(res); res=t; 
        }
    } free(s); return res;
}

char *process_token(char *str, t_env *env, int *quote_found)
{
    char *res = ft_strdup("");
    int i = 0;
    bool in_sq = false;
    bool in_dq = false;

    while (str[i])
    {
        if (str[i] == '\'' && !in_dq)
        {
            in_sq = !in_sq;
            if (quote_found) 
                *quote_found = 1;
            i++;
        }

        else if (str[i] == '\"' && !in_sq)
        {
            in_dq = !in_dq;
            if (quote_found)
                *quote_found = 1;
            i++; 
        }
        
        else if (str[i] == '$' && !in_sq)
        {
        
            if (str[i+1] == '?' || isalpha(str[i+1]) || str[i+1] == '_')
            {
                i++; 
                char *key;
                
                if (str[i] == '?')
                {
                    key = ft_strdup("?");
                    i++;
                }
                else
                {
                    int start = i;
                    while (str[i] && (isalnum(str[i]) || str[i] == '_'))
                        i++;
                    key = ft_substr(str, start, i - start);
                }
                
                char *val;
                if (strcmp(key, "?") == 0)
                    val = ft_itoa(g_exit_status);
                else
                    val = get_env_val_wrapper(key, env);
                
                char *tmp = ft_strjoin(res, val);
                free(res);
                free(key);
                free(val);
                res = tmp;
            }
            else
            {
                char one_char[2] = {str[i], 0};
                char *tmp = ft_strjoin(res, one_char);
                free(res);
                res = tmp;
                i++;
            }
        }
        else
        {
            char one_char[2] = {str[i], 0};
            char *tmp = ft_strjoin(res, one_char);
            free(res);
            res = tmp;
            i++;
        }
    }
    free(str);
    return res;
}

void expander(t_cmd *cmd, t_env *env) 
{
    while(cmd) 
    {
        int i=0; 
        while(cmd->argv && cmd->argv[i]) 
        { 
            cmd->argv[i] = process_token(cmd->argv[i], env, NULL);
            i++;
        }
        t_redir *r = cmd->redirections;
        while(r) 
        {
            int quote_found = 0;
            r->filename = process_token(r->filename, env, &quote_found);

            if (r->type == TOKEN_HEREDOC)
                r->heredoc_quoted = quote_found;
            r = r->next;
        }
        cmd = cmd->next;
    }
}

//New readline for heredocs signal compatible
char *readline_heredocs(void)
{
	char *line;
	size_t line_size = 256;
	size_t len = 0;
	char c;
	ssize_t bytes_read;

	line = malloc(line_size); // malloc our return buffer	// which will be too big but thats ok : design choice
	
	while (1)
	{
		bytes_read = read(1, &c, 1);
		if (bytes_read == 0) break;
		if (bytes_read < 0)
		{
			return (free(line), NULL);
		}
		if (c == '\n') break;
		line[len] = c;
		len++;
	}
	if (len == 0 && bytes_read == 0)
		return(free(line), NULL);

	line[len] = '\0';
	return (line);
}

// modify run heredocs to handle sigint correctly
int	run_heredocs(char *delimiter, int quoted, t_env *env)
{
	int fd[2];
	char *line;
	
	if (pipe(fd) == -1)
		return (-1);
	set_heredoc_signals();
	while (1)
	{
		write (1, "> ", 2);
		line = readline_heredocs();
		if (g_exit_status < 0)
		{
			restore_signals();
			close(fd[0]);
			close(fd[1]);
			return (-1);
		}

		if (!line)
			break;
		if (strcmp(line, delimiter) == 0)
		{
			free(line);
			break;
		}
        if (!quoted)
            line = expand_heredoc_line(line, env);
		write(fd[1], line, strlen(line));
		write(fd[1], "\n", 1);
		free(line);
	}
	restore_signals();
	close(fd[1]);
	return (fd[0]);
}

void handle_heredocs(t_cmd *cmd_list, t_env *env)
{
    t_cmd *cmd = cmd_list;
    while (cmd)
    {
        t_redir *r = cmd->redirections;
        while (r)
        {
            if (r->type == TOKEN_HEREDOC)
            {
                r->heredoc_fd = run_heredocs(r->filename, r->heredoc_quoted, env);
		if (r->heredoc_fd == -1)
			return ;
            }
            r = r->next;
        }
        cmd = cmd->next;
    }
}

void close_unused_heredoc_fds(t_cmd *cmd_list, t_cmd *current_cmd)
{
    t_cmd *tmp = cmd_list;
    while (tmp)
    {
        if (tmp != current_cmd)
        {
            t_redir *r = tmp->redirections;
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

void cleanup_heredocs(t_cmd *cmd_list)
{
    t_cmd *cmd = cmd_list;
    while (cmd)
    {
        t_redir *r = cmd->redirections;
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

void ft_pwd(void)
{
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)))
        printf("%s\n", cwd);
    else
        perror("pwd");
}

void update_pwd(t_env *env, char *pwd)
{
    char cwd[1024];
    getcwd(cwd, sizeof(cwd));
    update_env(env, pwd, cwd);
}

/* CD BLOCK STARTING HERE */

int	cd_argument(t_cmd *cmd)
{
	if (cmd->argv[1] && cmd->argv[2])
	{
		ft_putstr_fd("minishell: cd: too many arguments\n", 2);
		return (0);
	}
	return (1);
}

char	*cd_home(t_cmd *cmd, t_env *env)
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

char	*cd_oldpwd(t_env *env)
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
void ft_exit(t_cmd *cmd, t_env *env)
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
		printf("minishell: exit: %s: numeric argument required\n", cmd->argv[1]);
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
int	is_new_line(char *arg)
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
			break;
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
		ft_putstr_fd("\n", 1);
	return (0);
}
/* End of echo block */

void	print_env_list(t_env **env_head) 
{
	t_env *tmp;
       
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

int	check_first_char (char c)
{
    if ((c >= 'a' && c <= 'z') 
	|| (c >= 'A' && c <= 'Z') || (c == '_'))
	    return (1);
    ft_putstr_fd("minishell: export: not a valid identifier:", 2);
    ft_putchar_fd(c, 2);
    ft_putchar('\n');
    return (0);
}

int	check_key(char *key)
{
       	int	i;

       	i = 0;
	while (key[i])
       	{
	       	if (ft_isdigit(key[i]))
	       	{
		       	i++;
		       	continue;
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
	if (!*env_head)
		*env_head = new;
	else 
	{
		t_env *tmp = *env_head;
		while (tmp->next)
			tmp = tmp->next;
		tmp->next = new;
	}
	return (*env_head);
}

int	export_key_as_var(char *keyvar, t_env **env_head)
{
	int	i;
	t_env	*no_value_node;
	
	i = 0;
	if (!check_key(keyvar))
		return (0);
	no_value_node =  new_env_node(keyvar, ft_strdup(""));
	add_envback(env_head, no_value_node);
	return (1);
}

int	export_value(char *value, char *key, t_env **env_head)
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
	if (!cmd->argv[1])
	       	print_env_list(env_head);
	while (cmd->argv[i])
	{ 
	       	if (!check_first_char(cmd->argv[i][0]))
		       	return (1); 
		eq_pos = strchr(cmd->argv[i], '=');
		if (!eq_pos) 
			if (!export_key_as_var(ft_strdup(cmd->argv[i]), env_head))
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
			break;
		}
		prev = curr;
		curr = curr->next;
	}
}

int ft_unset(t_cmd *cmd, t_env **env_head) 
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

char*	absolute_cmd(char *cmd)
{
	if (ft_strchr(cmd, '/'))
	{ 
		if (access(cmd, X_OK) == 0)
			return ft_strdup(cmd); 
	}
	return (NULL);
}

char	**get_directories(t_env *env)
{
	char	*path_env;
	char	**directories;

	path_env = get_env_val(env, "PATH"); 
	if (!path_env)
		return (NULL);
	directories = ft_split(path_env, ':'); 
	return (directories);
}

char	*get_command_path(char *cmd, t_env *env)
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

/* --- EXECUTOR --- */

void	execute_parent_builtin(t_cmd *cmd, t_env *env)
{
	if (cmd && !cmd->next && cmd->argv && cmd->argv[0])
	{
		if (ft_strcmp(cmd->argv[0], "exit") == 0)
		{
			clean_on_exit(cmd, env);
			ft_exit(cmd, env); // need to clean up when exiting.
		}
		else if (ft_strcmp(cmd->argv[0], "export") == 0 && cmd->argv[1])
		{
			g_exit_status = ft_export(cmd, &env);
			return ;
		}
		else if (ft_strcmp(cmd->argv[0], "unset") == 0)
		{
			g_exit_status = ft_unset(cmd, &env);
			return ;
		}
		else if (ft_strcmp(cmd->argv[0], "cd") == 0)
		{
			g_exit_status = ft_cd(cmd, env);
			return ;
		}
	}
}

int	execute_children_builtin(t_cmd *cmd, t_env *env)
{
	int	ret;

	ret = 0;
	if (strcmp(cmd->argv[0], "env") == 0) 
		ft_env(env);
	else if (strcmp(cmd->argv[0], "pwd") == 0)
		ft_pwd();
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

// connect the pipes between the i cmd (2 or more)
// make the stdin/stdout of this child point to the right place
void	pipeline_orchestration(t_cmd *cmd, t_cmd *cmd_list, int prev_fd, int pipe_fd[2])
{
	pid_t	last_pid;
	while (cmd)
	{
		if (cmd->next)
			pipe(pipe_fd);
		last_pid = fork();
		if (last_pid == -1)
		{
			perror("fork");
			return ;
		}
		if (last_pid == 0)
		{
			signal(SIGINT, SIG_DFL);
			signal(SIGQUIT, SIG_DFL);
			close_unused_heredoc_fds(cmd_list, cmd);
			
			// set_up_fds()

			// apply_redirection()

			// execute_children_builtin()

		}
		// clean_parent_fds()
	}
	//update_status()
}

/* executor
└── run_parent_builtin_if_needed
└── pipeline_orchestration
    ├── fork
    │   ├── (child)
    │   │   ├── setup_child_fds        ← plumbing
    │   │   ├── apply_redirections     ← still plumbing
    │   │   └── execute_child_command  ← logic
    │   └── (parent)
    │       └── cleanup_parent_fds
└── wait_and_update_status
*/


void executor(t_cmd *cmd_list, t_env *env)
{
    int pipe_fd[2]; 
    int prev_fd = -1;  // there is no previous cmd output to read from
    t_cmd *cmd = cmd_list;
    int status = 0;
    execute_parent_builtin(cmd_list, env);

    pid_t last_pid = 0;
    while (cmd) 
    {
        if (cmd->next) 
		pipe(pipe_fd);
        last_pid = fork();
        if (last_pid == -1) 
	{
	       	perror("fork");
	       	return;
       	}
        if (last_pid == 0) 
	{ 
            signal(SIGINT, SIG_DFL);
            signal(SIGQUIT, SIG_DFL);
            close_unused_heredoc_fds(cmd_list, cmd);
            if (prev_fd != -1) 
	    {
		    dup2(prev_fd, 0);
		    close(prev_fd); 
	    }
            if (cmd->next) 
	    {
		    close(pipe_fd[0]);
		    dup2(pipe_fd[1], 1);
		    close(pipe_fd[1]);
	    }
            t_redir *r = cmd->redirections;
            while (r) 
	    {
                int fd = -1;
                if (r->type == TOKEN_REDIR_OUT) 
		{
                    fd = open(r->filename, O_WRONLY|O_CREAT|O_TRUNC, 0644);
                }
                else if (r->type == TOKEN_REDIR_APPEND) 
		{
                    fd = open(r->filename, O_WRONLY|O_CREAT|O_APPEND, 0644);
                }
                else if (r->type == TOKEN_REDIR_IN) {
                    fd = open(r->filename, O_RDONLY);
                }
                else if (r->type == TOKEN_HEREDOC) {
                    fd = r->heredoc_fd;
                }

                if (fd < 0) { perror(r->filename); exit(1); }

                if (r->type == TOKEN_REDIR_IN || r->type == TOKEN_HEREDOC)
                    dup2(fd, STDIN_FILENO);
                else
                    dup2(fd, STDOUT_FILENO);

                if (r->type != TOKEN_HEREDOC) close(fd); 
                r = r->next;
            }

            if (!cmd->argv[0])
	    {
		    clean_on_exit(cmd, env);
		    exit(0);
	    } 
	    /*
            if (strcmp(cmd->argv[0], "env") == 0) 
	    {
		    ft_env(env);
		    clean_on_exit(cmd, env);
		    exit(0); 
	    }
            else if (strcmp(cmd->argv[0], "pwd") == 0) { ft_pwd(); clean_on_exit(cmd, env);exit(0); }
            else if (strcmp(cmd->argv[0], "echo") == 0) { ft_echo(cmd);clean_on_exit(cmd, env); exit(0); }
            else if (strcmp(cmd->argv[0], "cd") == 0) 
	    {
		    clean_on_exit(cmd, env);
		    exit(0); 
	    } 
	    else if (strcmp(cmd->argv[0], "export") == 0) { int ret = ft_export(cmd, &env);clean_on_exit(cmd, env); exit(ret); }
            else if (strcmp(cmd->argv[0], "unset") == 0) { int ret = ft_unset(cmd, &env);clean_on_exit(cmd, env); exit(ret); }
	    */
	    //int child_builtin_ret = 0;
	    g_exit_status = execute_children_builtin(cmd, env);
	    //clean_on_exit(cmd, env);
	    //exit (child_builtin_ret);
            char *path = get_command_path(cmd->argv[0], env);
            if (!path) { 
                write(2, "minishell: command not found: ", 30);
                write(2, cmd->argv[0], strlen(cmd->argv[0]));
                write(2, "\n", 1);
		clean_on_exit(cmd, env);
                exit(127); 
            }

            char **env_arr = env_list_to_tab(env);
            execve(path, cmd->argv, env_arr);
            perror(cmd->argv[0]); 
	    clean_on_exit(cmd, env);
	    free(path);
            exit(127);
        }
        else {
            if (prev_fd != -1) close(prev_fd);
            if (cmd->next) { close(pipe_fd[1]); prev_fd = pipe_fd[0]; }
            cmd = cmd->next;
        }
    }

    if (prev_fd != -1) close(prev_fd); 

    pid_t wpid;
    while ((wpid = wait(&status)) > 0)
    {
        if (wpid == last_pid)
        {
            if (WIFEXITED(status))
                g_exit_status = WEXITSTATUS(status);
        }
	if (WTERMSIG(status))
	{
		g_exit_status = WTERMSIG(status) + 128;
		// check if allowed function else Remi to rewrite 
        if (WCOREDUMP(status))
			write(2, "Quit (core dumped)\n",19);
	}
    }
}

int main(int argc, char **argv, char **envp)
{
    (void)argc; (void)argv;
    printf("--- The best Minishell is in the works ---\n");
    t_env *env_list = init_environment(envp);
    if (env_list)
	    increment_shelvl(&env_list);
    else
	    env_list = populate_empty_envlist();
    char *input;

    signal(SIGINT, handle_sigint);  
    signal(SIGQUIT, SIG_IGN);       

    while (1) {
        input = readline("MOGILLIO> ");
        if (!input) { printf("exit\n"); break; }
        if (!*input) { free(input); continue; }
        add_history(input);

        t_token *tok = lexer(input); 
        free(input);
        
        if (!tok)
	       	continue;
        
        t_cmd *cmds = parser(tok);
        free_tokens(tok);
        
        if (!cmds)
		continue;

        expander(cmds, env_list);
        handle_heredocs(cmds, env_list); 
	if (g_exit_status < 0)
	{
		g_exit_status = 130;
		cleanup_heredocs(cmds);
		free_cmd_list(cmds);
		continue;
	}
        executor(cmds, env_list);
        cleanup_heredocs(cmds); 
        free_cmd_list(cmds);
    }
    free_env_list(env_list);
    return 0;
}
