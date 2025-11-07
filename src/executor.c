#include "../include/minishell.h"

/* the *executor*
 * as an input, it is going to take a linked list
 * from the parser that will comtain the following command
 * ls -l | wc -l
 * my job is to execute this command. 
 * but wait, if I have ls -l | wc -l, I have 2 cmds. I will already know that
 * I have two cmds. So the same command will be called twice ?
 * the forking part is in the executor, the piping and the forking append for different reason
 * the forking happens each time there is a simple command, the piping happens when there is
 * this `|`.
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

char	**get_executable_path(t_shell *shell)
{
	char	*paths = getenv_builtin(shell, "PATH");
	if (!paths)
		return (NULL);
	char	**path = ft_split(paths, ':');
	if (!path)
		return (NULL);
	return (path);
}

int	execute_binary(t_shell *shell, t_cmd *cmd)
{
	char	**paths;
	char	*full_path;
	char	*cmd_path;
	
	paths = get_executable_path(shell);
	if (!paths)
		return (127);
	full_path = NULL;
	for (int i = 0; paths[i]; i++)
	{
		full_path = ft_strjoin(paths[i], "/");
		cmd_path = ft_strjoin(full_path, cmd->ptr[0]);
		//printf("%s\n", cmd_path);
		//printf("%s\n", full_path);
		free(full_path);
		if (access(cmd_path, X_OK) == 0)
		{
			printf("argv[0]:%s, argv[1]:%s\n", cmd->ptr[0], cmd->ptr[1]);
			execve(cmd_path, cmd->ptr, shell->env_list);
			//     ls -a     "ls, -a, NULL"
			perror("execve");
			exit(127);
		}
	}
	free(cmd_path);
	printf("cmd not found\n");
	free(paths);
	return (127);
}

// more than one cmd -> we pipe
// and pipe goes first before any redirection
// can use access to check if it executable first

/* So first we need to check if it's a builtin cmd
 * if it is, we call the builtin function without forking and
 * thats pretty much it.
 * Then, we will check for a REDIR_IN sign, if there is a one
 * we will call exec_redirin() function. 
 * we will fork so the shell stdin does not change into the input
 * file and the redirection appends only in the child process. 
 * basically, we dont want to change parents fd. */

int	execute_simple_command(t_shell *shell, t_cmd *cmd)
{
	int	pid;
	int	status;

	status = 0;
	if (execute_builtin(cmd->ptr, shell))
		return (0);
	pid = fork();
	if (pid == 0)
	{
		if (cmd->redirs)
			set_redirect(cmd->redirs);
		execute_binary(shell, cmd);
		perror("execve");
		exit(127);
	}
	else if (pid < 0)
		perror("fork error");
	else
		waitpid(pid, &status, 0);
	return (WEXITSTATUS(status));
}

int	set_redirect(t_redir *redir)
{
	int	fd;

	while (redir)
	{
		if (redir->type == TOKEN_REDIR_IN)
		{
			fd = open(redir->filename, O_RDONLY);
			dup2(fd, STDIN_FILENO);
			close(fd);
		}
		else if (redir->type == TOKEN_REDIR_OUT)
		{
			fd = open(redir->filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
			dup2(fd, STDOUT_FILENO);
			close(fd);
		}
		else if (redir->type == TOKEN_REDIR_APPEND)
		{
			fd = open(redir->filename, O_WRONLY | O_CREAT | O_APPEND, 0644);
			dup2(fd, STDOUT_FILENO);
			close(fd);
		}
		else if (redir->type == TOKEN_HEREDOC)
			printf("heredox\n");
		redir = redir->next;
	}
	return (0);
}

int	execute_pipe(t_shell *shell, t_cmd *cmd)
{
	int	fds[2];
	int	fd_in;
	pid_t	pid;

	fd_in = STDIN_FILENO;
	while (cmd)
	{
		pipe(fds);
		pid = fork();
		if (pid == 0)
		{
			dup2(fd_in, STDIN_FILENO);
			if (cmd->next)
			{
				printf("cmd next\n");
				dup2(fds[1], STDOUT_FILENO);
			}
			close(fds[0]);
			if (cmd->redirs)
				set_redirect(cmd->redirs);
			execute_binary(shell, cmd);
			perror("execve");
			return (0);
		}
		else
		{
			close(fds[1]);
			if (fd_in != STDIN_FILENO)
				close(fd_in);
			fd_in = fds[0];
			cmd = cmd->next;
		}
	}
	return (0);
}

int	execute(t_shell *shell, t_cmd *cmd)
{
	if (!cmd)
		return (0);
	if (cmd->next)
		return (execute_pipe(shell, cmd));
	else
		return (execute_simple_command(shell, cmd));
	return (0);
}

/* in case of a heredocs, I need to to do some kind of filter - reading from the stdin and have a delimiter (the second argument)*/
