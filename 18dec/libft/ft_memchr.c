/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memchr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: regillio <regillio@student.42singapore.sg  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/07 23:13:35 by regillio          #+#    #+#             */
/*   Updated: 2025/05/18 15:04:30 by regillio         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_memchr(const void *s, int c, size_t n)
{
	const unsigned char	*str;
	unsigned char		target;
	size_t				i;

	str = s;
	target = (unsigned char)c;
	i = 0;
	while (i < n)
	{
		if (*str == target)
			return ((void *)str);
		str++;
		i++;
	}
	return (NULL);
}
/*
int	main()
{
	char *dest;
	char s[] = "bonjour";

	dest = ft_memchr(s, 'j', 6);
	printf("%s\n", dest);
}
*/
