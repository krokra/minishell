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

static int  handle_quotes(char *input, int *i, int *quote)
{
	if (input[*i] == '"' || input[*i] == '\'')
	{
		*quote = input[*i];
		(*i)++;
		while (input[*i] && input[*i] != *quote)
			(*i)++;
		return (input[*i] != '\0');
	}
	return (1);
}

static int	handle_operators(char *input, int *i)
{
	if (input[*i] == '>' || input[*i] == '<' || input[*i] == '|')
	{
		(*i)++;
		if (input[*i] == input[*i - 1])
			(*i)++;
		return (1);
	}
	return (0);
}

static int	handle_normal_word(char *input, int *i)
{
	while (input[*i] && input[*i] != ' ' && input[*i] != '\t'
		&& input[*i] != '"' && input[*i] != '\'' && input[*i] != '>'
		&& input[*i] != '<' && input[*i] != '|')
		(*i)++;
	return (1);
}

static int	handle_quotes_and_operators(char *input, int *i, int *quote)
{
	if (!handle_quotes(input, i, quote))
		return (0);
	if (!handle_operators(input, i))
		handle_normal_word(input, i);
	return (1);
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

char	*get_token(char *input, int *i, int *quote)
{
	int	start;
	int	current_quote;

	current_quote = 0;
	if (!skip_spaces(input, i))
		return (NULL);
	start = *i;
	if (!handle_quotes_and_operators(input, i, &current_quote))
		return (NULL);
	if (current_quote)
	{
		*quote = current_quote;
		(*i)++;
	}
	return (create_token_string(input, start, *i));
}
