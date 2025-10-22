#include "../include/minishell.h"

void	reset_prompt(int sig)
{
	//mohd is here
	(void)sig;
	write(STDOUT_FILENO, "\n", 1);
	rl_replace_line("", 0);
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
//	char *shell_str = input[0];
//	int shell_str_len = ft_strlen(shell_str);
	while (1)
	{
		input[i] = readline("> ");
		if (input[i][0] == 'x')
			break;
		add_history(input[i]);
		if (ft_strncmp(input[i], "clear", 5) == 0)
			clear_history();
		if (ft_strncmp(input[i], "echo", 4) == 0)
		{
			ft_echo(input[i], "echo", input[i]);
			reset_prompt(0);
		}
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
