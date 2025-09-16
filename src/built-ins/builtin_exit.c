/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_exit.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dalabrad <dalabrad@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/20 12:18:50 by dalabrad          #+#    #+#             */
/*   Updated: 2025/09/16 15:17:02 by dalabrad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell_exec.h"
#include "minishell_parsing.h"

int extern	g_status;

static int	ft_isspace(int c)
{
	if (c == ' ' || c == '\t' || c == '\n' || c == '\r'
		|| c == '\v' || c == '\f')
		return (c);
	return (0);
}

/* check_out_of_range:
*	Checks if the number goes over LONG_MAX or LONG_MIN.
*	Sets an error boolean to true if the number is out of range, false if not.
*/
static bool	check_out_of_range(int neg, unsigned long long num, bool *error)
{
	if ((neg == 1 && num > LONG_MAX)
		|| (neg == -1 && num > -(unsigned long)LONG_MIN))
		*error = true;
	return (*error);
}

/* ft_atoi_long:
*	Transforms a string comprised of digits into a long integer.
*	Returns the long integer. In case of error, sets an error boolean
*	to true.
*/
static int	ft_atoi_long(const char *str, bool *error)
{
	unsigned long long	num;
	int					neg;
	int					i;

	num = 0;
	neg = 1;
	i = 0;
	while (str[i] && ft_isspace(str[i]))
		i++;
	if (str[i] == '+')
		i++;
	else if (str[i] == '-')
	{
		neg *= -1;
		i++;
	}
	while (str[i] && ft_isdigit(str[i]))
	{
		num = (num * 10) + (str[i] - '0');
		if (check_out_of_range(neg, num, error))
			break ;
		i++;
	}
	return (num * neg);
}

/* get_exit_code:
*	Gets the exit code from the arguments given to the exit builtin.
*	Returns 0 if no arguments were provided.
*	Returns 2 in case argument is not digits.
*	Returns the numeric exit code on success.
*/
static int	get_exit_code(char *arg, bool *error)
{
	unsigned long long	i;

	if (!arg)
		return (1);
	i = 0;
	while (ft_isspace(arg[i]))
		i++;
	if (arg[i] == '\0')
		*error = true;
	if (arg[i] == '-' || arg[i] == '+')
		i++;
	if (!ft_isdigit(arg[i]))
		*error = true;
	while (arg[i])
	{
		if (!isdigit(arg[i]) && !ft_isspace(arg[i]))
			*error = true;
		i++;
	}
	i = ft_atoi_long(arg, error);
	return (i % 256);
}

int	shell_exit(char **args, t_data *data)
{
	int		exit_code;
	bool	error;

	error = false;
	if (data && (data->nbr_cmds == 1 || data->nbr_cmds == 0))
		printf("exit\n");
	if (!args || !args[0])
		exit_code = g_status;
	else
	{
		exit_code = get_exit_code(args[0], &error);
		if (error)
		{
			ft_putstr_fd("minishell: exit: numeric argument required\n", 2);
			exit_code = 2;
		}
		else if (args[1])
			return (ft_putstr_fd("minishell: exit: too many arguments\n",
					2), 1);
	}
	free_data(data);
	free_cmd_list(data->first_cmd);
	exit(exit_code);
	return (exit_code);
}
