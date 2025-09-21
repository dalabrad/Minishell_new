/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_heredoc.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vlorenzo <vlorenzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/05 19:45:33 by vlorenzo          #+#    #+#             */
/*   Updated: 2025/09/19 22:19:03 by vlorenzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell_exec.h"
#include "minishell_parsing.h"
#include "minishell_parsing.h"
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

/* expansión para heredoc sin respetar comillas: expande $ y $? */
static char *expand_heredoc_line(const char *s, t_env *env, int last_status)
{
	size_t	i = 0;
	char	*out = ft_strdup("");

	if (!out) return NULL;
	while (s && s[i])
	{
		if (s[i] == '$')
		{
			if (s[i + 1] == '?')
			{
				i += 2;
				if (handle_exit_status(&out, last_status)) { free(out); return NULL; }
				continue;
			}
			if (s[i + 1] && (ft_isalnum(s[i + 1]) || s[i + 1] == '_'))
			{
				i++;
				i = handle_variable(s, i, &out, env);
				continue;
			}
			out = ft_strjoin_char_free(out, '$');
			if (!out) return NULL;
			i++;
			continue;
		}
		out = ft_strjoin_char_free(out, s[i]);
		if (!out) return NULL;
		i++;
	}
	return out;
}

static void	heredoc_child(const char *delim, int quoted, t_env *env, int last_status)
{
	char	*line;
	int		fd;

	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_IGN);

	fd = open("/tmp/1", O_CREAT | O_WRONLY | O_TRUNC, 0600);
	if (fd < 0)
		_exit(1);
	while (1)
	{
		line = readline("> ");
		if (!line || ft_strcmp(line, delim) == 0)
			break;
		if (!quoted)
		{
			char *exp = expand_heredoc_line(line, env, last_status);
			if (!exp) { free(line); close(fd); _exit(1); }
			write(fd, exp, ft_strlen(exp));
			write(fd, "\n", 1);
			free(exp);
		}
		else
		{
			write(fd, line, ft_strlen(line));
			write(fd, "\n", 1);
		}
		free(line);
	}
	free(line);
	close(fd);
	_exit(0);
}

int	process_heredoc_runtime(const char *delimiter, int quoted, t_env *env, int last_status)
{
	pid_t	pid;
	int		status;

	status = 0;
	pid = fork();
	if (pid < 0)
		return (error_msg(FORK_ERROR), -1);
	else if (pid == 0)
		heredoc_child(delimiter, quoted, env, last_status);
	else
		waitpid(pid, &status, 0);
	if (WIFSIGNALED(status))
		return (-1);
	return (0);
}
