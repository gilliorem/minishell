#include "../include/minishell.h"

void	reset_prompt(int sig)
{
	//mohd is here
	(void)sig;
	write(STDOUT_FILENO, "\n", 1);
	rl_replace_line("test", 10);
	rl_on_new_line();
	rl_redisplay();
}

void	handle_sigint(int sig)
{
	printf("Caught signal %d\n", sig);
	exit(0);
}

void	get_user_input()
{
	char **input = (char**)calloc(10, sizeof(char*));
	int i = 0;
	while (1)
	{
		input[i] = readline("$minichel$ ");
		if (input[i][0] == 'x')
			break;
		add_history(input[i]);
		if (ft_strncmp(input[i], "clear", 5) == 0)
			clear_history();
		i++;
	}
}

int main()
{
	signal(SIGINT, reset_prompt);
	while (1)
	{
		get_user_input();
		sleep(1);
	}
}
