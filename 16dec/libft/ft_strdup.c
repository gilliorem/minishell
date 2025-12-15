/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strdup.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: regillio <regillio@student.42singapore.sg  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/07 20:46:16 by regillio          #+#    #+#             */
/*   Updated: 2025/07/03 15:39:08 by regillio         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strdup(const char *s)
{
	char	*dup_src;
	size_t	s_len;

	s_len = ft_strlen(s);
	dup_src = malloc(sizeof(char) * s_len + 1);
	if (!dup_src)
		return (NULL);
	ft_memcpy(dup_src, s, s_len);
	dup_src[s_len] = '\0';
	return (dup_src);
}

/*
int main()
{
	char *b = NULL;
	char *a = "cc";
	ft_strdup_(a);
}
*/
