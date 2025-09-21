/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strjoin.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: regillio <regillio@student.42singapore.sg  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/07 20:49:08 by regillio          #+#    #+#             */
/*   Updated: 2025/05/19 18:53:42 by regillio         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strjoin(char const *s1, char const *s2)
{
	char	*str_join;
	size_t	str_join_len;
	int		i;

	str_join_len = ft_strlen(s1) + ft_strlen(s2);
	str_join = (char *)malloc((str_join_len + 1) * sizeof(char));
	if (!str_join)
		return (NULL);
	i = 0;
	while (s1[i])
	{
		str_join[i] = s1[i];
		i++;
	}
	i = 0;
	while (s2[i])
	{
		str_join[i + ft_strlen(s1)] = s2[i];
		i++;
	}
	str_join[ft_strlen(s1) + i] = '\0';
	return (str_join);
}
/*
int	main()
{
	char *str_join = ft_strjoin("bon", "jour");
	printf("%s\n", str_join);
}
*/
