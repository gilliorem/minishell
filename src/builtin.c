#include "../include/minishell.h"


/* Even if this is not asked, we are
 * going to need our own getenv function.
 * in order to properly handle the cases
 * like echo $USER - unset USER - echo $USER.
 * because right now getenv function depends
 * on the original env list, we need to pass
 * our own copy of the list as an new arg. */

/* we will go through our own env list
 * 	look if the var name match the env_var_name
 * 	if it does, we return that value
 * 	else we return NULL
 */

/* I think I do need to split the list like I did in unset*/
// if the env_key match with the env_key list, then return it
//
// char *home = getenv(HOME)
// first we check if HOME is a key in our shellenvlist
// >>		/home/regillio
// si ca match
// we want to get the value at this key

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
		env_values = ft_split(shell->env_list[i], '=');
		if (ft_strncmp(var_values[0], env_values[0], ft_strlen(env_values[0])) == 0)
		{
			free(shell->env_list[i]);
			shell->env_list[i] = NULL;
			shell->env_list[i] = ft_strdup(var);
			return (1);
		}
		i++;
	}
	if (shell->env_list[i] == NULL)
	{
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
