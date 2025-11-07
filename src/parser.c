#include "../include/minishell.h"

t_cmd *new_cmd(void)
{
    t_cmd *cmd = malloc(sizeof(t_cmd));
    if (!cmd) exit(1);
    cmd->ptr = NULL; cmd->redirs = NULL; cmd->next = NULL; 
    return cmd;
}

t_cmd *parser(t_token *token) 
{
    if (!token) return NULL; 
    t_cmd *head = new_cmd();  
    t_cmd *cmd = head; 
    t_token *tok = token; 
    int wc = count_words(tok); 
    cmd->ptr = malloc(sizeof(char *) * (wc + 1)); 
    if (!cmd->ptr) exit(1);
    int i = 0;
    while (tok) 
    {
        if (tok->type == TOKEN_PIPE) 
        {
            cmd->ptr[i] = NULL; 
            cmd->next = new_cmd(); 
            cmd = cmd->next; 
            tok = tok->next; 
            wc = count_words(tok); 
            cmd->ptr = malloc(sizeof(char *) * (wc + 1));
            if (!cmd->ptr) exit(1);
            i = 0;
            continue;
        }
        if (tok->type >= TOKEN_REDIR_IN) 
        {
            t_redir *r = malloc(sizeof(t_redir));
            r->type = tok->type;
            printf("PARSER: Redirection of type %d\n", r->type);
            tok = tok->next; 
            if (!tok || tok->type != TOKEN_WORD) return NULL;
            r->filename = ft_strdup(tok->value);
            printf("PARSER: Redirection filename '%s'\n", r->filename);
            r->next = cmd->redirs;
            cmd->redirs = r;
        } else 
        {
            cmd->ptr[i++] = ft_strdup(tok->value);
            printf("PARSER: Added word   '%s'\n", tok->value);
        }
        tok = tok->next;
    }
    cmd->ptr[i] = NULL;
    return head;
}

