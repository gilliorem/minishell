/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strrchr.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: regillio <regillio@student.42singapore.sg  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/07 20:42:52 by regillio          #+#    #+#             */
/*   Updated: 2025/05/07 23:09:10 by regillio         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strrchr(const char *s, int c)
{
	char	target;
	int		i;

	target = (char)c;
	s = (char *)s;
	i = ft_strlen(s);
	while (i >= 0)
	{
		if (s[i] == target)
			return ((char *)&s[i]);
		i--;
	}
	return (NULL);
}
