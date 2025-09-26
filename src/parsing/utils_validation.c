/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_validation.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vlorenzo <vlorenzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/06 17:41:37 by vlorenzo          #+#    #+#             */
/*   Updated: 2025/09/23 18:46:03 by vlorenzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell_exec.h"
#include "minishell_parsing.h"

int	has_invalid_pipe_usage(const char *s)
{
	int		i;
	bool	pipe_found;

	i = 0;
	pipe_found = false;
	while (s[i])
	{
		if (s[i] == '|')
		{
			if (pipe_found || i == 0 || !s[i + 1] || s[i + 1] == '|')
				return (1);
			pipe_found = true;
		}
		else if (s[i] != ' ')
			pipe_found = false;
		i++;
	}
	return (0);
}

static size_t	take_operator(const char *s, size_t i, char **out)
{
	char	op[2];

	if (s[i] == '>' && s[i + 1] == '>')
	{
		*out = ft_strdup(">>");
		return (2);
	}
	if (s[i] == '<' && s[i + 1] == '<')
	{
		*out = ft_strdup("<<");
		return (2);
	}
	if (s[i] == '>' || s[i] == '<' || s[i] == '|')
	{
		op[0] = s[i];
		op[1] = '\0';
		*out = ft_strdup(op);
		return (1);
	}
	*out = NULL;
	return (0);
}
