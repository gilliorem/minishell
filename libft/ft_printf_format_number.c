/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   format_number.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: regillio <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/16 10:04:28 by regillio          #+#    #+#             */
/*   Updated: 2025/06/16 10:28:12 by regillio         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_putnbr_unsigned(unsigned int nb)
{
	unsigned int	i;

	i = 0;
	if (nb >= 10)
	{
		i += ft_putnbr_unsigned(nb / 10);
		nb = nb % 10;
	}
	ft_putchar(nb + '0');
	i++;
	return (i);
}

int	ft_putnbr_hex(unsigned int nb)
{
	int	i;

	i = 0;
	if (nb >= 16)
	{
		i += ft_putnbr_hex(nb / 16);
		nb = nb % 16;
	}
	if (nb >= 10 && nb <= 16)
		ft_putchar(nb + ('A' - 10));
	else
		ft_putchar(nb + '0');
	i++;
	return (i);
}

int	ft_putnbr_hex_lowercase(unsigned int nb)
{
	int	i;

	i = 0;
	if (nb >= 16)
	{
		i += ft_putnbr_hex_lowercase(nb / 16);
		nb = nb % 16;
	}
	if (nb >= 10 && nb <= 16)
		ft_putchar(nb + ('a' - 10));
	else
		ft_putchar(nb + '0');
	i++;
	return (i);
}

int	ft_putnbr_hex_lowercase_ul(unsigned long nb)
{
	int	i;

	i = 0;
	if (nb >= 16)
	{
		i += ft_putnbr_hex_lowercase_ul(nb / 16);
		nb = nb % 16;
	}
	if (nb >= 10 && nb <= 16)
		ft_putchar(nb + ('a' - 10));
	else
		ft_putchar(nb + '0');
	i++;
	return (i);
}

int	ft_putnbr_ptr(void *ptr)
{
	int	i;

	i = 0;
	if (!ptr)
		return (ft_putstr("(nil)"));
	ft_putstr("0x");
	i += 2;
	i += ft_putnbr_hex_lowercase_ul((unsigned long)ptr);
	return (i);
}
