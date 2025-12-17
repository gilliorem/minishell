/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: regillio <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/16 10:02:41 by regillio          #+#    #+#             */
/*   Updated: 2025/07/14 20:13:02 by regillio         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static int	format_type(char c, va_list *ap)
{
	int	count;

	count = 0;
	if (c == 'c')
		count += ft_putchar((char)va_arg(*ap, int));
	else if (c == 's')
		count += ft_putstr(va_arg(*ap, char *));
	else if (c == 'i' || c == 'd')
		count += ft_putnbr(va_arg(*ap, int));
	else if (c == 'u')
		count += ft_putnbr_unsigned(va_arg(*ap, unsigned int));
	else if (c == 'X')
		count += ft_putnbr_hex(va_arg(*ap, unsigned int));
	else if (c == 'x')
		count += ft_putnbr_hex_lowercase(va_arg(*ap, unsigned int));
	else if (c == 'p')
		count += ft_putnbr_ptr(va_arg(*ap, void *));
	else if (c == '%')
		count += ft_putchar('%');
	else
		return (-1);
	return (count);
}

int	ft_printf(const char *format, ...)
{
	va_list	ap;
	int		i;
	int		count;

	va_start(ap, format);
	i = 0;
	count = 0;
	while (format[i])
	{
		if (format[i] == '%')
		{
			count += format_type(format[i + 1], &ap);
			i++;
		}
		else
		{
			ft_putchar(format[i]);
			count++;
		}
		i++;
	}
	va_end(ap);
	return (count);
}
