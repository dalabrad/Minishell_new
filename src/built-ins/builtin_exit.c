/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_exit.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vlorenzo <vlorenzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/20 12:18:50 by dalabrad          #+#    #+#             */
/*   Updated: 2025/09/07 23:33:29 by vlorenzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell_exec.h"
#include "minishell_parsing.h"

int	shell_exit(char **args, t_data *data)
{
	(void)args;
	if (data && (data->nbr_cmds == 1 || data->nbr_cmds == 0))
		printf("exit\n");
	free_data(data);
	free_cmd_list(data->first_cmd);
	exit(EXIT_SUCCESS);
	return (EXIT_SUCCESS);
}
