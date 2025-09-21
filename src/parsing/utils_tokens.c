/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_tokens.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vlorenzo <vlorenzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/05 18:32:39 by vlorenzo          #+#    #+#             */
/*   Updated: 2025/09/20 13:48:34 by vlorenzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell_exec.h"
#include "minishell_parsing.h"

/* -------------------------------------------------------------------------- */
/*  Quitar comillas externas in-place                                         */
/* -------------------------------------------------------------------------- */
void	strip_quotes_inplace(char *str)
{
	size_t	len;

	if (!str)
		return ;
	len = ft_strlen(str);
	if (len >= 2 && (str[0] == '\'' || str[0] == '\"') && str[len - 1] == str[0])
	{
		memmove(str, str + 1, len - 2);
		str[len - 2] = '\0';
	}
}

/* -------------------------------------------------------------------------- */
/*  Extrae un lexema desde *i (respeta comillas); marca was_quoted            */
/* -------------------------------------------------------------------------- */
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

	while (s[*i])
	{
		handle_quotes(s[*i], &in_s, &in_d, was_quoted);
		if (!in_s && !in_d && (s[*i] == ' ' || is_token_sep(s[*i])))
			break;
		(*i)++;
	}

	if (in_s || in_d)
		return (NULL);

	return (ft_substr(s, start, *i - start));
}

/* -------------------------------------------------------------------------- */
/*  Helpers para quote_type                                                   */
/* -------------------------------------------------------------------------- */
static inline t_quote_type	map_quote_type_from_int(int q)
{
	if (q == 2)
		return (SINGLE_QUOTE);
	if (q == 1)
		return (DOUBLE_QUOTE);
	return (NO_QUOTE);
}

/* Detecta comillas EXTERNAS (envolventes) si existen: '...' o "..." */
static int  detect_outer_quote_type(const char *s)
{
    size_t  len;

    if (!s)
        return 0;
    len = ft_strlen(s);
    if (len >= 2)
    {
        if (s[0] == '\'' && s[len - 1] == '\'')
            return 2; /* SINGLE_QUOTE */
        if (s[0] == '\"' && s[len - 1] == '\"')
            return 1; /* DOUBLE_QUOTE */
    }
    return 0;
}

/* 👉 NO usar was_quoted para el tipo; calcular SIEMPRE desde el string */
static void set_token_quote_flags(t_tokens *tok)
{
    int q;

    if (!tok || !tok->str)
        return ;
    q = detect_outer_quote_type(tok->str);
    tok->quote_type = map_quote_type_from_int(q);

	if (tok->str[0] == '\0' && q != 0)
        tok->was_quoted = 1;  /* asegura preservación de argumento vacío */
}


/* -------------------------------------------------------------------------- */
/*  MAIN TOKENIZER + EXPAND (creación de tokens)                              */
/* -------------------------------------------------------------------------- */
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
	new_tok->s_quoted = 0;
	new_tok->d_quoted = 0;
	new_tok->skip = 0;
	new_tok->next = NULL;

	/* 👇 clave: fijar quote_type para decidir expansión más adelante */
	set_token_quote_flags(new_tok);

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
