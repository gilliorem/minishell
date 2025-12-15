/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strlcpy.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: regillio <regillio@student.42singapore.sg  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/07 20:55:31 by regillio          #+#    #+#             */
/*   Updated: 2025/05/17 11:26:02 by regillio         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
//#include <bsd/string.h>

size_t	ft_strlcpy(char *dest, const char *src, size_t size)
{
	size_t	i;

	i = 0;
	if (size != 0)
	{
		while (src[i] && i < size - 1)
		{
			dest[i] = src[i];
			i++;
		}
		dest[i] = '\0';
	}
	return (ft_strlen(src));
}
/*
int	main(void)
{
	char src[] = "bonjour";
	char src2[] = "bonjour";
	char dest[] = "";
	char dest2[] = "";
	size_t len = ft_strlcpy(dest, src, 0);
	size_t len2 = ft_strlcpy(dest2, src2, 0);

	printf("LIBFT:%ld\n", len);
	printf("CLIB:%ld\n", len2);
}
*/
