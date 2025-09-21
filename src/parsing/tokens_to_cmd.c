/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokens_to_cmd.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vlorenzo <vlorenzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/18 16:44:09 by vlorenzo          #+#    #+#             */
/*   Updated: 2025/09/19 22:36:05 by vlorenzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell_exec.h"
#include "minishell_parsing.h"

/* Quita SOLO la capa externa que coincide con el tipo original del token */
static void	strip_outer_by_type(char *s, t_quote_type qt)
{
	size_t	len;

	if (!s)
		return ;
	len = ft_strlen(s);
	if (qt == SINGLE_QUOTE)
	{
		if (len >= 2 && s[0] == '\'' && s[len - 1] == '\'')
		{
			memmove(s, s + 1, len - 2);
			s[len - 2] = '\0';
		}
		return ;
	}
	if (qt == DOUBLE_QUOTE)
	{
		if (len >= 2 && s[0] == '\"' && s[len - 1] == '\"')
		{
			memmove(s, s + 1, len - 2);
			s[len - 2] = '\0';
		}
		return ;
	}
}

static int	is_arglike(t_tokens *t)
{
	return (t->type == COMMAND || t->type == ARG || t->type == OPTION
		|| t->type == PATH || t->type == SETTING);
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
/* … includes y strip_outer_by_type como ya lo tienes … */

/* firma cambiada: añadimos env y last_status */
/* … includes y strip_outer_by_type como ya lo tienes … */

/* firma cambiada: añadimos env y last_status */
static int	handle_redirs_and_heredoc(t_cmd *cmd, t_tokens **tokens, t_env *env,
		int last_status)
{
	char	*hd_path;
		char *delim;
		int quoted;

	if (((*tokens)->type == RED_OUT || (*tokens)->type == APPEND_OUT))
	{
		if (!(*tokens)->next)
			return (syntax_error("newline"), -1);
		strip_quotes_inplace((*tokens)->next->str);
		if (set_file_out(cmd, (*tokens)->next->str,
				((*tokens)->type == APPEND_OUT)) != 0)
			return (-1);
		*tokens = (*tokens)->next;
	}
	else if ((*tokens)->type == RED_IN)
	{
		if (!(*tokens)->next)
			return (syntax_error("newline"), -1);
		strip_quotes_inplace((*tokens)->next->str);
		if (set_file_in(cmd, (*tokens)->next->str) != 0)
			return (-1);
		*tokens = (*tokens)->next;
	}
	else if ((*tokens)->type == HEREDOC)
	{
		if (!(*tokens)->next)
			return (syntax_error("newline"), -1);
		/* delim: quitar SOLO la capa externa original */
		delim = ft_strdup((*tokens)->next->str);
		if (!delim)
			return (-1);
		strip_outer_by_type(delim, (*tokens)->next->quote_type);
		quoted = ((*tokens)->next->quote_type != NO_QUOTE);
		/* ahora pasamos env y last_status REALES */
		if (process_heredoc_runtime(delim, quoted, env, last_status) == -1)
			return (free(delim), -1);
		free(delim);
		/* tu path temporal (ajusta si tu API devuelve otro) */
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

/* firma cambiada: añadimos env y last_status */
static int	fill_cmd_args(t_cmd *cmd, t_tokens *t, t_env *env, int last_status)
{
	size_t	i;

	i = 0;
	while (t)
	{
		if (is_arglike(t))
		{
			if ((t->str && *t->str) || t->was_quoted)
			{
				strip_outer_by_type(t->str, t->quote_type);
				cmd->args[i] = ft_strdup(t->str);
				if (!cmd->args[i])
					return (-1);
				i++;
			}
		}
		else if (t->type == RED_IN || t->type == RED_OUT
			|| t->type == APPEND_OUT || t->type == HEREDOC)
		{
			if (is_invalid_redirection_sequence(t))
				return (syntax_error(t->next->str), -1);
			/* 👇 pasa env/last_status hacia heredoc */
			if (handle_redirs_and_heredoc(cmd, &t, env, last_status) == -1)
				return (-1);
		}
		t = t->next;
	}
	return (0);
}

/* firma cambiada: añadimos env/last_status y los pasamos a fill_cmd_args */
t_cmd	*tokens_to_cmd(t_tokens *tokens, t_env *env, int last_status)
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
	if (fill_cmd_args(cmd, tokens, env, last_status) == -1)
		return (free_cmd_list(cmd), NULL);
	if (!cmd->args[0] && !cmd->file_in && !cmd->file_out)
	{
		syntax_error("newline");
		free_cmd_list(cmd);
		return (NULL);
	}
	return (cmd);
}
