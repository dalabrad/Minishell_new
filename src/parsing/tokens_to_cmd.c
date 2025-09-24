/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokens_to_cmd.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vlorenzo <vlorenzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/18 16:44:09 by vlorenzo          #+#    #+#             */
/*   Updated: 2025/09/24 19:55:33 by vlorenzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell_exec.h"
#include "minishell_parsing.h"

extern int		g_status;

int	is_arglike(t_tokens *t)
{
	if (!t)
		return (0);
	return (t->type == COMMAND || t->type == ARG || t->type == OPTION
		|| t->type == PATH || t->type == SETTING);
}

static size_t	count_args(t_tokens *t)
{
	size_t	n;

	n = 0;
	while (t)
	{
		if (is_arglike(t))
		{
			if ((t->str && *t->str) || t->was_quoted)
				n++;
		}
		t = t->next;
	}
	return (n);
}

static int	is_invalid_redirection_sequence(t_tokens *t)
{
	if (!t || !t->next)
		return (0);
	if ((t->type == RED_IN || t->type == RED_OUT || t->type == APPEND_OUT
			|| t->type == HEREDOC) && (t->next->type == RED_IN
			|| t->next->type == RED_OUT || t->next->type == APPEND_OUT
			|| t->next->type == HEREDOC))
		return (1);
	return (0);
}

t_cmd	*tokens_to_cmd(t_tokens *tokens, t_env *env, int g_status)
{
	t_cmd	*cmd;
	size_t	nargs;

	if (!tokens)
		return (NULL);
	cmd = new_cmd();
	if (!cmd)
		return (NULL);
	cmd->append_out = false;
	cmd->file_in = NULL;
	cmd->file_out = NULL;
	nargs = count_args(tokens);
	cmd->args = (char **)ft_calloc(nargs + 1, sizeof(char *));
	if (!cmd->args)
		return (free_cmd_list(cmd), NULL);
	if (fill_cmd_args(cmd, tokens, env, g_status) == -1)
		return (free_cmd_list(cmd), NULL);
	if (!cmd->args[0] && !cmd->file_in && !cmd->file_out)
	{
		syntax_error("newline");
		free_cmd_list(cmd);
		return (NULL);
	}
	return (cmd);
}
