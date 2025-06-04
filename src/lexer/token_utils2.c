/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token_utils2.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: psirault <psirault@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/04 10:43:47 by psirault          #+#    #+#             */
/*   Updated: 2025/06/04 17:09:09 by psirault         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/lexer.h"

void	set_token_type(t_token *token)
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
		token->type = T_HEREDOC;
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

	new = malloc(sizeof(t_token));
	if (!new)
		return (NULL);
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

void	merge_tokens_without_space(t_token **tokens)
{
	t_token	*cur;
	char	*merged;
	t_token	*to_free;

	cur = *tokens;
	while (cur && cur->next)
	{
		if (!cur->has_space_after && (cur->type == T_WORD || cur->quotes != 0)
			&& (cur->next->type == T_WORD || cur->next->quotes != 0))
		{
			merged = ft_strjoin(cur->content, cur->next->content);
			free(cur->content);
			cur->content = merged;
			cur->has_space_after = cur->next->has_space_after;
			to_free = cur->next;
			cur->next = to_free->next;
			free(to_free->content);
			free(to_free);
			continue ;
		}
		cur = cur->next;
	}
}
