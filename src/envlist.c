#include "../include/minishell.h"

void	copy_envlist(t_shell *shell, char *env[])
{
	int	i;
	shell->env_list = (char**)ft_calloc(1, sizeof(char*));

	i = 0;
	while (env[i])
	{
		shell->env_list[i] = ft_strdup(env[i]);
		//printf("%s\n", shell->env_list[i]);
		i++;
	}
}

char	*update_pwd_env(t_shell *shell)
{
	char	*cur_dir;
	char	buf[1024];
	int	cur_dir_len;
	int	i;
	
	i = 0;
	chdir(buf);
	cur_dir_len = ft_strlen(cur_dir);
	
	while (shell->env_list[i])
	{
		if (ft_strncmp(shell->env_list[i], "PWD=", 4) == 0)
		{
			shell->pwd = ft_strjoin("PWD=", buf);
			shell->env_list[i] = ft_strdup(shell->pwd);
			
		}
		i++;
	}
}
