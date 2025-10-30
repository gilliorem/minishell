#include "../include/minishell.h"

// this function has to be a builtin because it works on the actual
// process environment. the parent process.
// structure wise: I need to have the environment.
// what does it mean to change environment ?...
// after a successfull cd, getcwd()
// to find the username, can execve the whoami

void	pwd_builtin()
{
	char buf[1096];
	char *wd = getcwd(buf, sizeof(buf));
	printf("%s\n",wd);
}

int	env_builtin(char *arg, t_shell *shell)
{
	int	i;

	i = 0;
	if (ft_strncmp(arg, "env", 4) != 0)
	{
		perror("env builtin");
		return (0);
	}
	while (shell->env_list[i])
	{
		printf("%s\n",shell->env_list[i]);
		i++;
	}
	return (1);
}

int	cd_builtin(char *arg, char *path, t_shell *shell)
{
	char *home = get_env_home(shell->env_list);
	int dir = 0;
	update_old_pwd_env(shell);
	if (ft_strncmp(arg, "cd", 3) == 0)
	{
		if (path != NULL)
			dir = chdir(path);
		else
		{
			dir = chdir(home);
			printf("welcome home.\n");
		}
		if (dir != 0)
		{
			perror("chdir error");
			return (0);
		}
	}
	update_pwd_env(shell);
	print_pwds(shell->env_list);
	return (1);
}

int	export_builtin(char *arg, t_shell *shell)
{

}

// TODO: Replace the char *arg with shell->arg
int	unset_builtin(t_shell *shell, char *var)
{
	int	i;

	i = 0;
	if (ft_strncmp(shell->args[0], "unset", 6) != 0)
	{
		perror("unset");
		return (0);
	}
	while (shell->env_list[i])
	{
		if (ft_strncmp(shell->env_list[i], var, ft_strlen(var) + 1) == 0)
		{
			free(shell->env_list[i]);
			shell->env_list[i] = NULL;
		}
		i++;
	}
	return (1);
}
