#include "../include/minishell.h"

t_token *new_token(char *value, t_tokentype type)
{
	t_token	*token;

	token = malloc(sizeof(t_token));
	if (!token) exit(1);
	token->value = value;
	token->type = type;
	token->next = NULL;
	printf("Token: %s, Type: %d \n", value, type);
	return (token);
}

void	add_token_back(t_token **list, t_token *new)
{
	t_token	*current;

	if (!*list)
	{
		*list = new;
		return ;
	}
	current = *list;
	while (current->next)
	{
		current = current->next;
		current->next = new;
	}
}
