#include "include/minishell.h"

t_shell *init_shell()
{
	t_shell *shell;
	shell = ft_calloc(1, sizeof(t_shell));
	return shell;
}

/* main in construction...*/
int	main(void)
{
	char	*input;
	while (1)
	{
		input = readline("MOGIL> ");
		if (!input) 
		{
			printf("\n");
			break;
		}
		if (!*input) 
		{
			free(input);
			continue;
		}
		add_history(input);
		t_token *tokens = lexer(input);
		free(input);
		if (!tokens)
			continue;
	}
	return (0);
}
