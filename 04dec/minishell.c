/* Dec 4, testing (cgoh, cedric, brandon) */

/*
 * export test!=abc
  bash: export: `test!=abc': not a valid identifier
  export key value should not take any special char
 
 * exit 9223372036854775808 (more than long long int) 
 >$ exit 9223372036854775808
 exit
 bash: exit: 9223372036854775808: numeric argument required
 can use string compare: if the string value is more than long long int limit, print the error.

 * exit with no arguments does not overwrite the last exit status
  >$ echo ^C
  >$ exit
  exit
  >$ echo $?
  130
  --> prints the status of echo ^C, not the status of `exit` 

 * export spaces="           "
  should run the ls command (as white spaces are ignored) 
  >$ $spaces ls $spaces
  a.out minishell.c

 * Expansion with echo
  echo "'$USER'"
  >$ 'regillio'
  double quote should ignore everything until it finds a double quote

 * Expansion and quotes for heredocs
  when heredocs see a DOUBLE quote, it should NOT do the `$` expansion
  cat << "hi" OR cat << 'hi' are both exited typing `hi` with no quotes
  	first case does not expand environment variables, second case does.

 * Write in stderror fd
  We should be able to write in the specific fd specify in the redirect out `>`
  >$ cat -e wrong_param 2>b.out
  >$ cat b.out
  >$ cat: wrong_param: No such file or directory
 
 * catch fd limits: if fd > 1024, echo will just print the value as plain text
  >$ cat -e fichier 1024>c.out
  bash: 1024: Bad file descriptor

  >$ cat -e file 2>c.out // will run and the content of file will print on stdout //fd range: ([3-1023])
			 // /!\ it will not write in the outfile /!\

  >$ cat -e file 1>c.out // will run but the content of file will not print on stdout; ->it will write in c.out<-
 
 * our program should not crash if it does not have the env list:
 > env -i ./a.out
 > Segmentation fault (core dumped)

 */

/* yuchi tests */

/* echo -nnnn hello: hello */


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

typedef enum e_tokentype { TOKEN_WORD, TOKEN_PIPE, TOKEN_REDIR_IN, TOKEN_REDIR_OUT, TOKEN_REDIR_APPEND, TOKEN_HEREDOC } t_tokentype;
typedef struct s_token { char *value; t_tokentype type; struct s_token *next; } t_token;
typedef struct s_redir { char *filename; t_tokentype type; int heredoc_fd; struct s_redir *next; } t_redir;
typedef struct s_cmd { char **argv; t_redir *redirections; struct s_cmd *next; } t_cmd;
typedef struct s_env { char *key; char *value; size_t exit_status; struct s_env *next; } t_env;

int g_in_child = 0; 

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

