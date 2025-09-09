/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   command_exec.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vlorenzo <vlorenzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/13 12:38:55 by dalabrad          #+#    #+#             */
/*   Updated: 2025/09/08 08:04:53 by vlorenzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell_exec.h"
#include "minishell_parsing.h"

static void	exec_external(t_cmd *cmd, t_data *data)
{
	char	*path;

	path = resolve_exec_path(cmd, data);
	if (!path)
		return (err_cmd_not_found(cmd->args[0], data));
	if (check_exec_preflight(path, data))
		return (free(path));
	run_execve_handle(path, cmd, data);
	free(path);
}

void	command_exec(t_cmd *cmd, t_data *data)
{
	if (!cmd || !cmd->args || !cmd->args[0])
	{
		data->last_status = 0;
		return ;
	}
	 if (is_builtin(cmd->args[0]))
		data->last_status = run_builtin(cmd, data);
	else
		exec_external(cmd, data);
}