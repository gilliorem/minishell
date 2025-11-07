#include "../include/minishell.h"

t_token *new_token(char *value, t_tokentype type) 
{
    t_token *token = malloc(sizeof(t_token));
    if (!token) exit(1);
    token->value = value;
    token->type = type;
    token->next = NULL;
    printf("Token %s Type %d \n", value, type);
    return token;
}

void add_token_back(t_token **list, t_token *new) 
{
    if (!*list) { *list = new; return; }
    t_token *current = *list;
    while (current->next) current = current->next;
    current->next = new;
}

int count_words(t_token *tok)
{
    int wc = 0;
    while (tok && tok->type != TOKEN_PIPE)
    {
        if (tok->type == TOKEN_WORD) wc++;
        else if (tok->type >= TOKEN_REDIR_IN) 
            if (tok->next) tok = tok->next;
        tok = tok->next;
    }
    return wc;
}

void remove_quotes(t_cmd *cmd)
{
    int i = 0, j, k;
    while (cmd->ptr && cmd->ptr[i])
    {
        char *original = cmd->ptr[i];
        printf ("Removing quotes from: %s\n", original);
        size_t len = ft_strlen(original);
        char *cleaned = malloc(len + 1);
        j = 0; k = 0;
        while (original[j])
        {
            if (original[j] == '\'' || original[j] == '"')
            {
                char quote = original[j++];
                while (original[j] && original[j] != quote)
                    cleaned[k++] = original[j++];
                if (original[j]) j++;
            }
            else cleaned[k++] = original[j++];
        }
        cleaned[k] = '\0';
        printf ("Cleaned to: %s\n", cleaned);
        free(original);
        cmd->ptr[i] = cleaned;
        i++;
    }
}
