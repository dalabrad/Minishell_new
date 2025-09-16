/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_expand.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vlorenzo <vlorenzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/03 20:35:00 by vlorenzo          #+#    #+#             */
/*   Updated: 2025/09/15 20:13:53 by vlorenzo         ###   ########.fr       */
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

// Prototipo sugerido en .h:
// int append_var(char **out, const char *s, t_env *env, size_t *i);

static int	append_var(char **out, const char *s, t_env *env, size_t *i)
{
	size_t		start;
	char		*name;
	const char	*raw;
	char		*val;

	/* '$' al final → no añade nada (echo $ => línea vacía) */
	if (!s[*i + 1])
	{
		(*i)++;
		return (0);
	}
	/* Si lo siguiente NO es nombre válido ni '?' → no añade nada */
	if (!(ft_isalpha((unsigned char)s[*i + 1]) || s[*i + 1] == '_') && s[*i + 1] != '?')
	{
		(*i)++;
		return (0);
	}

	/* $VAR */
	start = *i + 1;
	while (s[start] && (ft_isalnum((unsigned char)s[start]) || s[start] == '_'))
		start++;

	/* si no hay nombre tras '$' (p.ej. '$ ') → no añadir */
	if (start == *i + 1)
	{
		(*i)++;
		return (0);
	}

	name = ft_substr(s, *i + 1, start - (*i + 1));
	if (!name)
		return (1);

	raw = get_env_value_from_list(name, env);
	free(name);

	/* duplica SIEMPRE para gestionar memoria homogénea */
	val = ft_strdup(raw ? raw : "");
	if (!val)
		return (1);

	*out = ft_strjoin_free(*out, val);
	free(val);
	if (!*out)
		return (1);

	/* deja *i en el primer carácter tras el nombre, para que el bucle siga con lo que venga (p.ej. 'yes' en $USERyes) */
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
		if (s[i] == '\'' && !dq)               /* abrir/cerrar ' ... ' */
		{
			sq = !sq;
			i++;
		}
		else if (s[i] == '\"' && !sq)          /* abrir/cerrar " ... " */
		{
			dq = !dq;
			i++;
		}
		else if (s[i] == '$' && !sq)           /* expansión solo fuera de comillas simples */
		{
			/* $$ -> PID */
			if (s[i + 1] == '$')
			{
				char *pid = ft_itoa(getpid());
				if (!pid)
					return (free(out), NULL);
				out = ft_strjoin_free(out, pid);
				free(pid);
				if (!out)
					return (NULL);
				i += 2;
			}
			/* $? -> último status */
			else if (s[i + 1] == '?')
			{
				i += 2;
				if (append_status(&out, last_status))
					return (free(out), NULL);
			}
			/* $VAR / $... (delegamos a append_var) */
			else
			{
				if (append_var(&out, s, env, &i))
					return (free(out), NULL);
			}
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
