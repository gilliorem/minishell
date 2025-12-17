/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_putnbr_fd.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: regillio <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/16 20:22:22 by regillio          #+#    #+#             */
/*   Updated: 2025/05/16 20:22:40 by regillio         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
#include <fcntl.h>
#include <sys/stat.h>
*/
#include "libft.h"

void	ft_putnbr_fd(int n, int fd)
{
	if (n == -2147483648)
	{
		write(fd, "-2147483648", 11);
		return ;
	}
	if (n < 0)
	{
		ft_putchar_fd('-', fd);
		n *= -1;
	}
	if (n > 9)
		ft_putnbr_fd(n / 10, fd);
	ft_putchar_fd(n % 10 + '0', fd);
}
/*
int	main(void)
{
	int fd;
	fd = open("./putnbr_fd", O_CREAT, S_IRWXU);
	close(fd);
	fd = open("./putnbr_fd", O_WRONLY);
	if (fd != -1)
		ft_putnbr_fd(-1234, fd);
	close(fd);
	return (0);
}
*/
