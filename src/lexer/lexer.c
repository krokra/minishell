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
	t_token	*tokens;
	t_token	*new;
	char	*content;
	int		i;

	tokens = NULL;
	i = 0;
	while (input[i])
	{
		content = get_token(input, &i, &quote);
		if (!content)
			break ;
		new = create_token(content, quote);
		if (!new)
		{
			free(content);
			free_tokens(tokens->first);
			return (NULL);
		}
		add_token(&tokens, new);
	}
	tokens->first = tokens;
	set_index(tokens);
	return (tokens);
}

void	free_tokens(t_token *tokens)
{
	t_token *tmp;

	while (tokens)
	{
		tmp = tokens;
		tokens = tokens->next;
		free(tmp->content);
		free(tmp);
	}
}