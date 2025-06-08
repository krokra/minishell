/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token_parser.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: psirault <psirault@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/30 07:59:04 by psirault          #+#    #+#             */
/*   Updated: 2025/06/08 13:54:28 by psirault         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/lexer.h"

static char	*create_token_string(char *input, int start, int end)
{
	char	*token;
	int		len;
	int		j;

	len = end - start;
	j = 0;
	token = malloc(len + 1);
	if (!token)
		return (NULL);
	while (j < len)
	{
		token[j] = input[start + j];
		j++;
	}
	token[len] = '\0';
	return (token);
}

t_token	*get_token_operator(char *input, int *i, int *quote, int start)
{
	t_token		*token;
	char		*content;

	if (input[*i] == input[*i + 1])
		(*i)++;
	(*i)++;
	*quote = 0;
	content = create_token_string(input, start, *i);
	if (!content)
		return (NULL);
	token = create_token(content, *quote);
	if (!token)
	{
		if (content)
			free(content);
		return (NULL);
	}
	token->has_space_after = (input[*i] == ' ' || input[*i] == '\t');
	return (token);
}

t_token	*get_token_general(char *input, int *i, int *quote, int start)
{
	t_token		*token;
	char		*content;

	content = create_token_string(input, start, *i);
	if (!content)
		return (NULL);
	token = create_token(content, *quote);
	if (!token)
	{
		if (content)
			free(content);
		return (NULL);
	}
	while (input[*i] == ' ' || input[*i] == '\t')
	{
		token->has_space_after = 1;
		(*i)++;
	}
	return (token);
}

t_token	*get_token_end(char *input, int *i, int *quote, int start)
{
	t_token		*token;
	char		*content;

	if (start == *i)
		return (NULL);
	content = create_token_string(input, start, *i);
	if (!content)
		return (NULL);
	token = create_token(content, *quote);
	if (!token)
		return (NULL);
	while (input[*i] == ' ' || input[*i] == '\t')
		(*i)++;
	token->has_space_after = 0;
	*quote = 0;
	return (token);
}
