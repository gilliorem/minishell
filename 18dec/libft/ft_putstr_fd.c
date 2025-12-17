/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_putstr_fd.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: regillio <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/16 19:46:43 by regillio          #+#    #+#             */
/*   Updated: 2025/05/23 21:19:58 by regillio         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
#include <fcntl.h> // open and close
#include <sys/stat.h> // S_IRWXU  
*/
#include "libft.h"

void	ft_putstr_fd(char *s, int fd)
{
	while (*s)
	{
		ft_putchar_fd(*s, fd);
		s++;
	}
}

/*
int	main(void)
{
	int fd;
	fd = open("./txt", O_CREAT, S_IRWXU);
	close(fd);
	fd = open("./txt", O_WRONLY);
	if (fd != -1)
		ft_putstr_fd("j'ecris dans un fichier depuis un autre fichier\n", fd);
	close(fd);
	return (0);
}
*/
