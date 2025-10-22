#include <stdio.h>
# include "../libft/libft.h"
# include <stdlib.h>
# include <string.h>
# include <stdio.h>
# include <readline/readline.h>
# include <readline/history.h>
# include <unistd.h>
# include <signal.h>



void	temp_echo(char *cmd_line, char *cmd_name);
void	get_input()
{
	while (1)
	{
		char *input = readline("> ");
		temp_echo(input, "echo");
	}
}

int	ft_isspace(char c)
{
	if (c == ' ')
		return 1;
	return 0;	
}

void	temp_echo(char *cmd_line, char *cmd_name)
{
	if (strcmp(cmd_name, "echo") == 0)
		cmd_name = "echo";
	else if (strcmp(cmd_name, "echo -n") == 0)
		cmd_name = "echo -n";
	else 
	{
		perror("echo");
		return ;
	}
	char *echo_cmd = strstr(cmd_line, cmd_name);
	if (!echo_cmd || *echo_cmd == '\0')
		return ;
	while (!ft_isspace(*echo_cmd) && *echo_cmd)
	{
		echo_cmd++;
	}
	echo_cmd++;
	printf("%s\n", echo_cmd);
	return ;
}
// need to redefine argv within minishell readline


int main()
{
	get_input();
}
