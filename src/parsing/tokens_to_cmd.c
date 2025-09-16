/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokens_to_cmd.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vlorenzo <vlorenzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/18 16:44:09 by vlorenzo          #+#    #+#             */
/*   Updated: 2025/09/15 22:51:32 by vlorenzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell_exec.h"
#include "minishell_parsing.h"

/* redir seguida de otra redir -> error de sintaxis */
static int	is_invalid_redirection_sequence(t_tokens *t)
{
	if (!t || !t->next)
		return (0);
	if ((t->type == RED_IN || t->type == RED_OUT
			|| t->type == APPEND_OUT || t->type == HEREDOC)
		&& (t->next->type == RED_IN || t->next->type == RED_OUT
			|| t->next->type == APPEND_OUT || t->next->type == HEREDOC))
		return (1);
	return (0);
}

static int	count_args(t_tokens *t)
{
	int	n;

	n = 0;
	while (t)
	{
		if (t->type == COMMAND || t->type == ARG
			|| t->type == OPTION || t->type == PATH
			|| t->type == SETTING)
			n++;
		t = t->next;
	}
	return (n);
}

/* helpers: sobrescriben liberando lo previo (last wins) */
static int	set_file_in(t_cmd *cmd, const char *path)
{
	if (cmd->file_in)
		free(cmd->file_in);
	cmd->file_in = ft_strdup(path);
	return (cmd->file_in == NULL);
}

static int	set_file_out(t_cmd *cmd, const char *path, int append)
{
	if (cmd->file_out)
		free(cmd->file_out);
	cmd->file_out = ft_strdup(path);
	cmd->append_out = (append != 0);
	return (cmd->file_out == NULL);
}

/* procesa una redirección + su target; avanza *tokens al target */
static int	handle_redirs_and_heredoc(t_cmd *cmd, t_tokens **tokens)
{
	char	*hd_path;

	if (((*tokens)->type == RED_OUT || (*tokens)->type == APPEND_OUT))
	{
		if (!(*tokens)->next)
			return (syntax_error("newline"), -1);
		if (set_file_out(cmd, (*tokens)->next->str,
				((*tokens)->type == APPEND_OUT)) != 0)
			return (-1);
		*tokens = (*tokens)->next;
	}
	else if ((*tokens)->type == RED_IN)
	{
		if (!(*tokens)->next)
			return (syntax_error("newline"), -1);
		if (set_file_in(cmd, (*tokens)->next->str) != 0)
			return (-1);
		*tokens = (*tokens)->next;
	}
	else if ((*tokens)->type == HEREDOC)
	{
		if (!(*tokens)->next)
			return (syntax_error("newline"), -1);
		/* crea el tmp del heredoc; ajusta si tu API devuelve path distinto */
		if (process_heredoc_runtime((*tokens)->next->str) == -1)
			return (-1);
		hd_path = ft_strdup("/tmp/1");
		if (!hd_path)
			return (-1);
		if (set_file_in(cmd, hd_path) != 0)
			return (free(hd_path), -1);
		free(hd_path);
		*tokens = (*tokens)->next;
	}
	return (0);
}

static int	fill_cmd_args(t_cmd *cmd, t_tokens *t)
{
	size_t	i;

	i = 0;
	while (t)
	{
		if (t->type == COMMAND || t->type == ARG
			|| t->type == OPTION || t->type == PATH
			|| t->type == SETTING)
		{
			cmd->args[i] = ft_strdup(t->str);
			if (!cmd->args[i])
				return (-1);
			i++;
		}
		else if (t->type == RED_IN || t->type == RED_OUT
			|| t->type == APPEND_OUT || t->type == HEREDOC)
		{
			if (is_invalid_redirection_sequence(t))
				return (syntax_error(t->next->str), -1);
			if (handle_redirs_and_heredoc(cmd, &t) == -1)
				return (-1);
		}
		t = t->next;
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
	cmd->file_in = NULL;
	cmd->file_out = NULL;
	cmd->args = ft_calloc(count_args(tokens) + 1, sizeof(char *));
	if (!cmd->args)
		return (free_cmd_list(cmd), NULL);
	if (fill_cmd_args(cmd, tokens) == -1)
		return (free_cmd_list(cmd), NULL);
	/* No hay comando NI redirecciones -> error de sintaxis */
	if (!cmd->args[0] && !cmd->file_in && !cmd->file_out)
	{
		syntax_error("newline");
		free_cmd_list(cmd);
		return (NULL);
	}
	return (cmd);
}
