/*
Failed tests
$? (mohid) [X]
builtins return value (1 on failure, 0 on success to match with $) [X]
cd can take only 0 or 1 argument. [X]
cd ~ [X]
cd - [X]
cd OLPWD state []
update the $PWD [X]
update the $OLDPWD [X]
unset a b [X]
export a=1 b=2 [X]
export identifier [X]
unset identifiers []
export with no args []

cat <<HERE <<DOC
cat <<HERE (Inside $USER should be expanded)
cat <minishell.c <<HERE | cat
cat << $
*/

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

int g_in_child = 0; 
int g_exit_status = 0;

void handle_sigint(int sig)
{
    (void)sig;
    if (g_in_child == 0)
    {
        write(1, "\n", 1);
        rl_on_new_line();
        rl_replace_line("", 0);
        rl_redisplay();
    }
    else
    {
        write(1, "\n", 1);
    }
}

typedef enum e_tokentype { TOKEN_WORD, TOKEN_PIPE, TOKEN_REDIR_IN, TOKEN_REDIR_OUT, TOKEN_REDIR_APPEND, TOKEN_HEREDOC } t_tokentype;
typedef struct s_token { char *value; t_tokentype type; struct s_token *next; } t_token;
typedef struct s_redir { char *filename; t_tokentype type; int heredoc_fd; struct s_redir *next; } t_redir;
typedef struct s_cmd { char **argv; t_redir *redirections; struct s_cmd *next; } t_cmd;

typedef struct s_env { char *key; char *value; struct s_env *next; } t_env;
typedef struct s_shell {int exit_status;} t_shell;

t_shell	*init_shell()
{
	t_shell	*shell = malloc(sizeof(t_shell) * 1);
	shell->exit_status = 0;
	return shell;
}

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
    char *sub = malloc(sizeof(char) * (len + 1)); if (!sub) return NULL;
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
    while (*temp) { if (*temp!=delimiter && !new_word) { new_word=true; count++; } else if (*temp==delimiter) { *temp='\0'; new_word=false; } temp++; }
    return count;
}
static bool fill_ptr(char **ptr, char *s1, int wc) {
    int i=0; while (i<wc) { if (*s1) { ptr[i++]=ft_strdup(s1); if (!ptr[i-1]) return false; while (*s1) s1++; } s1++; } ptr[i]=NULL; return true;
}
char **ft_split(char const *s, char c) {
    if (!s) return NULL; char *s1=ft_strdup(s); if (!s1) return NULL;
    int wc=word_countandreplace(s1, c); char **ptr=malloc(sizeof(char*)*(wc+1));
    if (!ptr || !fill_ptr(ptr, s1, wc)) { free(s1); return NULL; } free(s1); return ptr;
}
void free_split(char **arr) { int i=0; if (!arr) return; while(arr[i]) free(arr[i++]); free(arr); }

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

t_env *init_environment(char **envp) 
{
    t_env *head = NULL; t_env *tail = NULL; int i = 0;
    while (envp[i]) 
    {
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
        } i++;
    }
    return head;
}
// returns a copy of the env value when the key match.
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

void ft_env(t_env *env)
{
    while (env) 
    { 
        if (env->value && strcmp(env->value, "?") != 0)
        printf("%s=%s\n", env->key, env->value); 
        env = env->next; 
    }
}


// if the key match, replace the old env value with a new one.
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

int ft_cd(t_cmd *cmd, t_env *env)
{
    char *target_path = NULL;
    if (cmd->argv[2])
    {
	    write(2, "bash: cd: too many arguments\n", 29);
	    return (1);
    }
    char *oldpwd = get_env_val(env, "OLDPWD");
    update_pwd(env, "OLDPWD");
    if (!cmd->argv[1] || strcmp(cmd->argv[1], "~") == 0)
    {
        target_path = get_env_val(env, "HOME");
        if (!target_path)
        {
            write(2, "minishell: cd: HOME not set\n", 28);
            return 1;
        }
    }
    else if (strcmp(cmd->argv[1], "-") == 0)
	    target_path = oldpwd;
    else
	    target_path = ft_strdup(cmd->argv[1]);
    if (chdir(target_path) != 0)
    {
		    perror("cd");
		    return (1); 
    }
    update_pwd(env, "PWD");
    free(target_path);
    return 0;
}

void ft_echo(t_cmd *cmd)
{
	int i = 1;
	bool newline = true;
	while (cmd->argv[i] && strcmp(cmd->argv[i], "-n") == 0)
	{
		newline = false;
		i++;
	}

	while (cmd->argv[i])
	{
		printf("%s", cmd->argv[i]);
		if (cmd->argv[i + 1])
			printf(" ");
		i++;
	}
	if (newline)
		printf("\n");
}
int	ft_isalpha_underscore(int c)
{
	if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')
	|| (c == '_'))
		return (1);
	return (0);
}

