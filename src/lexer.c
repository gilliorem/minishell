#include "../include/minishell.h"

t_token *lexer(char *input)
{
	t_token	*head;
	char	*word;
	int	i;
	int	start;

	head = NULL;
	i = 0;
	start = 0;
	while (input[i])
	{
		if (ft_isspace(input[i]))
		{
			i++;
			continue;
		}
		if (input[i] == '|')
		{
			add_token_back(&head, new_token(ft_strdup("|"), TOKEN_PIPE));
			i++;
		}
		else if (input[i] == '>')
		{
			if (input[i+1] == '>')
			{
				add_token_back(&head, new_token(ft_strdup(">>"), TOKEN_REDIR_APPEND));
				i += 2;
			}
			else
			{
				add_token_back(&head, new_token(ft_strdup(">"), TOKEN_REDIR_OUT));
				i++;
			}
		}
		else if (input[i] == '<')
		{
			if (input[i] == '<')
			{
				add_token_back(&head, new_token(ft_strdup("<<"), TOKEN_HEREDOC));
				i += 2;
			}
			else
			{
				add_token_back(&head, new_token(ft_strdup("<"), TOKEN_REDIR_IN));
				i++;
			}
		}
		else
		{
			start = i;
			while (input[i] && !ft_isspace(input[i] && !ft_strchr("|<>", input[i])))
			{
				if (input[i] == '\'' || input[i] == '"')
				{
					char quote = input[i++];
					while (input[i] && input[i] != quote)
						if (input[i])
							i++;
				}
				else
					i++;
			}
			word = ft_substr(input, start, i - start);
			add_token_back(&head, new_token(word, TOKEN_WORD));
		}
	}
	return (head);
}
