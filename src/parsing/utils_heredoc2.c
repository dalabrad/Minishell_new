/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_heredoc2.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vlorenzo <vlorenzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/23 18:53:52 by vlorenzo          #+#    #+#             */
/*   Updated: 2025/09/24 22:15:45 by vlorenzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell_exec.h"
#include "minishell_parsing.h"

extern int	g_status;

int	hd_make_tmp(int *fd, char **path_out)
{
	char	template_path[sizeof(HD_TEMPLATE)];

	*path_out = ft_strdup(template_path);
	if (!*path_out)
		return (-1);
	*fd = mkstemp(*path_out);
	if (*fd < 0)
	{
		free(*path_out);
		*path_out = NULL;
		return (-1);
	}
	return (0);
}

static void	hd_child_run(const char *delim, int quoted, t_env *env)
{
	char	*tmp_path;

	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_IGN);
	if (heredoc_loop_open(delim, quoted, env, &tmp_path) < 0)
		_exit(1);
	free(tmp_path);
	_exit(0);
}

static int	hd_parent_wait(pid_t pid, char **out_path)
{
	int	st;

	if (waitpid(pid, &st, 0) < 0)
		return (-1);
	if (WIFSIGNALED(st) && WTERMSIG(st) == SIGINT)
	{
		g_status = 130;
		return (1);
	}
	if (WIFEXITED(st) && WEXITSTATUS(st) != 0)
		return (-1);
	(void)out_path;
	return (0);
}

int	run_heredoc(const char *delim, int quoted, t_env *env, char **path_out)
{
	pid_t	pid;
	int		rc;

	*path_out = NULL;
	pid = fork();
	if (pid < 0)
		return (-1);
	if (pid == 0)
		hd_child_run(delim, quoted, env);
	rc = hd_parent_wait(pid, path_out);
	if (rc != 0)
		return (rc);
	if (heredoc_loop_open(delim, quoted, env, path_out) < 0)
		return (-1);
	return (0);
}
