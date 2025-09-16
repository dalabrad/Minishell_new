/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   one_builtin_with_redir.c                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vlorenzo <vlorenzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/28 19:05:30 by dalabrad          #+#    #+#             */
/*   Updated: 2025/09/16 00:40:53 by vlorenzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell_exec.h"
#include "minishell_parsing.h"

static int	save_stdio(int *in_bk, int *out_bk)
{
	*in_bk = dup(STDIN_FILENO);
	if (*in_bk < 0)
		return (-1);
	*out_bk = dup(STDOUT_FILENO);
	if (*out_bk < 0)
	{
		close(*in_bk);
		return (-1);
	}
	return (0);
}

/* Ejecuta un builtin aplicando redirecciones SIN salir de la shell en error */
void	one_builtin_with_redir(t_data *data, t_cmd *cmd)
{
	int	in_bk;
	int	out_bk;

	if (!cmd || !cmd->args || !cmd->args[0])
		return ;
	if (save_stdio(&in_bk, &out_bk) < 0)
	{
		data->last_status = 1;
		return ;
	}
	/* IN */
	if (cmd->file_in && file_in_redir(cmd) < 0)
	{
		data->last_status = 1;
		restore_stdio(in_bk, out_bk);
		return ;
	}
	/* OUT */
	if (cmd->file_out && file_out_redir(cmd) < 0)
	{
		data->last_status = 1;
		restore_stdio(in_bk, out_bk);
		return ;
	}
	data->last_status = run_builtin(cmd, data);
	restore_stdio(in_bk, out_bk);
}
