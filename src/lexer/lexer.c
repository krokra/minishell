/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: psirault <psirault@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/30 07:58:59 by psirault          #+#    #+#             */
/*   Updated: 2025/06/04 10:33:25 by psirault         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/lexer.h"

t_token	*lexer(char *input, int quote)
{
	t_token	*tokens_head;
	t_token	*token;
	t_token	*current;
	int		i;

	tokens_head = NULL;
	i = 0;
	while (input[i])
	{
		token = get_token(input, &i, &quote);
		if (!token)
			break ;
		add_token(&tokens_head, token);
	}
	if (tokens_head)
	{
		current = tokens_head;
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
