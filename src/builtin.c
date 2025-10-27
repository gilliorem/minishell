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

char*	get_username()
{
	char *username = getenv("USER");	
	printf("%s\n", username);
	return username;
}

char	*get_env_home(char **env)
{
	char *new_home = malloc(1096);
	for (int i = 0; env[i]; i++)
		if (strncmp(env[i], "HOME", 4) == 0)
			new_home = ft_strchr(env[i], '/');
	return new_home;
}

char	*get_pwd(char **env)
{
	char *pwd = NULL;
	for (int i = 0; env[i]; i++)
		if (strncmp(env[i], "PWD", 3) == 0) 
			pwd = ft_strchr(env[i], '/');
	return pwd;
}

// need to update the PWD and OLDPWD
int	cd_builtin(char *arg, char *path, char *env[])
{
	char buf[1096];
	char *home = get_env_home(env);
	int dir = 0;
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
		getcwd(buf, sizeof(buf));
		char *pwd = get_pwd(env);
		ft_strlcpy(buf, pwd, ft_strlen(buf));
	}
	return (1);
}
