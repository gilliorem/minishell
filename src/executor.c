#include "../include/minishell.h"

/* the *executor*
 * as an input, it is going to take a linked list
 * from the parser that will comtain the following command
 * ls -l | wc -l
 * my job is to execute this command. 
 * but wait, if I have ls -l | wc -l, I have 2 cmds. I will already know that
 * I have two cmds. So the same command will be called twice ?
 * the forking part is in the executor,
 * what if there is only one cmd ? the execute part doest not execute differently,
 * still,  I need to fork in the execution part since, we are forking at each new command.
 * I think the executor does not go through the parser, it just goes through the list of commands ?
 *
 * ok so lets say that the parser is giving us a linked list of 2 commands
 * which are "ls -c" and "wc -l" and that the operator is a pipe"
 * 
 * so we have two different cases, either a command either a redirection,
 * since I wont execute a file as I treat a file.
 *
 * ok so for now it is the executor's responsability to check for redirection element,
 * like a file and skip it.
 * 
 * in my understanding the executor just run cmd thats it.
 * goes through the list of parsed cmds and them accordingly, and then we redirect...
 *
 * !!! so the executor will look a lot like the lexer. 
 * because it will do different execution part based on the token type.
 * we dont need to check for any pipe since, if there is a pipe, there is 2 cmds.
 * we just need to check for redirection.
 * 
 * if there is a redirection, we do the fd manipulation accordingly using dup2
 * if not, we go through the cmd list and run them
 *
 * ptr[0] = ls
 * ptr[1] = -l
 *
 * ptr[0] = wc
 * ptr[1] = -l
 * */

//TODO: FREE
char	**get_executable_path(t_shell *shell)
{
	char	*env_values = getenv_builtin(shell, "PATH");
	char	**env_value = ft_split(env_values, ':');
	char	**executable = (char**)malloc(sizeof(char*));

	for (int i = 0; env_value[i]; i++)
	{
		executable[i] = ft_strjoin(env_value[i], "/");
		printf("%s\n",executable[i]);
	}
	return (executable);
}

// more than one cmd -> we pipe
// and pipe takes precderence on any reirection
// can use access to check if it executable first

/* So first we need to check if it's a builtin cmd
 * if it is, we call the builtin function without forking and
 * thats pretty much it.
 * Then, we will check for a REDIR_IN sign, if there is a one
 * we will call exec_redirin() function. 
 * we will fork so the shell stdin does not change into the input
 * file and the redirection appends only in the child process. 
 * basically, we dont want to change parents fd. */

int	execute_builtin(char **cmd, t_shell *shell)
{
	if (ft_strncmp(cmd[0], "cd", 3) == 0)
		return (cd_builtin(cmd[0], cmd[1], shell));
	if (ft_strncmp(cmd[0], "pwd", 4) == 0)
		return (pwd_builtin());
	if (ft_strncmp(cmd[0], "echo", 5) == 0)
		return (echo_builtin(cmd));
	if (ft_strncmp(cmd[0], "unset", 6) == 0)
		return (unset_builtin(shell, cmd));
	if (ft_strncmp(cmd[0], "export", 7) == 0)
		return (export_builtin(shell, cmd));
	if (ft_strncmp(cmd[0], "env", 4) == 0)
		return (env_builtin(cmd[0], shell));
	if (ft_strncmp(cmd[0], "exit", 5) == 0)
		return (exit_builtin());
	return (0);
}

void	execute_builtin();
void	execute_pipe();
void	execute_redirin();
void	execute_redirout();
void	execute_heredoc();
void	execute_append();


int	execute(t_shell *shell, t_cmd **cmd_list)
{
	int	child = -1;
	char	**path = get_executable_path(shell);
	while (*cmd_list != NULL)
	{
		// check for built ins
		child = fork();
		execve(path, *cmd_list->ptr, shell->env_list);
		*cmd_list = *cmd_list->next;
	}		
}


/* in case of a heredocs, I need to to do some kind of filter - reading from the stdin and have a delimiter*/
