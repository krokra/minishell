#ifndef LEXER_H
# define LEXER_H

# include <readline/history.h>
# include <readline/readline.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <unistd.h>
# include "minishell.h"

typedef enum e_token_type
{
	T_WORD,      // mot
	T_PIPE,      // |
	T_REDIR_IN,  // <
	T_REDIR_OUT, // >
	T_APPEND,    // >>
	T_HEREDOC,   // <<
	T_ENVVAR     // $
}					t_token_type;

typedef struct s_token
{
	char			*content;
	char			quotes;
	t_token_type	type;
	int				index;
	struct s_token	*next;
}					t_token;

// Fonctions utils
char				*ft_strcat(char *dst, const char *src);

// Fonctions de tokenisation
t_token				*lexer(char *input, int quote);
void				free_tokens(t_token *tokens);
void				print_tokens(t_token *tokens);
int					syntax_checker(t_token *tokens);

// Fonctions utils token
t_token				*create_token(char *content, char quote);
void				add_token(t_token **tokens, t_token *new);
char				*get_token(char *input, int *i, int *quote);
void				set_token_type(t_token *token);
void				quote_and_token_handling(char *line, int quote, t_token **tokens);
int					find_first_quote(const char *str);
void				set_index(t_token *tokens);

#endif