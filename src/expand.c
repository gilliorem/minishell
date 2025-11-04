#include "../include/minishell.h"

char *ft_strcpy_mohid(const char *src, int len)
{
    char *dest = malloc(len + 1);
    if (!dest) {
        perror("malloc");
        return NULL;
    }
    memcpy(dest, src, len);
    dest[len] = '\0';
    return dest;
}

char *exp_env_var(t_shell *shell, const char *input)
{
    int i = 1; 
    while (input[i] && (isalnum(input[i]) || input[i] == '_')) i++;
    char *var_name = ft_strcpy_mohid(input + 1, i - 1);
    if (!var_name) return NULL;
    char *var_value = getenv_builtin(shell, var_name);
    free(var_name);
    if (!var_value) return ft_strdup("");
    else return ft_strdup(var_value);
}
