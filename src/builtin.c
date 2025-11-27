#include "../include/minishell.h"

char	*getenv_builtin(t_shell *shell, char* env_key)
{
	int	i;
	char	**envs;
	char	*env_value;

	i = 0;
	env_value = NULL;
	while (shell->env_list[i])
	{
		envs = ft_split(shell->env_list[i], '=');
		if (ft_strncmp(envs[0], env_key, ft_strlen(env_key)) == 0)
		{
			env_value = envs[1];
			return (env_value);
		}
		i++;
	}
	return (env_value);
}

int	echo_builtin(char **args)
{
	int	argc;
	int	i;

	argc = 0;
	while (args[argc])
		argc++;
	if (argc < 2)
		return (0);
	if (ft_strncmp(args[0], "echo", 5) != 0)
		return (0);
	i = 1;
	if (ft_strncmp(args[1], "-n", 3) != 0)
	{
		while (args[i])
		{
			if (i == argc - 1)
				printf("%s", args[i]);
			else
				printf("%s ", args[i]);
			i++;
		}
		printf("\n");
		return (1);
	}
	i = 2;
	while (args[i])
	{
		if (i == argc - 1)
			printf("%s", args[i]);
		else
			printf("%s ", args[i]);
		i++;
	}
	return (1);
}

int	pwd_builtin()
{
	char buf[1096];
	char *wd = getcwd(buf, sizeof(buf));
	printf("%s\n",wd);
	return (1);
}

int	exit_builtin()
{
	return (EXIT_SUCCESS);
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

int	export_builtin(t_shell *shell, char **arg)
{
	int	i;
	int	j;
	char ***var_values;
	char **env_values;

	var_values = (char***)malloc(sizeof(char**));
	i = 0;
	if (ft_strncmp(arg[0], "export", 7) != 0)
	{
		return (0);
	} 
	while (arg[i])
	{
		var_values[i] = ft_split(arg[i], '=');
		i++;
	}
	i = 0;
	j = 0;
	while (shell->env_list[i])
	{
		env_values = ft_split(shell->env_list[i], '=');
		if (ft_strncmp(var_values[j][0], env_values[0], ft_strlen(env_values[0])) == 0)
		{
			free(shell->env_list[i]);
			shell->env_list[i] = NULL;
			shell->env_list[i] = ft_strdup(arg[j]);
			return (1);
		}
		i++;
		j++;
	}
	if (shell->env_list[i] == NULL)
	{
		shell->env_list[i] = ft_strdup(arg[j]);
		i++;
		shell->env_list[i] = NULL;
	}
	return (1);
}

// TODO: Replace the char *arg with shell->arg
// we also need a function that count the numbers of args/words the user pass an input
// need a "global function" that will run all our builtins.

int	unset_builtin(t_shell *shell, char **args)
{
	int	i;

	i = 0;
	if (args[1] == NULL || args[1][0] == '\0')
	{
		//printf("unset needs a value\n");
		return (0);
	}
	if (ft_strncmp(args[0], "unset", 6) != 0)
	{
		return (0);
	}
	while (shell->env_list[i])
	{
		if (ft_strncmp(shell->env_list[i], args[1], ft_strlen(args[1])) == 0)
		{
			free(shell->env_list[i]);
			shell->env_list[i] = NULL;
			printf("%s removed.\n", args[1]);
		}
		i++;
	}
	return (1);
}
