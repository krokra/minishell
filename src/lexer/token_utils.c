/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: psirault <psirault@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/30 07:59:09 by psirault          #+#    #+#             */
/*   Updated: 2025/04/30 10:08:42 by psirault         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/lexer.h"

void	set_token_type(t_token *token)
{
	if (token->quotes == '"' || token->quotes == '\'')
		token->type = T_WORD;
	else if (strcmp(token->content, "|") == 0)
		token->type = T_PIPE;
	else if (strcmp(token->content, "<") == 0)
		token->type = T_REDIR_IN;
	else if (strcmp(token->content, ">") == 0)
		token->type = T_REDIR_OUT;
	else if (strcmp(token->content, ">>") == 0)
		token->type = T_APPEND;
	else if (strcmp(token->content, "<<") == 0)
		token->type = T_HEREDOC;
	else if (strcmp(token->content, "$") == 0)
		token->type = T_ENVVAR;
	else
		token->type = T_WORD;
}

t_token	*create_token(char *content, char quote)
{
	t_token	*new;

	new = malloc(sizeof(t_token));
	if (!new)
		return (NULL);
	new->content = content;
	new->quotes = quote;
	new->next = NULL;
	set_token_type(new);
	return (new);
}

void	add_token(t_token **tokens, t_token *new)
{
	t_token *current;
	static int count;

	count = 0;
	if (!*tokens)
	{
		*tokens = new;
		new->index = count++;
		return ;
	}
	current = *tokens;
	while (current->next)
		current = current->next;
	current->next = new;
	new->index = count++;
}