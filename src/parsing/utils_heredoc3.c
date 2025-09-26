/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_heredoc3.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vlorenzo <vlorenzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/23 21:04:34 by vlorenzo          #+#    #+#             */
/*   Updated: 2025/09/24 20:58:02 by vlorenzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell_exec.h"
#include "minishell_parsing.h"

extern int	g_status;

int	ft_strappend(char **dest, const char *src)
{
	char	*newbuf;
	size_t	a;

	if (!dest)
		return (-1);
	if (!src || *src == '\0')
		return (0);
	if (!*dest)
	{
		*dest = ft_strdup(src);
		if (!*dest)
			return (-1);
		return (0);
	}
	a = ft_strlen(*dest);
	newbuf = (char *)ft_calloc(a + ft_strlen(src) + 1, 1);
	if (!newbuf)
		return (-1);
	ft_memcpy(newbuf, *dest, a);
	ft_memcpy(newbuf + a, src, ft_strlen(src));
	free(*dest);
	*dest = newbuf;
	return (0);
}

char	*expand_variables(const char *s, t_env *env, int g_status)
{
	return (expand_core(s, env));
}

char	*hd_add_nl(char *s)
{
	char	*out;
	size_t	len;

	len = ft_strlen(s);
	out = (char *)ft_calloc(len + 2, 1);
	if (!out)
		return (NULL);
	ft_memcpy(out, s, len);
	out[len] = '\n';
	return (out);
}

static int	hd_expand_status(char **p)
{
	char	*st;

	st = ft_itoa(g_status);
	if (!st)
		return (-1);
	if (ft_strappend(p, st) < 0)
		return (free(st), -1);
	free(st);
	return (0);
}

int	hd_expand_line(char **p, t_env *env)
{
	char	*expanded;

	expanded = expand_variables(*p, env, g_status);
	if (!expanded)
		return (-1);
	free(*p);
	*p = expanded;
	return (0);
}
