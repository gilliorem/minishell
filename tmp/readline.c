# include "../libft/libft.h"
# include <stdlib.h>
# include <stdio.h>
# include <string.h>
# include <sys/wait.h>
# include <readline/readline.h>
# include <readline/history.h>
# include <unistd.h>
# include <signal.h>
# include <stdbool.h>


void	get_user_input()
{
	char **input = (char**)calloc(10, sizeof(char*));
	int i = 0;
	while (1)
	{
		input[i] = readline("> ");
		if (input[i][0] == 'x')
			break;
		add_history(input[i]);
		if (strncmp(input[i], "clear", 5) == 0)
			clear_history();
		i++;
	}
}

int main()
{
	
	
		get_user_input();
	
}
