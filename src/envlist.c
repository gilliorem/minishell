#include "../include/minishell.h"

void	copy_envlist(t_shell *shell, char *env[])
{
	int	i;

	i = 0;
	while (env[i])
		i++;
	shell->env_list = (char**)ft_calloc(i + 1, sizeof(char*));

	i = 0;
	while (env[i])
	{
		shell->env_list[i] = ft_strdup(env[i]);
		//printf("%s\n", shell->env_list[i]);
		i++;
	}
	shell->env_list[i] = NULL;
}

void	update_pwd_env(t_shell *shell)
{
	char	buf[1024];
	int	i;
	
	i = 0;
	getcwd(buf, 1024);
	while (shell->env_list[i])
	{
		if (ft_strncmp(shell->env_list[i], "PWD=", 4) == 0)
		{
			shell->pwd = ft_strjoin("PWD=", buf);
			free(shell->env_list[i]);
			shell->env_list[i] = shell->pwd;
		}
		i++;
	}
}

void	update_old_pwd_env(t_shell *shell)
{
	char	buf[1024];
	int	i;

	i = 0;
	getcwd(buf, 1024);
	while (shell->env_list[i])
	{	
		if (ft_strncmp(shell->env_list[i], "OLDPWD=", 7) == 0)
		{
			shell->oldpwd = ft_strjoin("OLDPWD=", buf);
			free(shell->env_list[i]);
			shell->env_list[i] = shell->oldpwd;
		}
		i++;
	}
}

char	*get_env_home(char **env)
{
	char *home = NULL;
	for (int i = 0; env[i]; i++)
		if (strncmp(env[i], "HOME=", 5) == 0)
			home = ft_strchr(env[i], '/');
	if (!home)
	{
		perror("env home");
		return (NULL);
	}
	return home;
}
/* helper to `grep` the PWD and OLD PWD */
void	print_pwds(char **shell_env)
{
	char *pwd = NULL;
	char *oldpwd = NULL;
	for (int i = 0; shell_env[i]; i++)
	{
		if (strncmp(shell_env[i], "PWD=", 4) == 0)
			pwd = ft_strchr(shell_env[i], '/');
		if (ft_strncmp(shell_env[i], "OLDPWD=", 7) == 0)
			oldpwd = ft_strchr(shell_env[i], '/');
	}
	printf("[PWD:%s]\n[OLDPWD:%s]\n",pwd,oldpwd);
}
