/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_atoi.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: regillio <regillio@student.42singapore.sg  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/07 20:17:51 by regillio          #+#    #+#             */
/*   Updated: 2025/05/18 14:47:06 by regillio         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_atoi(const char *nptr)
{
	int	minus_count;
	int	i;
	int	result;

	minus_count = 0;
	i = 0;
	result = 0;
	while ((nptr[i] == ' ') || ((nptr[i]) >= 9 && nptr[i] <= 13))
		i++;
	while (nptr[i] == '-' || nptr[i] == '+')
	{
		if (nptr[i] == '-')
			minus_count++;
		i++;
		break ;
	}
	while (nptr[i] >= '0' && nptr[i] <= '9')
	{
		result = result * 10 + nptr[i] - '0';
		i++;
	}
	if (minus_count > 0)
		result = result * (-1);
	return (result);
}
/*
int main()
{
	int result = ft_atoi("+-42");
	printf("%d\n", result);
}
*/
