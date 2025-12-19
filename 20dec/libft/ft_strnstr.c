/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strnstr.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: regillio <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/12 13:52:36 by regillio          #+#    #+#             */
/*   Updated: 2025/05/19 19:04:22 by regillio         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strnstr(const char *big, const char *little, size_t len)
{
	char	*big_c;
	char	*little_c;
	size_t	i;
	size_t	j;

	big_c = (char *)big;
	little_c = (char *)little;
	i = 0;
	if (little_c[0] == 0)
		return (big_c);
	while (big_c[i] != '\0' && i < len)
	{
		j = 0;
		while (big_c[i + j] == little_c[j] && i + j < len)
		{
			if (little_c[j + 1] == '\0')
			{
				return (&big_c[i]);
			}
			j++;
		}
		i++;
	}
	return (NULL);
}
