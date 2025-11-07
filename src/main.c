#include "../include/minishell.h"

t_shell *init_shell()
{
	t_shell *shell;
	shell = ft_calloc(1, sizeof(t_shell));
	return shell;
}


int main(int argc, char *argv[], char *envp[])
{
	if (argc != 1)
		return 0;
	(void) argv;
    char *input;
    t_shell *shell = init_shell();
    copy_envlist(shell, envp);
    while (1)
    {
        input = readline("MOGILLIO> "); 
        if (!input) { printf("\n"); break; } 
        if (!*input) { free(input); continue; } 
        add_history(input); 
        t_token *tokens = lexer(input); 
        free(input); 
        if (!tokens) continue;
        t_cmd *cmd_list = parser(tokens);
        free_tokens(tokens);
        if (!cmd_list) continue;
        t_cmd *temp_cmd = cmd_list;
        while (temp_cmd) {
        remove_quotes(temp_cmd);
        temp_cmd = temp_cmd->next;
	execute(shell, cmd_list);

        }
    }
    return (0);
}
