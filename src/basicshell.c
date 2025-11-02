#include "../include/minishell.h"

char *ft_strcpy(const char *src, int len)
{
    char *dest = malloc(len + 1);
    if (!dest) {
        perror("malloc");
        return NULL;
    }
    memcpy(dest, src, len);
    dest[len] = '\0';
    return dest;
}

char *exp_env_var(const char *input)
{
    int i = 1; 
    while (input[i] && (isalnum(input[i]) || input[i] == '_')) i++;
    char *var_name = ft_strcpy(input + 1, i - 1);
    if (!var_name) return NULL;
    char *var_value = getenv(var_name);
    free(var_name);
    if (!var_value) return ft_strdup("");
    else return ft_strdup(var_value);
}

int parse_input(char *input, char **args, int *quotetype)
{
    int i = 0, j = 0, start = -1, len = 0;
    bool in_quotes = false;
    char quote_char = '\0';
    while (1)
    {
        char c = input[j];
        if ((c == '"' || c == '\'') && !in_quotes) 
        {  
            in_quotes = true; 
            quote_char = c; 
            start = j + 1; 
        } 
        else if (c == quote_char && in_quotes) 
        {
            in_quotes = false;
            len = j - start;
            args[i] = ft_strcpy(&input[start], len);
            if (!args[i++]) exit(1);  
            if (quote_char == '\'') quotetype[i - 1] = 1;
            else quotetype[i - 1] = 2;
            quote_char = '\0';
            start = -1;
        } 
        else if ((c != ' ' && c != '\t' && c != '\0' && c != '\n') && start == -1)
            start = j;
        else if ((c == ' ' || c == '\t' || c == '\0' || c == '\n') && start != -1)
        {
            int len = j - start;
            args[i] = ft_strcpy(&input[start], len);
            if (!args[i]) exit(1);
            quotetype[i] = 0;
            i++; 
            start = -1;
        }
        if (c == '\0') break;
        j++;
    }
    args[i] = NULL;
    return i;
}

t_shell *init_shell()
{
	t_shell *shell;
	shell = ft_calloc(1, sizeof(t_shell));
	return shell;
}

int main(int argc, char *argv[], char *env[])
{
	(void) argv;
	if (argc > 1)
		return EXIT_FAILURE;
    char *input;
    char *args[100];
    int quotetype[100];
    pid_t pid;
    int status, tc, i=0;
    t_shell *shell;
    shell = init_shell();
    copy_envlist(shell, env);
    while (1)
    {
        input = readline("MOGILLIO> ");
        if (!input) { printf("\n"); break; }
        add_history(input);
        tc = parse_input(input, args, quotetype);
        if (tc == 0) { free(input); continue; }
        i = 0;
        while (i < tc)
        {
            if (args[i][0] == '$' && quotetype[i] != 1)
            {
                char *expanded = exp_env_var(args[i]);
                if (expanded)
                {
                    free(args[i]);
                    args[i] = expanded;
                }
            }
            i++;
        }
	if (ft_strncmp(args[0], "env", 4) == 0)
	{
		env_builtin(args[0], shell);
		continue;
	}
	if (ft_strncmp(args[0], "cd", 2) == 0)
	{
		cd_builtin(args[0], args[1], shell);
		continue; 
	}
	if (ft_strncmp(args[0], "pwd", 3) == 0)
	{
		pwd_builtin();
		continue; 
	}
	if (unset_builtin(shell, args))
		continue;
	if (export_builtin(shell, args[0], args[1]))
		continue;
	if (echo_builtin(args))
		continue;
        if (ft_strncmp(args[0],"exit", 5) == 0) 
        {
            i = 0;
            while (i < tc) free(args[i++]);
            free(input);    
            break;
        }
        pid = fork();
        if (pid == 0) { execvp(args[0], args); exit(EXIT_FAILURE);}
            else if (pid < 0) perror("fork error");
            else waitpid(pid, &status, 0);
        i = 0;
        while (i < tc) free(args[i++]);
        free(input);
    }
    return 0;
}