int	ft_isdigit(int c)
{
	if (c >= '0' && c <= '9')
		return (1);
	return (0);
}

// return (1) on success.
int	check_key(char *key)
{
	int	i;

	i = 0;
	if (!ft_isalpha_underscore(key[0]) || ft_isdigit(key[0]))
	{printf("%c is not a valid identifier [%s]\n", key[0], key); return 0;}
	while (key[i] || key[i] == '=')
	{
		if (!ft_isalpha_underscore(key[i]))
			return (0);
		i++;
	}
	return (1);
}

/* export KEY=VALUE */
/* weird cases: = `a=`ok `a==`ok (both save the value as `` and  `=`) 
cannot start with a special characters nor a number, 
 * if there is an equal sign, it has to be one only with valid char on the right and any char left 
 * I need to have 
 * the value cannot contain special characters */

int	ft_export(t_cmd *cmd, t_env **env_head) 
{
	int key_check = 0;
	if (!cmd->argv[1])
		return 1;
	for (int i = 1; cmd->argv[i]; i++)
	{
		char *arg = cmd->argv[i];
		if (arg[0] == '=')
		{
				printf("minishell: export: '=': not a valid identifier\n");
				return (1);
		}
		char *eq_pos = strchr(arg, '=');
		if (eq_pos)
		{
			char *key = ft_substr(arg, 0, eq_pos - arg);
			key_check = check_key(key);
			if (key_check == 0)
			{
				printf("minishell: export: '%s': not a valid identifier\n", cmd->argv[i]);
				free(key);
				return (1);
			}
			char *value = eq_pos + 1;
			if (!update_env(*env_head, key, value)) 
			{
				t_env *new_node = new_env_node(key, ft_strdup(value));
				if (!*env_head)
					*env_head = new_node;
				else
				{ 
					t_env *tmp = *env_head;
					while (tmp->next)
						tmp = tmp->next;
					tmp->next = new_node;
				}
			} 
			else 
				free(key);
		}
		else
		{
			printf("minishell: export: '%s': not a valid identifier\n", cmd->argv[i]);
			return (1);
		}
	}
	return (0);
}

/* returns 1 on failure: when the key does not match the shell format */

