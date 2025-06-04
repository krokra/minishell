/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: psirault <psirault@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/19 12:11:14 by psirault          #+#    #+#             */
/*   Updated: 2025/06/04 21:19:39 by psirault         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LEXER_H
# define LEXER_H

# include <readline/history.h>
# include <readline/readline.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <unistd.h>
# include <stdbool.h>
# include "minishell.h"

typedef struct s_data	t_data;

typedef enum e_token_type
{
	T_WORD,
	T_PIPE,
	T_REDIR_IN,
	T_REDIR_OUT,
	T_APPEND,
	T_HEREDOC,
	T_ENVVAR
}					t_token_type;

typedef struct s_token
{
	char			*content;
	char			quotes;
	t_token_type	type;
	int				index;
	int				heredoc_pipe_read_fd;
	int				has_space_after;
	struct s_token	*next;
	struct s_token	*first;
	t_data			*data;
}					t_token;

typedef struct s_data
{
	int				exit_status;
	int				status_getter;
	t_token			*tokens;
	int				saved_stdout;
}					t_data;

typedef struct s_token_state
{
	int		in_quote;
	int		start;
	char	quote_char;
}					t_token_state;

char				*ft_strcat(char *dst, const char *src);
t_token				*lexer(char *input, int quote);
void				free_tokens(t_token *tokens);
void				print_tokens(t_token *tokens);
int					syntax_checker(t_token *tokens, t_data *data);
t_token				*create_token(char *content, char quote);
void				add_token(t_token **tokens, t_token *new);
t_token				*get_token(char *input, int *i, int *quote);
void				set_token_type(t_token *token);
void				quote_and_token_handling(char *line, int quote,
						t_data **data);
int					find_first_quote(const char *str);
void				set_index(t_token *tokens);
void				merge_tokens_without_space(t_token **tokens);
void				merge_adjacent_tokens(t_token **tokens);
void				replace_env_vars(t_token *tokens, char **envp,
						t_data *data);
void				remove_quotes_after_expansion(t_token *tokens);
t_token				*get_token_end(char *input, int *i, int *quote, int start);
t_token				*get_token_operator(char *input, int *i, int *quote,
						int start);
t_token				*get_token_general(char *input, int *i, int *quote,
						int start);
t_token				**split_tokens_by_pipe(t_token *tokens, int *count,
						int i, int n);
int					has_output_redirection(t_token *tokens);
int					is_append(t_token *tokens);
char				*get_output_filename(t_token *tokens);
char				**build_argv_from_tokens(t_token *cmd, int argc, int i);
int					get_heredoc_fd_from_segment(t_token *seg);

#endif