/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dalabrad <dalabrad@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/20 15:42:59 by dalabrad          #+#    #+#             */
/*   Updated: 2025/09/21 11:41:31 by dalabrad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell_exec.h"
#include "minishell_parsing.h"
#include "minishell_signals.h"

//////////////////////////////////
//------------BANNER-------------
//////////////////////////////////
/* MINI (ASCII) */
static const char	*MINI_ASCII[5] = {
	C_ORG " ##   ##  ###  ##   ##  ###        " C_RST,
	C_ORG " ### ###   #   ###  ##   #         " C_RST,
	C_ORG " ## # ##   #   #### ##   #         " C_RST,
	C_ORG " ##   ##   #   ## ####   #         " C_RST,
	C_ORG " ##   ##  ###  ##   ##  ###        " C_RST
};

/* SHELL (ASCII) */
static const char *SHELL_ASCII[5] = {
	C_CYAN " █████  █   █  █████  █      █     " C_RST,
	C_CYAN " █      █   █  █      █      █     " C_RST,
	C_CYAN " █████  █████  █████  █      █     " C_RST,
	C_CYAN "     █  █   █  █      █      █     " C_RST,
	C_CYAN " █████  █   █  █████  █████  █████ " C_RST
};

/* WAVES */
static const char *FLAME[5] = {
	C_LAV  " )" C_RST,
	C_CYAN "( " C_RST,
	C_WHT  " )" C_RST,
	C_YEL  "( " C_RST,
	C_ORG  " )" C_RST
};

static const char *FLAME2[5] = {
	C_ORG  "( " C_RST,
	C_YEL  " )" C_RST,
	C_PINK "( " C_RST,
	C_LAV  " )" C_RST,
	C_WHT  "( " C_RST
};

/* PRINT FUNCTIONS BANNER */
void	print_mini42_banner(void)
{
	int i;

	ft_putstr("\n");
	for (i = 0; i < 5; ++i)
	{
		ft_putstr((char *)FLAME[i]);
		ft_putstr((char *)FLAME2[i]);
		ft_putstr((char *)FLAME[i]);
		ft_putstr((char *)MINI_ASCII[i]);
		ft_putstr((char *)FLAME2[i]);
		ft_putstr((char *)FLAME[i]);
		ft_putstr((char *)FLAME2[i]);
		ft_putstr((char *)"\n");
	}
}

/* PRINT FUNCTIONS BANNER */
void	print_boom_banner(void)
{
	for (int i = 0; i < 5; ++i)
	{
		ft_putstr((char *)FLAME[i]);
		ft_putstr((char *)FLAME2[i]);
		ft_putstr((char *)FLAME[i]);
		ft_putstr((char *)SHELL_ASCII[i]);
		ft_putstr((char *)FLAME[i]);
		ft_putstr((char *)FLAME2[i]);
		ft_putstr((char *)FLAME[i]);
		ft_putstr((char *)"\n");
	}
}
//////////////////////////////////
//----------END BANNER-----------
//////////////////////////////////
/* AUX TOKENS */
t_tokens	*get_tokens(void)
{
	static t_tokens	tokens;

	return (&tokens);
}

//////////////////////////////////
//------------MAIN---------------
//////////////////////////////////
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
	print_mini42_banner();
	print_boom_banner();
	ft_putstr((char *)"\n");
	main_loop(&data);
	free_data(&data);
	return (EXIT_SUCCESS);
}
//////////////////////////////////
//----------END MAIN-------------
//////////////////////////////////