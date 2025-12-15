/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strtrim.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: regillio <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/13 20:50:09 by regillio          #+#    #+#             */
/*   Updated: 2025/05/25 14:53:34 by regillio         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static int	is_set(char const *set, char c)
{
	int	i;

	i = 0;
	while (set[i] != '\0')
	{
		if (set[i] == c)
			return (1);
		i++;
	}
	return (0);
}

static int	count_set(char const *s1, char const *set)
{
	int	set_counter;
	int	i;

	set_counter = 0;
	i = 0;
	while (is_set(set, s1[i]) == 1)
	{
		set_counter++;
		i++;
	}
	return (set_counter);
}

static int	count_set_back(char const *s1, char const *set, int i, int s)
{
	while (is_set(set, s1[i]) == 1)
	{
		s++;
		i--;
	}
	return (s);
}

char	*ft_strtrim(char const *s1, char const *set)
{
	char		*str_trim;
	size_t		set_counter;
	int			start;
	size_t		i;
	size_t		j;

	i = 0;
	j = 0;
	set_counter = count_set(s1, set);
	i += set_counter;
	start = i;
	if (set_counter < ft_strlen(s1))
	{
		i = ft_strlen(s1) - 1;
		set_counter = count_set_back(s1, set, i, set_counter);
	}
	str_trim = (char *)malloc((ft_strlen(s1)- set_counter) + 1);
	if (!str_trim)
		return (NULL);
	i = 0;
	while (j < (ft_strlen(s1)- set_counter))
		str_trim[j++] = s1[start + i++];
	str_trim[j] = '\0';
	return (str_trim);
}

/*
int	main()
{
	ft_strtrim("--a--", "-");
}
*/
