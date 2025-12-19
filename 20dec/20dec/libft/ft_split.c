/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_split.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: regillio <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/20 21:08:30 by regillio          #+#    #+#             */
/*   Updated: 2025/05/20 21:10:25 by regillio         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	count_word(char const *s, char c)
{
	int	i;
	int	word_c;

	i = 0;
	word_c = 0;
	while (s[i])
	{
		if (s[i] != c)
		{
			word_c++;
			while (s[i] != c && s[i] != '\0')
				i++;
			i--;
		}
		i++;
	}
	return (word_c);
}

static char	*free_words(char **strs, char *word, int j)
{
	j--;
	while (j >= 0)
	{
		free(word);
		j--;
	}
	free(strs);
	return (NULL);
}

static char	*make_word(char const *s, char c, int i)
{
	char	*word;
	int		start;
	int		len;

	len = 0;
	start = i;
	while (s[i] != c && s[i] != '\0')
	{
		len++;
		i++;
	}
	i--;
	word = ft_substr(s, start, len);
	return (word);
}

char	**ft_split(char const *s, char c)
{
	char	**strs;
	int		i;
	int		j;
	int		word_c;

	j = 0;
	i = 0;
	word_c = count_word(s, c);
	strs = (char **)malloc((word_c + 1) * sizeof(char *));
	if (!strs)
		return (NULL);
	while (s[i])
	{
		if (s[i] != c)
		{
			strs[j] = make_word(s, c, i);
			i += ft_strlen(strs[j]) - 1;
			if (!strs[j])
				return (free_words(strs, strs[j], j), NULL);
			j++;
		}
		i++;
	}
	strs[j] = NULL;
	return (strs);
}
/*
int	main()
{
	char **words = ft_split("hello-world-salut-bonjour-", '-');	
	int i = 0;
	while (words[i] != NULL)
		printf("%s\n", words[i++]);
}
*/
