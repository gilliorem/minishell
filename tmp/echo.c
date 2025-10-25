#include "../include/minishell.h"

// jump the shell str len
// jump the cmd_len
// start printing
void	ft_echo(char *shell_str, char *echo, char *echo_args)
{
	char	*start;
	int	i;

	i = 0;
	start = shell_str;
	while (shell_str[i])
	{
		start++;
		i++;
	}
	i = 0;
	while (echo[i])
	{
		start++;
		i++;
	}
	while (*echo_args)
	{
		ft_putchar(*echo_args++);	
	}
}
