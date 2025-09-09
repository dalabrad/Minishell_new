/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   one_builtin_with_redir.c                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dalabrad <dalabrad@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/28 19:05:30 by dalabrad          #+#    #+#             */
/*   Updated: 2025/09/09 16:46:06 by dalabrad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell_exec.h"
#include "minishell_parsing.h"

static int	save_stdio(int *in_bk, int *out_bk)
{
	*in_bk = dup(STDIN_FILENO);
	*out_bk = dup(STDOUT_FILENO);
	if (*in_bk < 0 || *out_bk < 0)
		return (-1);
	return (0);
}

/* Ejecuta un builtin en el proceso padre aplicando redirecciones. */
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
	/* Aplica redirecciones (prototipos void según tu header) */
	if (cmd->file_in)
		file_in_redir(cmd);
	if (cmd->file_out)
		file_out_redir(cmd);
	/* Ejecuta el builtin directamente */
	data->last_status = run_builtin(cmd, data);
	/* Restaura STDIN/STDOUT */
	restore_stdio(in_bk, out_bk);
}
