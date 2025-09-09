/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_tokens.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vlorenzo <vlorenzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/05 18:32:39 by vlorenzo          #+#    #+#             */
/*   Updated: 2025/08/06 18:12:50 by vlorenzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell_exec.h"
#include "minishell_parsing.h"

void	strip_quotes_inplace(char *str)
{
	size_t	i;
	size_t	j;
	char	quote;

	if (!str || (str[0] != '\'' && str[0] != '"'))
		return ;
	quote = str[0];
	i = 1;
	j = 0;
	while (str[i] && str[i] != quote)
		str[j++] = str[i++];
	str[j] = '\0';
}

char	*poly_substr(const char *s, size_t *i, int *was_quoted)
{
	size_t	start;
	bool	in_s;
	bool	in_d;

	start = *i;
	in_s = false;
	in_d = false;
	*was_quoted = 0;
	if (is_token_sep(s[*i]))
	{
		if ((s[*i] == '<' || s[*i] == '>') && s[*i + 1] == s[*i])
			*i += 2;
		else
			(*i)++;
		return (ft_substr(s, start, *i - start));
	}
	while (s[*i] && !(s[*i] == ' ' || is_token_sep(s[*i])))
	{
		handle_quotes(s[*i], &in_s, &in_d, was_quoted);
		(*i)++;
	}
	if (in_s || in_d)
		return (NULL);
	return (ft_substr(s, start, *i - start));
}

// MAIN TOKENIZER + EXPAND
t_tokens	*create_token(const char *input, size_t *k, size_t *i_words)
{
	t_tokens	*new_tok;

	new_tok = malloc(sizeof(t_tokens));
	if (!new_tok)
		return (NULL);
	new_tok->str = poly_substr(input, k, &new_tok->was_quoted);
	if (!new_tok->str)
	{
		free(new_tok);
		return (NULL);
	}
	new_tok->type = clasify_token(new_tok->str);
	new_tok->skip = 0;
	new_tok->next = NULL;
	(*i_words)++;
	return (new_tok);
}

t_tokens	*check_args_fixed(const char *input, size_t *i_words)
{
	t_tokens	*head;
	t_tokens	*curr;
	t_tokens	*new_tok;
	size_t		k;

	head = NULL;
	curr = NULL;
	k = 0;
	while (input[k])
	{
		while (input[k] == ' ')
			k++;
		if (!input[k])
			break ;
		new_tok = create_token(input, &k, i_words);
		if (!new_tok)
			return (free_tokens_list(head), NULL);
		if (!head)
			head = new_tok;
		else
			curr->next = new_tok;
		curr = new_tok;
	}
	set_command_type(head);
	return (head);
}
