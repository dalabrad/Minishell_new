/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_expand.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dalabrad <dalabrad@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/03 20:35:00 by vlorenzo          #+#    #+#             */
/*   Updated: 2025/09/09 19:06:23 by dalabrad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "array_utils.h"
#include "minishell_exec.h"
#include "minishell_parsing.h"
#include "minishell_signals.h"

static int	is_name_char(int c)
{
	if (ft_isalnum(c))
		return (1);
	if (c == '_')
		return (1);
	return (0);
}

static int	append_status(char **out, int last_status)
{
	char	*num;

	num = ft_itoa(last_status);
	if (!num)
		return (1);
	*out = ft_strjoin_free(*out, num);
	free(num);
	if (!*out)
		return (1);
	return (0);
}

static int append_var(char **out, const char *s, t_env *env, size_t *i)
{
	size_t	start;
	char	*name;
	char	*val;

	if (!s[*i + 1]) // '$' al final de la cadena
	{
		*out = ft_strjoin_char_free(*out, '$');
		(*i)++;  // <- AVANZAR para evitar bucle infinito
		return (*out == NULL);
	}

	if (s[*i + 1] == '?') // este caso ya lo maneja expand_core, así que lo ignoramos aquí
		return (0);

	start = *i + 1;
	while (s[start] && is_name_char(s[start]))
		start++;

	if (start == *i + 1) // '$' no seguido de nombre válido
	{
		*out = ft_strjoin_char_free(*out, '$');
		(*i)++;  // <- AVANZAR
		return (*out == NULL);
	}

	name = ft_substr(s, *i + 1, start - (*i + 1));
	if (!name)
		return (1);

	val = get_env_value_from_list(name, env);
	free(name);

	if (!val)
		val = "";

	*out = ft_strjoin_free(*out, val);
	if (!*out)
		return (1);

	*i = start;
	return (0);
}

static char	*expand_core(const char *s, t_env *env, int last_status)
{
	size_t	i;
	int		sq;
	int		dq;
	char	*out;

	i = 0;
	sq = 0;
	dq = 0;
	out = ft_strdup("");
	if (!out)
		return (NULL);
	while (s[i])
	{
		if (s[i] == '\'' && !dq && (i++ || 1))
			sq = !sq;
		else if (s[i] == '\"' && !sq && (i++ || 1))
			dq = !dq;
		else if (s[i] == '$' && !sq && s[i + 1] == '?')
		{
			if (append_status(&out, last_status))
				return (free(out), NULL);
			i += 2;
		}
		else if (s[i] == '$' && !sq)
		{
			if (append_var(&out, s, env, &i))
				return (free(out), NULL);
		}
		else
		{
			out = ft_strjoin_char_free(out, s[i++]);
			if (!out)
				return (NULL);
		}
	}
	return (out);
}

/* Nota: ignoramos was_quoted anterior; hacerlo char-a-char resuelve casos
 * mixtos como foo"$USER"bar. Las comillas no se copian => ya salen strippeadas.
 */
char	*expand_variables(const char *s, t_env *env, int was_quoted, int status)
{
	char	*out;

	(void)was_quoted;
	out = expand_core(s, env, status);
	return (out);
}

void	expand_tokens(t_tokens *tks, t_env *env, int last_status)
{
	char	*new_s;

	while (tks)
	{
		if (tks->str && tks->type != ERROR)
		{
			new_s = expand_variables(tks->str, env, tks->was_quoted,
					last_status);
			if (!new_s)
				return ;
			free(tks->str);
			tks->str = new_s;
		}
		tks = tks->next;
	}
}
