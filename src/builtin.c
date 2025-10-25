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
// home is /home/username

// cd with nothing bring us home
// I need to update PWD

char	*get_env_home(char **env)
{
	char *new_home = malloc(96);
	for (int i = 0; env[i]; i++)
		if (strncmp(env[i], "HOME", 4) == 0)
			new_home = ft_strchr(env[i], '/');
	printf("%s\n", new_home);
	return new_home;
}

char	*move_back(char **env)
{
	char *initial_wd = getenv("PWD");

}

void	cd_builtin(char **env, char *arg)
{
	if (arg)
		move_back();
	char *home = get_env_home(env);
	int change_dir = chdir(home);
	if (change_dir != 0)
		perror("chdir:");
	else
	{
		char buf[96];
		char *current_dir = getcwd(buf, sizeof(buf));
		printf("welcome home:[%s]\n", current_dir);
	}
}
