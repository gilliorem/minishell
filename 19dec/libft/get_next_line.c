/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: regillio <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/13 09:40:15 by regillio          #+#    #+#             */
/*   Updated: 2025/06/13 15:18:48 by regillio         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#define BUFFER_SIZE	1024
#include "libft.h"

/* initialize the static buffer for the first run and read
 * and append to the forming storage string (static_buf)
 * in the file until it finds a new line OR end of file
 * if there is a new line in the sbuf already, we process the s_buf
 * before reading again
 * add total bytes var to handle no new line ending file.
 * returns the number of bytes read.
 */

static int	read_file_until_newline(int fd, char *buf, char **static_buf)
{
	ssize_t	bytes_read;
	ssize_t	total_bytes;
	char	*tmp;

	bytes_read = 1;
	if (!*static_buf)
	{
		*static_buf = malloc(BUFFER_SIZE + 1);
		*static_buf[0] = '\0';
	}
	if (ft_strchr(*static_buf, '\n') != NULL)
		return (bytes_read);
	total_bytes = ft_strlen(*static_buf);
	while ((ft_strchr(buf, '\n') == NULL) && (bytes_read > 0))
	{
		bytes_read = read(fd, buf, BUFFER_SIZE);
		total_bytes += bytes_read;
		if (bytes_read <= 0)
			return (total_bytes);
		buf[bytes_read] = '\0';
		tmp = ft_strjoin(*static_buf, buf);
		*static_buf = tmp;
	}
	return (bytes_read);
}

/* exctract the line from the storage using 
 * substring until there is nothing left in the
 * storage
 * returns the extracted line 
 */

static char	*extract_line(char *static_buf)
{
	char	*line;
	int		i;

	i = 0;
	if (!static_buf || static_buf[0] == '\0')
		return (NULL);
	while (static_buf[i] != '\n' && static_buf[i])
		i++;
	if (static_buf[i] == '\n')
		i++;
	line = ft_substr(static_buf, 0, i);
	if (!line || line[0] == '\0')
	{
		if (line)
			free(line);
		free (static_buf);
		static_buf = NULL;
		return (NULL);
	}
	return (line);
}

/*  we make a copy to of the updated storage
 * so we can free the extracted-line.
 * returns the new storage. (static_buf)
 * I work with **, so I can point directly to *static_buf content.
 */

static char	*clean_static_buf(char **static_buf)
{
	char	*new_line_ptr;
	char	*new_static;

	new_line_ptr = ft_strchr(*static_buf, '\n');
	new_static = NULL;
	if (new_line_ptr != NULL)
	{
		new_line_ptr++;
		new_static = ft_strdup(new_line_ptr);
	}
	free (*static_buf);
	return (new_static);
}

/* Initilize the buffer we will store what
 * we attempt to read with the read()
 * returns the initialized buffer.
 */

char	*init_buf(char **buf)
{
	*buf = malloc(BUFFER_SIZE + 1);
	if (!*buf)
		return (NULL);
	else
		*buf[0] = '\0';
	return (*buf);
}

/* the beast */

char	*get_next_line(int fd)
{
	char		*line;
	char		*buf;
	int			bytes_read;
	static char	*static_buf;

	buf = init_buf(&buf);
	if (fd < 0 || BUFFER_SIZE <= 0)
	{
		free(buf);
		return (NULL);
	}
	bytes_read = read_file_until_newline(fd, buf, &static_buf);
	free(buf);
	if (bytes_read == 0 || bytes_read == -1 || !static_buf)
	{
		if (static_buf)
		{
			free(static_buf);
			static_buf = 0;
		}
		return (NULL);
	}
	line = extract_line(static_buf);
	static_buf = clean_static_buf(&static_buf);
	return (line);
}

/*
int	main()
{
	int fd = open("tester/files/41_with_nl", O_RDONLY );
	char *line = NULL;
	//while ((line = get_next_line(fd)))
	for (int i = 0; i < 20; i++)
	{
		line = get_next_line(fd);
		if (line)
		{
			printf("main>>> %s", line);
			free(line);
		}
	}
	return 0;
}
*/
