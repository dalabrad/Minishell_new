/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_echo.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dalabrad <dalabrad@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/20 12:10:22 by dalabrad          #+#    #+#             */
/*   Updated: 2025/09/09 18:25:14 by dalabrad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell_exec.h"
#include "minishell_parsing.h"

int	shell_echo(char **args, t_data *data)
{
	int		i;
	bool	nl_flag;

	i = 0;
	nl_flag = true;
	(void)data;
	if (!args || !args[0])
		return (EXIT_SUCCESS);
	if (args[i] && !ft_strcmp("-n", args[i]))
	{
		nl_flag = false;
		i++;
	}
	while (args[i])
	{
		ft_putstr_fd(args[i], STDOUT_FILENO);
		if (args[i + 1])
			ft_putstr_fd(" ", STDOUT_FILENO);
		i++;
	}
	if (nl_flag)
		ft_putstr_fd("\n", STDOUT_FILENO);
	return (EXIT_SUCCESS);
}
