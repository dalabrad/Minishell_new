/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   util_expands.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vlorenzo <vlorenzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/05 18:59:35 by vlorenzo          #+#    #+#             */
/*   Updated: 2025/09/19 22:17:06 by vlorenzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "array_utils.h"
#include "minishell_exec.h"
#include "minishell_parsing.h"

char	*ft_strjoin_free(char *s1, char *s2)
{
	char	*joined;

	if (!s1 && !s2)
		return (NULL);
	if (!s1)
		return (ft_strdup(s2));
	if (!s2)
	{
		free(s1);
		return (ft_strdup(""));
	}
	joined = ft_strjoin(s1, s2);
	free(s1);
	return (joined);
}

char	*ft_strjoin_char_free(char *s1, char c)
{
	char	str[2];

	str[0] = c;
	str[1] = '\0';
	return (ft_strjoin_free(s1, str));
}

char	*get_env_value_from_list(const char *name, t_env *env)
{
	while (env)
	{
		if (ft_strcmp(env->name, name) == 0)
			return (env->value);
		env = env->next;
	}
	return (NULL);
}

int	handle_exit_status(char **result, int last_status)
{
	char	*exit_code;

	exit_code = ft_itoa(last_status);
	if (!exit_code)
		return (1);
	*result = ft_strjoin_free(*result, exit_code);
	free(exit_code);
	return (0);
}

size_t	handle_variable(const char *str, size_t i, char **result, t_env *env)
{
	size_t	start;
	char	*var;
	char	*value;

	start = i;
	while (str[i] && (ft_isalnum(str[i]) || str[i] == '_'))
		i++;
	var = ft_substr(str, start, i - start);
	value = get_env_value_from_list(var, env);
	if (!value)
		value = "";
	*result = ft_strjoin_free(*result, value);
	free(var);
	return (i);
}

/*
** EXPANSIÓN NÚCLEO (respeta Bash):
** - Dentro de comillas simples '...'  → NO expande (literal)
** - Dentro de comillas dobles "..."  → SÍ expande
** - Sin comillas                      → SÍ expande
** - Soporta $? y $VAR
** - Si tras '$' no hay nombre válido → deja '$' literal
*/
char	*expand_core(const char *str, t_env *env, int last_status)
{
	size_t	i;
	int		sq; /* en comillas simples */
	int		dq; /* en comillas dobles */
	char	*out;

	if (!str)
		return (NULL);
	out = ft_strdup("");
	if (!out)
		return (NULL);
	i = 0;
	sq = 0;
	dq = 0;
	while (str[i])
	{
		/* toggles de comillas (no se copian) */
		if (str[i] == '\'' && !dq) { sq = !sq; i++; continue; }
		if (str[i] == '\"' && !sq) { dq = !dq; i++; continue; }

		/* ESCAPES con backslash */
		if (str[i] == '\\' && !sq)
		{
			if (dq)
			{
				/* en "..." solo escapan $, " y \ */
				if (str[i + 1] == '$' || str[i + 1] == '\"' || str[i + 1] == '\\')
				{
					out = ft_strjoin_char_free(out, str[i + 1]);
					if (!out) return (NULL);
					i += 2;
					continue;
				}
				/* backslash literal si no escapa especial */
				out = ft_strjoin_char_free(out, '\\');
				if (!out) return (NULL);
				i++;
				continue;
			}
			/* fuera de comillas: escapa el siguiente char si existe */
			if (str[i + 1])
			{
				out = ft_strjoin_char_free(out, str[i + 1]);
				if (!out) return (NULL);
				i += 2;
				continue;
			}
			/* '\' final sin siguiente char -> copiar tal cual */
			out = ft_strjoin_char_free(out, '\\');
			if (!out) return (NULL);
			i++;
			continue;
		}

		/* EXPANSIONES (nunca dentro de '...') */
		if (str[i] == '$' && !sq)
		{
			/* $? */
			if (str[i + 1] == '?')
			{
				i += 2;
				if (handle_exit_status(&out, last_status))
					return (free(out), NULL);
				continue;
			}
			/* $VAR */
			if (str[i + 1] && (ft_isalnum(str[i + 1]) || str[i + 1] == '_'))
			{
				i++; /* situarse en el 1er char del nombre */
				i = handle_variable(str, i, &out, env);
				continue;
			}
			/* $ suelto o seguido de no-válido -> copiar '$' */
			out = ft_strjoin_char_free(out, '$');
			if (!out) return (NULL);
			i++;
			continue;
		}

		/* carácter normal */
		out = ft_strjoin_char_free(out, str[i]);
		if (!out)
			return (NULL);
		i++;
	}
	return (out);
}