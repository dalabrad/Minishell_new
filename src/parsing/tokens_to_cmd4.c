/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokens_to_cmd4.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vlorenzo <vlorenzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/24 19:18:14 by vlorenzo          #+#    #+#             */
/*   Updated: 2025/09/24 22:04:17 by vlorenzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell_exec.h"
#include "minishell_parsing.h"

static int	has_outer_of(char *s, char q)
{
	size_t	len;

	if (!s || !*s)
		return (0);
	len = ft_strlen(s);
	if (len < 2)
		return (0);
	if (s[0] == q && s[len - 1] == q)
		return (1);
	return (0);
}

int	strip_outer_by_type(char *s, int quote_type)
{
	size_t	i;
	size_t	j;
	char	q;
	size_t	len;

	if (!s || quote_type == 0)
		return (0);
	if (quote_type == 1)
		q = '\'';
	else
		q = '\"';
	if (!has_outer_of(s, q))
		return (0);
	len = ft_strlen(s);
	i = 1;
	j = 0;
	while (i + 1 < len)
	{
		s[j] = s[i];
		j++;
		i++;
	}
	s[j] = '\0';
	return (1);
}

int	set_file_out(t_cmd *cmd, const char *path, int append)
{
	char	*dup;

	if (!cmd || !path || !*path)
		return (-1);
	dup = ft_strdup(path);
	if (!dup)
		return (-1);
	if (cmd->file_out)
		free(cmd->file_out);
	cmd->file_out = dup;
	return (0);
}

int	set_file_in(t_cmd *cmd, const char *path)
{
	char	*dup;

	if (!cmd || !path || !*path)
		return (-1);
	dup = ft_strdup(path);
	if (!dup)
		return (-1);
	if (cmd->file_in)
		free(cmd->file_in);
	cmd->file_in = dup;
	return (0);
}
