/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memmove.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: regillio <regillio@student.42singapore.sg  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/07 20:38:36 by regillio          #+#    #+#             */
/*   Updated: 2025/05/18 14:13:47 by regillio         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
//#include <string.h>

void	*ft_memmove(void *dest, const void *src, size_t n)
{
	const unsigned char	*s;
	unsigned char		*d;
	size_t				i;

	d = dest;
	s = src;
	if (n == 0 || dest == src)
		return (dest);
	if (d > s)
	{
		i = n;
		while (i > 0)
		{
			i--;
			d[i] = s[i];
		}
	}
	else
		ft_memcpy(dest, src, n);
	return (dest);
}
/*
int	main()
{
	char src[8] = "bonjour";
	char *dest = src + 3;
	memmove(dest, src, 3);
	printf("C LIB: %s\n", dest);
	
	char src2[8] = "bonjour";
	char *dest2 = src + 3;
	ft_memmove(dest2, src2, 3);
	printf("LIBFT: %s\n", dest2);
	
	return 0;
}
*/
