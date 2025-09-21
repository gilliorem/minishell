#include "../include/minishell.h"

void	sigint_handler(int sig)
{
	(void)sig;
	write(STDOUT_FILENO, "\n", 1);
	rl_replace_line("", 0);
	rl_on_new_line();
	rl_redisplay();
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
	signal(SIGINT, sigint_handler);
	get_user_input();
}