void	increment_shelvl(t_env **envp)
{
	int shlvl = 0;
	t_env *node = *envp;
	while (node->next != NULL)
	{
		if (strcmp(node->key, "SHLVL") == 0)
		{
			shlvl = atoi(node->value) + 1;
			node->value = ft_itoa(shlvl);
		}
		node = node->next;
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

/* now stop comparing ? with env value since we move out of the env list */
void ft_env(t_env *env)
{
	while (env) 
	{ 
		if (env->value)
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

/* now checks for OLDPWD env */
int ft_cd(t_cmd *cmd, t_env *env)
{
	char *target_path = NULL;
	if (cmd->argv[2])
	{
		write(2, "minishell: cd: too many arguments\n", 34);
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
	{
		target_path = oldpwd;
		if (!target_path)
		{
			write(2, "minishell: cd: OLDPWD not set\n", 30);
			return (1);
		}
	}
	else
		target_path = ft_strdup(cmd->argv[1]);
	if (chdir(target_path) != 0)
	{
		perror("bash: cd");
		return (1);
	}
	update_pwd(env, "PWD");
	free(target_path);
	return 0;
}

/* add-on helper */
int	is_number(char *s)
{
	int	i;

	i = 0;
	while (s[i])
	{
		if (s[0] == '-' || s[0] == '+')
			i++;
		if (s[i] < '0' || s[i] > '9')
			return (0);
		i++;
	}
	return (1);
}

// if there is more than one argument (numbers)
// exit will print `exit\n`
// bash: exit: too many arguments
// **and it will not exit the program**

// if sec arg is not a number, 
// exit will print `exit\n`,
// bash: exit

/* now we have a propper exit builtin,
 * that returns the exit code and pass it to the global exit_status */
int	ft_exit(t_cmd *cmd, t_env *env)
{
	int i = 0;
	int exit = 0;
	printf("exit\n");
	while (cmd->argv[i])
	{
		if (i > 1)
		{
			exit = 1;
			printf("minishell: exit: too many arguments\n");
			break;
		}
		if (strcmp(cmd->argv[0], "exit") == 0 && !cmd->argv[1])
		{
			exit = 0;
			break;
		}
		if (!is_number(cmd->argv[1]))
		{
			exit = 2;
			printf("minishell: exit: %s: numeric argument required\n", cmd->argv[1]);
			break;
		}
		else
			exit = atoi(cmd->argv[1]);
		i++;
	}
	if (env->exit_status >= 256)
		return (exit % 256);
	return (exit);
}

/* now check if can write */
int ft_echo(t_cmd *cmd)
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
		if (write(1, cmd->argv[i], strlen(cmd->argv[i])) == -1)
		{
			printf("minishell: echo: cannot write here\n");
			return (1);
		}
		if (cmd->argv[i + 1])
			write(1, " ", 1);
		i++;
	}
	if (newline)
		write(1, "\n", 1);
	return (0);
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
	if (ft_isalpha_underscore(key[0]) || ft_isdigit(key[0]))
	{
		printf("%c is not a valid identifier [%s]\n", key[0], key); 
		return 0;
	}
	while (key[i] || key[i] == '=')
	{
		if (ft_isalpha_underscore(key[i]) == 0)
			return (0);
		i++;
	}
	return (1);
}

/* export KEY=VALUE */
/* weird cases: = `a=`ok `a==`ok (even save the value as `=`) 
   cannot start with a special characters nor a number, 
 * if there is an equal sign, it has to be one only with valid char on the right and any char left 
 * I need to have 
 * the value cannot contain special characters */


/* now export with no arg prints the entire env list */
void	print_env_list(t_env **env_head)
{
	t_env *tmp = *env_head;
	while (tmp->next != NULL)
	{
		printf("declare -x %s=\"%s\"\n", tmp->key, tmp->value);
		tmp = tmp->next;
	}
}

// the first char can only be [A-Z] [a-z] _

int	check_first_char (char c)
{
	if ((c >= 'a' && c <= 'z')
			|| (c >= 'A' && c <= 'Z')
			|| (c == '_'))
		return (1);
	return (0);
}

/* Now check for valid first char */
int	ft_export(t_cmd *cmd, t_env **env_head) 
{
	if (!cmd->argv[1])
		print_env_list(env_head);
	for (int i = 1; cmd->argv[i]; i++)
	{
		char *arg = cmd->argv[i];
		if (check_first_char(arg[0]) == 0)
		{
			printf("minishell: export: '%s' not a valid identifier\n", arg);
			return (1);
		}
		char *eq_pos = strchr(arg, '=');
		if (eq_pos)
		{
			char *key = ft_substr(arg, 0, eq_pos - arg);
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
	}
	return (0);
}

int	ft_unset(t_cmd *cmd, t_env **env_head) 
{
	if (!cmd->argv[1])
		return (0);
	for (int i = 1; cmd->argv[i]; i++)
	{
		int j = 0;
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

		j++;
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

/* broken parser
t_cmd *parser(t_token *tok, t_env *env) 
{
	if(!tok)
		return NULL;
	t_cmd *head=new_cmd();
	t_cmd *cur=head;
	int wc=count_words_argv(tok);
	// cur is a cmd
	cur->argv=malloc(sizeof(char*)*(wc+1));
	int i=0;
	while(tok)
	{
		if(tok->type==TOKEN_PIPE) 
		{
			cur->argv[i]=NULL;
			cur->next=new_cmd();
			cur=cur->next;
			tok=tok->next;
			wc=count_words_argv(tok);
			cur->argv=malloc(sizeof(char*)*(wc+1));
			i=0;
			continue;
		}
		if(tok->type >= TOKEN_REDIR_IN) 
		{
			t_redir *r=malloc(sizeof(t_redir));
			r->type=tok->type;
			r->heredoc_fd = -1;
			if(!tok||tok->type!=TOKEN_WORD) 
				return NULL;
			r->next=cur->redirections;
			cur->redirections=r;
		} 
		else
			cur->argv[i++]=ft_strdup(tok->value);
		tok=tok->next;
	} 
	cur->argv[i]=NULL;
	return head;
}
*/

/*good parser from Nov30 */
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


/*
   check_writable_file(t_env *env, t_token *tok)
   {

   printf("filename:%s\n",r->filename);
   printf("redir: %d\n", cur->redirections->type); 
   printf("join_path:%s\n", ft_strjoin(get))
   if (strstr(r->filename, "full") != NULL && cur->redirections->type == 3)
   {
   printf("minishell: echo: write error: No space left on device\n");
   break;
   }

   }
   */

char *get_env_val_wrapper(char *key, t_env *env) { char *v=get_env_val(env, key); return v ? v : ft_strdup(""); }
char *expand_str(char *s, t_env *env) {
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
				v = ft_itoa(env->exit_status);
			else
				v=get_env_val_wrapper(k,env); 
			free(k); char *t=ft_strjoin(res,v); free(res); free(v); res=t; 
		}
		else { 
			char c[2]={s[i++],0}; char *t=ft_strjoin(res,c); free(res); res=t; 
		}
	} free(s); return res;
}
void expander(t_cmd *cmd, t_env *env) 
{
	while(cmd) 
	{
		int i=0; while(cmd->argv&&cmd->argv[i]) 
		{ 
			if(strchr(cmd->argv[i],'$')) 
				cmd->argv[i]=expand_str(cmd->argv[i],env); 
			i++;
		}
		t_redir *r=cmd->redirections;
		while(r) 
		{
			if(strchr(r->filename,'$'))
				r->filename=expand_str(r->filename,env);
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

/* now runs the exit builtin */

void executor(t_cmd *cmd_list, t_env *env)
{
	int pipe_fd[2]; 
	int prev_fd = -1; 
	t_cmd *cmd = cmd_list;
	int status = 0;

	if (cmd && !cmd->next && cmd->argv && cmd->argv[0]) {
		if (strcmp(cmd->argv[0], "exit") == 0) 
		{
			env->exit_status = ft_exit(cmd_list, env);
			free_cmd_list(cmd_list);
			exit(env->exit_status);
		} 
		else if (strcmp(cmd->argv[0], "export") == 0){status = ft_export(cmd, &env);env->exit_status = status;return;}
		else if (strcmp(cmd->argv[0], "unset") == 0) { status = ft_unset(cmd, &env); env->exit_status = status; return; }
		else if (strcmp(cmd->argv[0], "cd") == 0) { status = ft_cd(cmd, env); env->exit_status = status; return; }
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
	env->exit_status = status;

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

int main(int argc, char **argv, char **envp)
{
	(void)argc; (void)argv;
	t_env *env_list = init_environment(envp);
	increment_shelvl(&env_list);
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

		expander(cmds, env_list);
		remove_quotes(cmds);
		handle_heredocs(cmds); 

		g_in_child = 1;

		executor(cmds, env_list);

		cleanup_heredocs(cmds); 
		free_cmd_list(cmds);
	}
	free_env_list(env_list);
	return 0;
}
