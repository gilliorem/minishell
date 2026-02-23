/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_substr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: regillio <regillio@student.42singapore.sg  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/09 11:05:20 by regillio          #+#    #+#             */
/*   Updated: 2025/07/03 15:47:00 by regillio         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_substr(char const *s, unsigned int start, size_t len)
{
	char	*substr;
	size_t	s_len;
	size_t	subs_len;
	size_t	i;
	size_t	j;

	s_len = ft_strlen(s);
	if (start >= s_len)
		ft_strdup("");
	i = 0;
	j = 0;
	subs_len = ft_strlen(s) - start;
	if (subs_len > len)
		subs_len = len;
	substr = (char *)malloc((subs_len + 1) * sizeof(char));
	if (!substr)
		return (NULL);
	while (s[start + i] && i < len)
	{
		substr[j] = s[start + i];
		i++;
		j++;
	}
	substr[j] = '\0';
	return (substr);
}
/*
int	main()
{
	char *sub = ft_substr("tripouille", 0, 42000);
	printf("%s\n", sub);
	printf("%ld\n", ft_strlen(sub)+1);
}
*/
