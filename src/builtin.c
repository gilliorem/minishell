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

//TODO so export with no argument list the env list and sort them in different order that env does. 
//it looks that it is alphabetical order but special characters are at the end.
//I wonder how env sorts it by default
//
//
//if the variable already exist, I need to overwrite it instead of adding a new one.
//need to upgrade the logic and actually check for the value of var
//in the case of overwriting it.


// char *var = REMI=BG
// var_values[0] = REMI
// var_values[1] = BG <<< overwrite this one.
// we find the var value.
// we compare it with the other var value
// or we can check if it is in env_list[i]
// if it is, we just re write the whole line.
//
//
//
// I need to have a list of the keys and a list the values
// for the env variables. it is going to make things so much simpler
// TODO: make a list of the env keys and the envs values (two separate lists)
// meanwhile, I will perform a split.
int	export_builtin(t_shell *shell, char *arg, char *var)
{
	int	i;
	char **var_values;
	char **env_values;

	i = 0;
	if (ft_strncmp(arg, "export", 7) != 0)
	{
		return (0);
	} 
	var_values= ft_split(var, '=');
	
	while (shell->env_list[i])
	{
		//printf("%s\n", shell->env_list[i]);
		env_values = ft_split(shell->env_list[i], '=');
		//printf("var_v0:[%s], var_v1:[%s]\n", var_values[0], var_values[1]);
		//printf("env0:[%s], env1:[%s]\n", env_values[0], env_values[1]);
		if (ft_strncmp(var_values[0], env_values[0], ft_strlen(env_values[0])) == 0)
		{
			printf("var_key[1]:[%s], env_key[1]:[%s]\n",var_values[0], env_values[0]);
			printf("%s\n", shell->env_list[i]);
			free(shell->env_list[i]);
			shell->env_list[i] = NULL;
			shell->env_list[i] = ft_strdup(var);
			return (1);
		}
		i++;
	}
	if (shell->env_list[i] == NULL)
	{
		printf("going in the NULL if\n");
		shell->env_list[i] = ft_strdup(var);
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
