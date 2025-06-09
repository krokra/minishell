/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token_utils2.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: psirault <psirault@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/04 10:43:47 by psirault          #+#    #+#             */
/*   Updated: 2025/06/09 17:46:43 by psirault         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/lexer.h"

int	set_token_2(t_token *token)
{
	if ((token->quotes == '"' || token->quotes == '\'')
		&& token->content[1] != '$')
		token->type = T_WORD;
	else if (ft_strncmp(token->content, "|", 1) == 0)
		token->type = T_PIPE;
	else if (ft_strncmp(token->content, "<", 2) == 0)
		token->type = T_REDIR_IN;
	else if (ft_strncmp(token->content, ">", 2) == 0)
		token->type = T_REDIR_OUT;
	else if (ft_strncmp(token->content, ">>", 3) == 0)
		token->type = T_APPEND;
	else if (ft_strncmp(token->content, "<<", 3) == 0)
	{
		token->type = T_HEREDOC;
		if (token->next && (token->next->quotes == '\''
				|| token->next->quotes == '"'))
			token->heredoc_delimiter_quoted = 1;
		else
			token->heredoc_delimiter_quoted = 0;
	}
	else
		return (0);
	return (1);
}

void	set_token_type(t_token *token)
{
	if (!token || !token->content)
		return ;
	if (set_token_2(token) == 1)
		return ;
	else if (ft_strncmp(token->content, "$", 1) == 0
		|| ft_strncmp(token->content, "\"$", 2) == 0
		|| ft_strncmp(token->content, "'$", 2) == 0)
		token->type = T_ENVVAR;
	else
		token->type = T_WORD;
}

t_token	*create_token(char *content, char quote)
{
	t_token	*new;

	if (!content || !*content)
		return (NULL);
	new = malloc(sizeof(t_token));
	if (!new)
		return (free(content), NULL);
	new->content = content;
	new->quotes = quote;
	new->heredoc_pipe_read_fd = -1;
	new->has_space_after = 0;
	new->next = NULL;
	set_token_type(new);
	return (new);
}

void	set_index(t_token *tokens)
{
	t_token	*current;
	int		i;

	current = tokens;
	i = 0;
	while (current)
	{
		current->index = i++;
		current = current->next;
	}
}

void	add_token(t_token **tokens, t_token *new)
{
	t_token	*current;

	if (!*tokens)
	{
		*tokens = new;
		new->index = 0;
		return ;
	}
	current = *tokens;
	while (current->next)
		current = current->next;
	current->next = new;
}
