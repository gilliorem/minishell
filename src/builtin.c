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
	char *home = NULL;
	for (int i = 0; env[i]; i++)
		if (strncmp(env[i], "HOME=", 5) == 0)
			home = ft_strchr(env[i], '/');
	return home;
}

char	*find_env_index(char **env, char *key)
{
	char *pwd = NULL;
	for (int i = 0; env[i]; i++)
		if (strncmp(env[i], key, 4) == 0) 
			pwd = ft_strchr(env[i], '/');
	return pwd;
}

/*
char old[PATH_MAX];
getcwd(old, sizeof(old)); // get current before cd

if (chdir(path_or_home) != 0)
    return (perror("chdir"), 0);

char new[PATH_MAX];
getcwd(new, sizeof(new)); // after cd

update_env_var(env, "OLDPWD", old);
update_env_var(env, "PWD", new);
*/

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
		// we did a chdir so lets now update our env;

		char *pwd = getcwd(buf, sizeof(buf));
		for (int i = 0; env[i]; i++)
		{
			if (strncmp("PWD=", env[i], 4) == 0)
				env[i] = ft_strdup(pwd);
		}
		printf("%s\n",pwd);
	}
	return (1);
}
