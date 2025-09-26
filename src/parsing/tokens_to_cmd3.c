/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokens_to_cmd3.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vlorenzo <vlorenzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/23 22:10:06 by vlorenzo          #+#    #+#             */
/*   Updated: 2025/09/24 21:38:22 by vlorenzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell_exec.h"
#include "minishell_parsing.h"

extern int	g_status;

int	append_arg(t_cmd *cmd, const char *arg, size_t *i)
{
	char	**newv;
	size_t	n;

	if (!cmd || !arg || !i)
		return (-1);
	n = 0;
	while (cmd->args && cmd->args[n])
		n++;
	newv = (char **)ft_calloc(n + 2, sizeof(char *));
	if (!newv)
		return (-1);
	if (n)
		ft_memcpy(newv, cmd->args, n * sizeof(char *));
	newv[n] = ft_strdup(arg);
	if (!newv[n])
		return (free(newv), -1);
	free(cmd->args);
	cmd->args = newv;
	*i = n + 1;
	return (0);
}

void	strip_quotes_inplace(char *s)
{
	size_t	i;
	size_t	j;
	char	quote;

	if (!s)
		return ;
	i = 0;
	j = 0;
	quote = 0;
	while (s[i])
	{
		if (!quote && (s[i] == '\'' || s[i] == '\"'))
			quote = s[i];
		else if (quote && s[i] == quote)
			quote = 0;
		else
		{
			s[j] = s[i];
			j++;
		}
		i++;
	}
	s[j] = '\0';
}

static int	push_arg_if_any(t_cmd *cmd, t_tokens *t, size_t *i)
{
	if ((t->str && *t->str) || t->was_quoted)
	{
		strip_outer_by_type(t->str, t->quote_type);
		cmd->args[*i] = ft_strdup(t->str);
		if (!cmd->args[*i])
			return (-1);
		(*i)++;
	}
	return (0);
}

static int	process_redir_token(t_cmd *cmd, t_tokens **t, t_env *env)
{
	int	rc;

	rc = handle_redirs_and_heredoc(cmd, t, env, g_status);
	if (rc == -2)
		return (-2);
	if (rc != 0)
		return (-1);
	return (0);
}

int	fill_cmd_args(t_cmd *cmd, t_tokens *t, t_env *env, int last_status)
{
	size_t	i;
	int		rc;

	(void)last_status;
	i = 0;
	while (t)
	{
		if (is_arglike(t))
		{
			if (append_arg(cmd, t->str, &i) != 0)
				return (-1);
		}
		else if (is_redir_tok(t))
		{
			rc = handle_redirs_and_heredoc(cmd, &t, env, g_status);
			if (rc != 0)
				return (rc);
			if (t)
				t = t->next;
			continue ;
		}
		t = t->next;
	}
	return (0);
}
