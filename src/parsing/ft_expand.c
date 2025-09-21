/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_expand.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vlorenzo <vlorenzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/03 20:35:00 by vlorenzo          #+#    #+#             */
/*   Updated: 2025/09/19 20:35:38 by vlorenzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "array_utils.h"
#include "minishell_exec.h"
#include "minishell_parsing.h"
#include "minishell_signals.h"

void	expand_tokens(t_tokens *head, t_env *env, int last_status)
{
	t_tokens	*t;
	char		*newstr;

	t = head;
	while (t)
	{
		if ((t->str && *t->str) || t->was_quoted)
		{
			if (t->quote_type != SINGLE_QUOTE)
			{
				newstr = expand_core(t->str, env, last_status);
				if (newstr)
				{
					free(t->str);
					t->str = newstr;
				}
			}
		}
		t = t->next;
	}
}
