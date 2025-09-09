/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vlorenzo <vlorenzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/20 15:42:59 by dalabrad          #+#    #+#             */
/*   Updated: 2025/08/25 20:04:21 by vlorenzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell_exec.h"
#include "minishell_parsing.h"
#include "minishell_signals.h"

t_tokens	*get_tokens(void)
{
	static t_tokens	tokens;

	return (&tokens);
}

// MAIN PARSING ENVP VARIABLES FOR LATER EXPANSION
int	main(int argc, char **argv, char **envp)
{
	t_data	data;

	(void)argv;
	if (argc != 1)
	{
		printf("Too many arguments.\n");
		return (EXIT_FAILURE);
	}
	if (data_init(&data, envp))
	{
		printf("Failed to initialize data\n");
		return (EXIT_FAILURE);
	}
	main_loop(&data);
	free_data(&data);
	return (EXIT_SUCCESS);
}