int	ft_unset(t_cmd *cmd, t_env **env_head) 
{
	if (!cmd->argv[1])
		return (1);
	for (int i = 1; cmd->argv[i]; i++)
	{
		t_env *curr = *env_head;
		t_env *prev = NULL;
		while (curr) 
		{
			if (strcmp(curr->key, cmd->argv[i]) == 0) 
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
	return (0);
}


char *get_command_path(char *cmd, t_env *env) {
	if (strchr(cmd, '/')) { if (access(cmd, X_OK) == 0) return ft_strdup(cmd); return NULL; }
	char *path_env = get_env_val(env, "PATH"); if (!path_env) return NULL;
	char **directories = ft_split(path_env, ':'); free(path_env);
	char *full_path; char *temp; int i = 0;
	while (directories && directories[i]) {
		temp = ft_strjoin(directories[i], "/"); full_path = ft_strjoin(temp, cmd); free(temp);
		if (access(full_path, X_OK) == 0) { free_split(directories); return full_path; }
		free(full_path); i++;
	}
	free_split(directories); return NULL;
}

t_token *new_token(char *value, t_tokentype type) {
	t_token *token = malloc(sizeof(t_token)); token->value = value; token->type = type; token->next = NULL; return token;
}
void add_token_back(t_token **list, t_token *new) {
	if (!*list) *list = new; else { t_token *c = *list; while (c->next) c = c->next; c->next = new; }
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
t_cmd *new_cmd(void) { t_cmd *c = malloc(sizeof(t_cmd)); c->argv=NULL; c->redirections=NULL; c->next=NULL; return c; }
int count_words_argv(t_token *t) { int c=0; while(t&&t->type!=TOKEN_PIPE) { if(t->type==TOKEN_WORD)c++; else if(t->type>=TOKEN_REDIR_IN) { if(t->next)t=t->next; } t=t->next; } return c; }

t_cmd *parser(t_token *tok) {
	if(!tok) return NULL; t_cmd *head=new_cmd(); t_cmd *cur=head;
	int wc=count_words_argv(tok); cur->argv=malloc(sizeof(char*)*(wc+1)); int i=0;
	while(tok) {
		if(tok->type==TOKEN_PIPE) { cur->argv[i]=NULL; cur->next=new_cmd(); cur=cur->next; tok=tok->next; wc=count_words_argv(tok); cur->argv=malloc(sizeof(char*)*(wc+1)); i=0; continue; }
		if(tok->type>=TOKEN_REDIR_IN) {
			t_redir *r=malloc(sizeof(t_redir)); r->type=tok->type; r->heredoc_fd = -1; tok=tok->next;
			if(!tok||tok->type!=TOKEN_WORD) return NULL;
			r->filename=ft_strdup(tok->value); r->next=cur->redirections; cur->redirections=r;
		} else cur->argv[i++]=ft_strdup(tok->value);
		tok=tok->next;
	} cur->argv[i]=NULL; return head;
}

char *get_env_val_wrapper(char *key, t_env *env) { char *v=get_env_val(env, key); return v ? v : ft_strdup(""); }
char *expand_str(char *s, t_env *env, t_shell *shell) {
	char *res=ft_strdup(""); int i=0;
	while(s[i]) {
		if(s[i]=='\'') { 
			i++; int st=i; while(s[i]&&s[i]!='\'') i++; char *sub=ft_substr(s,st,i-st); char *t=ft_strjoin(res,sub); free(res); free(sub); res=t; if(s[i])i++; 
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
				v = ft_itoa(shell->exit_status);
			else
				v=get_env_val_wrapper(k,env); 
			free(k); char *t=ft_strjoin(res,v); free(res); free(v); res=t; 
		}
		else { 
			char c[2]={s[i++],0}; char *t=ft_strjoin(res,c); free(res); res=t; 
		}
	} free(s); return res;
}
void expander(t_cmd *cmd, t_env *env, t_shell *shell) 
{
	while(cmd) 
	{
		int i=0; while(cmd->argv&&cmd->argv[i]) 
		{ 
			if(strchr(cmd->argv[i],'$')) 
				cmd->argv[i]=expand_str(cmd->argv[i],env, shell); 
			i++;
		}
		t_redir *r=cmd->redirections;
		while(r) 
		{
			if(strchr(r->filename,'$'))
				r->filename=expand_str(r->filename,env, shell);
			r=r->next;
		}
		cmd=cmd->next;
	}
}

void remove_quotes(t_cmd *cmd) {
	while(cmd) {
		int i=0; while(cmd->argv&&cmd->argv[i]) {
			char *old=cmd->argv[i]; int len=strlen(old); char *new=malloc(len+1); int j=0,k=0;
			while(old[j]) { if(old[j]=='\''||old[j]=='"') { char q=old[j++]; while(old[j]&&old[j]!=q) new[k++]=old[j++]; if(old[j]) j++; } else new[k++]=old[j++]; }
			new[k]=0; free(old); cmd->argv[i++]=new;
		} cmd=cmd->next;
	}
}

void free_tokens(t_token *t) { t_token *tmp; while(t) { tmp=t->next; free(t->value); free(t); t=tmp; } }
void free_redirs(t_redir *r) { t_redir *tmp; while(r) { tmp=r->next; free(r->filename); free(r); r=tmp; } }
void free_cmd_list(t_cmd *c) { t_cmd *tmp; while(c) { tmp=c->next; int i=0; if(c->argv){while(c->argv[i])free(c->argv[i++]);free(c->argv);} if(c->redirections)free_redirs(c->redirections); free(c); c=tmp; } }

int run_heredoc(char *delimiter)
{
	int fd[2];
	if (pipe(fd) == -1) { perror("pipe"); return (-1); }

	char *line;
	while (1)
	{
		line = readline("> ");
		if (!line) break;
		if (strcmp(line, delimiter) == 0) { free(line); break; }
		write(fd[1], line, strlen(line));
		write(fd[1], "\n", 1);
		free(line);
	}
	close(fd[1]);
	return (fd[0]); 
}

void handle_heredocs(t_cmd *cmd_list)
{
	t_cmd *cmd = cmd_list;
	while (cmd)
	{
		t_redir *r = cmd->redirections;
		while (r)
		{
			if (r->type == TOKEN_HEREDOC)
			{
				r->heredoc_fd = run_heredoc(r->filename);
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

void executor(t_cmd *cmd_list, t_env *env, t_shell *shell)
{
	int pipe_fd[2]; 
	int prev_fd = -1; 
	t_cmd *cmd = cmd_list;
	int status = 0;
	// add return status for the builtins: cd, export, unset

	if (cmd && !cmd->next && cmd->argv && cmd->argv[0]) {
		if (strcmp(cmd->argv[0], "exit") == 0) { printf("exit\n"); free_cmd_list(cmd_list); exit(0); }
		else if (strcmp(cmd->argv[0], "export") == 0) 
		{ 
			status = ft_export(cmd, &env);
			shell->exit_status = status;
			return; 
		}
		else if (strcmp(cmd->argv[0], "unset") == 0) { status = ft_unset(cmd, &env); shell->exit_status = status; return; }
		else if (strcmp(cmd->argv[0], "cd") == 0) { status = ft_cd(cmd, env); shell->exit_status = status; return; }
	}

	pid_t last_pid = 0;
	while (cmd) {
		if (cmd->next) pipe(pipe_fd);
		last_pid = fork();
		if (last_pid == -1) { perror("fork"); return; }

		if (last_pid == 0) { 
			signal(SIGINT, SIG_DFL);
			signal(SIGQUIT, SIG_DFL);

			close_unused_heredoc_fds(cmd_list, cmd);

			if (prev_fd != -1) { dup2(prev_fd, 0); close(prev_fd); }
			if (cmd->next) { close(pipe_fd[0]); dup2(pipe_fd[1], 1); close(pipe_fd[1]); }

			t_redir *r = cmd->redirections;
			while (r) {
				int fd;
				if (r->type == TOKEN_REDIR_OUT) {
					fd = open(r->filename, O_WRONLY|O_CREAT|O_TRUNC, 0644);
				}
				else if (r->type == TOKEN_REDIR_APPEND) {
					fd = open(r->filename, O_WRONLY|O_CREAT|O_APPEND, 0644);
				}
				else if (r->type == TOKEN_REDIR_IN) {
					fd = open(r->filename, O_RDONLY);
				}
				else if (r->type == TOKEN_HEREDOC) {
					fd = r->heredoc_fd;
				}

				if (fd < 0) { perror("redirection"); exit(1); }

				if (r->type == TOKEN_REDIR_IN || r->type == TOKEN_HEREDOC)
					dup2(fd, STDIN_FILENO);
				else
					dup2(fd, STDOUT_FILENO);

				close(fd);
				r = r->next;
			}

			if (strcmp(cmd->argv[0], "env") == 0) { ft_env(env); exit(0); }
			else if (strcmp(cmd->argv[0], "pwd") == 0) { ft_pwd(); exit(0); }
			else if (strcmp(cmd->argv[0], "echo") == 0) { ft_echo(cmd); exit(0); }

			char *path = get_command_path(cmd->argv[0], env);
			if (!path) { write(2, "minishell: command not found\n", 29); exit(127); }

			char **env_arr = env_list_to_tab(env);
			execve(path, cmd->argv, env_arr);
			perror(cmd->argv[0]); 
			exit(127);
		}
		else {
			if (prev_fd != -1) close(prev_fd);
			if (cmd->next) { close(pipe_fd[1]); prev_fd = pipe_fd[0]; }
			cmd = cmd->next;
		}
	}

	pid_t wpid;
	while ((wpid = wait(&status)) > 0)
	{
		if (wpid == last_pid)
		{
			if (WIFEXITED(status))
				status = WEXITSTATUS(status);
		}
	}
	shell->exit_status = status;

	/*
	   char *status_str = ft_itoa(status);
	   if (!update_env(env, "?", status_str))
	   {
	   t_env *new_node = new_env_node(ft_strdup("?"), ft_strdup(status_str));
	   t_env *tmp = env; 
	   while(tmp->next) tmp=tmp->next; 
	   tmp->next = new_node;
	   }
	   free(status_str);
	   */
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

/* extra tab indent: Remi line added */

int main(int argc, char **argv, char **envp)
{
	(void)argc; (void)argv;
	t_shell *shell = init_shell();
	t_env *env_list = init_environment(envp);
	char *input;

	signal(SIGINT, handle_sigint);  
	signal(SIGQUIT, SIG_IGN);       

	while (1) {
		g_in_child = 0;
		input = readline("MOGILLIO> ");
		if (!input) { printf("exit\n"); break; }
		if (!*input) { free(input); continue; }
		add_history(input);

		t_token *tok = lexer(input); free(input);
		if (!tok) continue;
		t_cmd *cmds = parser(tok); free_tokens(tok);
		if (!cmds) continue;

		expander(cmds, env_list, shell);
		remove_quotes(cmds);
		handle_heredocs(cmds); 

		g_in_child = 1;

		executor(cmds, env_list, shell);

		cleanup_heredocs(cmds); 
		free_cmd_list(cmds);
	}
	free_env_list(env_list);
	return 0;
}
