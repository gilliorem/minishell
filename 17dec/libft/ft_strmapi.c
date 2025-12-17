/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strmapi.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: regillio <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/16 17:09:06 by regillio          #+#    #+#             */
/*   Updated: 2025/07/03 15:42:38 by regillio         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strmapi(char const *s, char (*f)(unsigned int, char))
{
	unsigned int	i;
	char			*s1;

	s1 = ft_strdup(s);
	i = 0;
	if (!s1)
		return (NULL);
	while (s1[i])
	{
		s1[i] = f(i, s[i]);
		i++;
	}
	return (s1);
}
/*
char	add_one(unsigned int i, char c)
{
	return i + c;
}

int	main(void)
{
	char *s = ft_strmapi("1234", add_one);
	printf("%s\n", s);
	return (0);
}
*/
