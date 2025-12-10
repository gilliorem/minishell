#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <readline/readline.h>
#include "../../libft/libft.h"

// we run argv[0]
// we catch `exit`
// we fork cmd 
// we can print the exit code

int g_exit = 0;

void run_cmd(char *input)
{
	int argc = 0;
	char **argv = ft_split(input, ' ');
	if (!argv[argc])
		return;
	while (argv[argc])
		argc++;
	
	if (strcmp(argv[0], "exit") == 0)
	{
		exit (g_exit);
	}

	if (strcmp(argv[0], "$?") == 0)
	{
		printf("%d\n", g_exit);
		return ;
	}

	int pid = fork();

	if (pid == -1)
	{
		perror("fork");
		exit(1);
	}
	if (pid == 0)
	{
		execvp(argv[0], argv);	
		perror("execve");
		exit(127);
	}

	int status;
	int w_pid = waitpid(pid, &status, 0);
	if (w_pid == -1)
	{
		perror("waitpid");
		exit (1);
	}
	if (WIFEXITED(status))
	{
		printf("child exit normally. back to parent process\n");
		g_exit = WEXITSTATUS(status);
	}
       	if (WIFSIGNALED(status))
	{
		printf("child received a signal.\n");
		g_exit = WTERMSIG(status) + 128;
	}
}

void handle_sigint(int sig)
{
	(void) sig;
	printf("SIG INT catched\n");
	write(1, ">", 1);
	g_exit = 130;
}

int main()
{
	signal(SIGINT, handle_sigint);

	char *input;
	while (1)
	{
		input = readline(">");
		run_cmd(input);
	}

}
