/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: psirault <psirault@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/30 07:58:59 by psirault          #+#    #+#             */
/*   Updated: 2025/05/07 12:05:50 by psirault         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/lexer.h"

t_token	*lexer(char *input, int quote)
{
	t_token	*tokens_head;
	t_token	*new_token;
	char	*content_str;
	int		i;

	tokens_head = NULL;
	i = 0;
	while (input[i])
	{
		content_str = get_token(input, &i, &quote);
		if (!content_str)
			break ;
		new_token = create_token(content_str, quote);
		if (!new_token)
		{
			free(content_str);
			free_tokens(tokens_head);
			return (NULL);
		}
		add_token(&tokens_head, new_token);
	}

	if (tokens_head)
	{
		t_token *current = tokens_head;
		while (current)
		{
			current->first = tokens_head;
			current = current->next;
		}
	}
	
	set_index(tokens_head);
	return (tokens_head);
}

void	free_tokens(t_token *tokens)
{
	t_token	*tmp;

	while (tokens)
	{
		tmp = tokens;
		tokens = tokens->next;
		free(tmp->content);
		free(tmp);
	}
}