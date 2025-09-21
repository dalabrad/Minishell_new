/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell_parsing.h                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vlorenzo <vlorenzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/13 10:21:52 by dalabrad          #+#    #+#             */
/*   Updated: 2025/09/20 15:22:59 by vlorenzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef	MINISHELL_PARSING_H
# define MINISHELL_PARSING_H

//////////////////////////////////
// HEADERS FROM INCLUDED LIBRARIES
//////////////////////////////////

# include "array_utils.h"
# include "libft.h"
# include "minishell_exec.h"
# include "minishell_signals.h"
# include <dirent.h>
# include <fcntl.h>
# include <limits.h>
# include <readline/history.h>
# include <readline/readline.h>
# include <signal.h>
# include <stdbool.h>
# include <stddef.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <sys/stat.h>
# include <sys/types.h>
# include <sys/wait.h>
# include <unistd.h>

//////////////////////////////////
// COLOURS
//////////////////////////////////
#define C_RST  "\033[0m"
#define C_YEL  "\033[38;5;226m"
#define C_ORG  "\033[38;5;214m"
#define C_RED  "\033[38;5;196m"
#define C_GRY  "\033[38;5;245m"
#define C_WHT  "\033[1;38;5;231m"
#define C_CYAN "\033[38;5;51m"
#define C_PINK "\033[38;5;218m"
#define C_LAV "\033[38;5;183m"
#define C_TTL  "\033[1;38;5;231m"

//////////////////////////////////
//-----MINISHELL PROMTP----------
//////////////////////////////////

# define PROMPT "minishell>>"

//////////////////////////////////
//-----STRUCTURES----------------
//////////////////////////////////

typedef struct s_cmd	t_cmd;
typedef struct s_data	t_data;
typedef struct s_tokens	t_tokens;
typedef struct s_env	t_env;

// ENUM TOKENS STRUCT
typedef enum t_TokenType
{
	RED_IN,
	RED_OUT,
	HEREDOC,
	APPEND_OUT,
	OPTION,
	COMMAND,
	SETTING,
	PATH,
	ARG,
	ERROR
}						t_TokenType;

// PIPES
typedef struct s_pipes
{
	char				*str;
	size_t				index;
	struct s_pipes		*next;
}						t_pipes;

// QUOTE TYPE
typedef enum e_quote_type
{
	NO_QUOTE = 0,
	DOUBLE_QUOTE,
	SINGLE_QUOTE
}	t_quote_type;

// TOKENS
typedef struct s_tokens
{
	int             was_quoted;
	int             s_quoted;
	int             d_quoted;
	int             skip;
	char           *str;
	t_TokenType     type;
	t_quote_type    quote_type;
	struct s_tokens *next;
}	t_tokens;

// COMMS
typedef struct s_comms
{
	t_tokens			*token;
	int					i;
	struct s_comms		*next;
}						t_comms;

// MINI-SPLIT STRUCT
typedef struct s_split
{
	char				**split;
	const char			*s;
	char				c;
}						t_split;

// STRUCTURA DE CLEANUP ARGS
typedef struct s_cleanup_args
{
	char				**segments;
	t_tokens			**tokens;
	size_t				count;
}						t_cleanup_args;

//////////////////////////////////
//-----------FUNCTIONS----------//
//////////////////////////////////

// UTILS INIT & HANDLE
t_tokens				*new_token(char *str);
t_cmd					*new_cmd(void);
int						handle_token_alloc_fail(char **segments, char *line);
t_pipes					*init_struct(t_pipes *args);
int						init_pipe_segments(char *line, char ***segments,
							size_t *n);
t_tokens				**init_tokens_by_segment(size_t count);
void					print_history(void);

// UTILS CLEAN STRUCTS
int						is_exit_command(char **line, t_data *data);
t_pipes					*clean_struct(t_pipes *args);
void					free_tokens_list(t_tokens *head);
void					cleanup(char **segments, t_tokens **tokens, size_t n);
void					free_cmd_list(t_cmd *cmd);

// FT-MINI-SPLIT
size_t					splitted_len(const char *s, char c);
char					**split2array(const char *s, char c, char **array,
							size_t w_count);
char					**ft_minisplit(const char *s, char c, size_t *n);
size_t					count_splitted(char *s, char c);
size_t					quoted_field_len(const char *s, char c);

// UTILS PARSING
int						is_path(const char *str);
int						is_path(const char *str);
int						ft_lstadd_front2(t_pipes **lst, t_pipes *new);
int						ft_lstadd_front2(t_pipes **lst, t_pipes *new);
const char				*skip_space(const char *s);
size_t					is_open(const char *s);
int						is_escaped(const char *s, int i, int in_single);
void					update_quote_state(char c, int *in_s, int *in_d,
							int prev_escape);

// FT_CLASIFY TOKENS
t_TokenType				clasify_token(const char *str);
void					set_command_type(t_tokens *tokens);
const char				*token_type_str(t_TokenType type);
char					*poly_substr(const char *s, size_t *i, int *was_quoted);
t_tokens				*check_args_fixed(const char *input, size_t *i_words);
int						is_token_sep(char c);
void					handle_quotes(char c, bool *in_s, bool *in_d,
							int *quoted);

// UTILS_TOKENS
t_tokens				*create_token(const char *input, size_t *k,
							size_t *i_words);
char					*poly_substr(const char *s, size_t *i, int *was_quoted);
t_tokens				*check_args_fixed(const char *input, size_t *i_words);
void					strip_quotes_inplace(char *str);

// PROCESS BY SEGMENT OR PIPE
void					process_segments(char **segments, t_tokens **tokens,
							size_t n, t_data *data);

// TOK_TO_CMD
t_cmd 					*tokens_to_cmd(t_tokens *tokens, t_env *env, int last_status);

// FT_EXPAND
void					expand_tokens(t_tokens *head, t_env *env, int last_status);

// UTILS EXPAND
char					*ft_strjoin_free(char *s1, char *s2);
char					*ft_strjoin_char_free(char *s1, char c);
char					*get_env_value_from_list(const char *name, t_env *env);
int						handle_exit_status(char **result, int last_status);
size_t					handle_variable(const char *str, size_t i,
							char **result, t_env *env);
char					*expand_core(const char *str, t_env *env, int last_status);

// UTILS HEREDOC
int						process_heredoc_runtime(const char *delimiter, int quoted, t_env *env, int last_status);


// GET TOKENS EVERYWHERE
t_tokens				*get_tokens(void);

#endif