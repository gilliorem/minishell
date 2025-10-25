#include "../include/minishell.h"

// TODO: to be replaced with ft_strdup()
static char *ft_strcpy(const char *src, int len)
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

int parse_input(char *input, char **args)
{
    int i = 0, j = 0, start = -1;
    while (1)
    {
        char c = input[j];
        if ((c != ' ' && c != '\t' && c != '\0' && c != '\n') && start == -1)
            start = j;
        else if ((c == ' ' || c == '\t' || c == '\0' || c == '\n') && start != -1)
        {
            int len = j - start;
            args[i] = ft_strcpy(&input[start], len);
            if (!args[i]) exit(1);
            i++; 
            start = -1;
        }
        if (c == '\0') break;
        j++;
    }
    args[i] = NULL;
    return i;
}

//TODO: create a separate function that use readline
int main(int argc, char *argv[], char *env[])
{
	(void)argv;
	if (argc > 1)
		return EXIT_FAILURE;
	char *input;
	char *args[100];
	pid_t pid;
	int status, tc, i;
	while (1)
	{
		input = readline("MOGILLIO> ");
		if (!input) { printf("\n"); break; }
		add_history(input);
		tc = parse_input(input, args);
		if (tc == 0) { free(input); continue; }
		if (strcmp(args[0]), "cd" == 0 && strcmp(args[1]) == "..")

		if (strcmp(args[0], "cd") == 0)
		{
			cd_builtin(env);
			continue; 
		}
		if (strcmp(args[0], "pwd") == 0)
		{
			pwd_builtin();
			continue; 
		}
		pid = fork();
		if (strcmp(args[0],"exit") == 0) 
		{
			i = 0;
			while (i < tc) free(args[i++]);
			free(input);    
			break;
		}
		// TODO: will have to do a pre-check to seperate cd builtin
		// so our cd never goes into the execvp
		// with a continue statement, it becomes super buggy and slow
		if (pid == 0) { execvp(args[0], args); exit(EXIT_FAILURE);}
		else if (pid < 0) perror("fork error");
		else waitpid(pid, &status, 0);
		i = 0;
		while (i < tc) free(args[i++]);
		free(input);
	}
	return 0;
}

