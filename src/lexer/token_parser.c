/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token_parser.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: psirault <psirault@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/30 07:59:04 by psirault          #+#    #+#             */
/*   Updated: 2025/04/30 08:10:56 by psirault         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/lexer.h"

static int	skip_spaces(char *input, int *i)
{
	while (input[*i] && (input[*i] == ' ' || input[*i] == '\t'))
		(*i)++;
	return (input[*i] != '\0');
}

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

char	*get_token(char *input, int *i, int *quote __attribute__((unused)))
{
	int	start;
	int	in_quote = 0;
	char	quote_char = 0;

	if (!skip_spaces(input, i))
		return (NULL);
	start = *i;

	// Si c'est un opérateur, on le prend tout de suite
	if (input[*i] == '|' || input[*i] == '<' || input[*i] == '>')
	{
		if (input[*i] == input[*i + 1]) // pour << ou >>
			(*i)++;
		(*i)++;
		return (create_token_string(input, start, *i));
	}

	// Sinon, on lit un mot ou une séquence entre quotes
	while (input[*i])
	{
		if (!in_quote && (input[*i] == ' ' || input[*i] == '\t' ||
			input[*i] == '|' || input[*i] == '<' || input[*i] == '>'))
			break;
		if (!in_quote && (input[*i] == '"' || input[*i] == '\''))
		{
			in_quote = 1;
			quote_char = input[*i];
			(*i)++;
			continue;
		}
		if (in_quote && input[*i] == quote_char)
	{
			in_quote = 0;
			quote_char = 0;
			(*i)++;
			continue;
		}
		(*i)++;
	}
	return (create_token_string(input, start, *i));
}
