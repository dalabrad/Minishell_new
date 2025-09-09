/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_heredoc.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dalabrad <dalabrad@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/05 19:45:33 by vlorenzo          #+#    #+#             */
/*   Updated: 2025/08/05 21:11:26 by dalabrad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell_exec.h"
#include "minishell_parsing.h"

static void	heredoc_child(const char *delim)
{
	char	*line;
	int		fd;

	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_IGN);
	fd = open("/tmp/1", O_CREAT | O_WRONLY | O_TRUNC, 0644);
	if (fd < 0)
		exit(1);
	while (1)
	{
		line = readline("> ");
		if (!line || ft_strcmp(line, delim) == 0)
			break ;
		write(fd, line, ft_strlen(line));
		write(fd, "\n", 1);
		free(line);
	}
	free(line);
	close(fd);
	exit(0);
}

int	process_heredoc_runtime(const char *delimiter)
{
	pid_t	pid;
	int		status;

	status = 0;
	pid = fork();
	if (pid < 0)
		return (error_msg(FORK_ERROR), -1);
	else if (pid == 0)
		heredoc_child(delimiter);
	else
		waitpid(pid, &status, 0);
	if (WIFSIGNALED(status))
		return (-1);
	return (0);
}
