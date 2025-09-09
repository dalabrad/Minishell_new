/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokens_to_cmd.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vlorenzo <vlorenzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/18 16:44:09 by vlorenzo          #+#    #+#             */
/*   Updated: 2025/09/07 19:28:33 by vlorenzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell_exec.h"
#include "minishell_parsing.h"

static int	is_invalid_redirection_sequence(t_tokens *token)
{
	if (!token || !token->next)
		return (0);
	if ((token->type == RED_IN || token->type == RED_OUT
			|| token->type == APPEND_OUT || token->type == HEREDOC)
		&& (token->next->type == RED_IN || token->next->type == RED_OUT
			|| token->next->type == APPEND_OUT || token->next->type == HEREDOC))
		return (1);
	return (0);
}

static int	count_args(t_tokens *tokens)
{
	int	count;

	count = 0;
	while (tokens)
	{
		if (tokens->type == COMMAND || tokens->type == ARG
			|| tokens->type == OPTION || tokens->type == PATH
			|| tokens->type == SETTING)
			count++;
		tokens = tokens->next;
	}
	return (count);
}

static int	handle_redirs_and_heredoc(t_cmd *cmd, t_tokens **tokens)
{
	if (((*tokens)->type == RED_OUT || (*tokens)->type == APPEND_OUT)
		&& (*tokens)->next)
	{
		cmd->file_out = ft_strdup((*tokens)->next->str);
		cmd->append_out = ((*tokens)->type == APPEND_OUT);
		*tokens = (*tokens)->next;
	}
	else if ((*tokens)->type == RED_IN && (*tokens)->next)
	{
		cmd->file_in = ft_strdup((*tokens)->next->str);
		*tokens = (*tokens)->next;
	}
	else if ((*tokens)->type == HEREDOC && (*tokens)->next)
	{
		if (process_heredoc_runtime((*tokens)->next->str) == -1)
			return (-1);
		cmd->file_in = ft_strdup("/tmp/1");
		*tokens = (*tokens)->next;
	}
	return (0);
}

static int	fill_cmd_args(t_cmd *cmd, t_tokens *tokens)
{
	size_t	idx;

	idx = 0;
	while (tokens)
	{
		if (tokens->type == COMMAND || tokens->type == ARG
			|| tokens->type == OPTION || tokens->type == PATH
			|| tokens->type == SETTING)
		{
			cmd->args[idx] = ft_strdup(tokens->str);
			if (!cmd->args[idx])
				return (-1);
			idx++;
		}
		else if (tokens->type == RED_IN || tokens->type == RED_OUT
			|| tokens->type == APPEND_OUT || tokens->type == HEREDOC)
		{
			if (handle_redirs_and_heredoc(cmd, &tokens) == -1)
				return (-1);
		}
		tokens = tokens->next;
	}
	return (0);
}

t_cmd	*tokens_to_cmd(t_tokens *tokens)
{
	t_cmd	*cmd;

	if (!tokens)
		return (NULL);
	cmd = new_cmd();
	if (!cmd)
		return (NULL);
	cmd->append_out = false;
	if (is_invalid_redirection_sequence(tokens))
		return (syntax_error(tokens->next->str), free_cmd_list(cmd), NULL);
	cmd->args = ft_calloc(count_args(tokens) + 1, sizeof(char *));
	if (!cmd->args)
		return (free_cmd_list(cmd), NULL);
	if (fill_cmd_args(cmd, tokens) == -1)
		return (free_cmd_list(cmd), NULL);
	if (!cmd->args[0])
		return (syntax_error("missing command after redirection"),
			free_cmd_list(cmd), NULL);
	return (cmd);
}
