/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexmain.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: psirault <psirault@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/30 07:58:57 by psirault          #+#    #+#             */
/*   Updated: 2025/06/08 12:40:19 by psirault         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/lexer.h"

int	find_first_quote(const char *str)
{
	int		i;
	int		found;
	char	quote;

	i = 0;
	found = 0;
	quote = 0;
	while (str[i])
	{
		if ((str[i] == '"' || str[i] == '\'') && !found)
		{
			quote = str[i];
			found = 1;
		}
		else if (str[i] == quote && found)
		{
			found = 0;
			quote = 0;
		}
		i++;
	}
	return (quote);
}

static int	is_quote_closed(const char *str, char quote)
{
	int	i;
	int	found;

	i = 0;
	found = 0;
	while (str[i])
	{
		if (str[i] == quote)
			found = !found;
		i++;
	}
	return (!found);
}

static char	*read_until_quote_closed(char *line, char *old_line, int *quote)
{
	char	*input;
	char	*tmp;

	old_line = line;
	while (*quote)
	{
		if (*quote == '"')
			input = readline("dquote> ");
		else
			input = readline("quote> ");
		tmp = malloc(ft_strlen(line) + ft_strlen(input) + 2);
		if (!tmp || !input)
		{
			if (input)
				free(input);
			if (line != old_line)
				free(line);
			return (NULL);
		}
		ft_memset(tmp, 0, ft_strlen(line) + ft_strlen(input) + 2);
		ft_memcpy(tmp, line, ft_strlen(line));
		tmp[ft_strlen(line)] = '\0';
		ft_strcat(tmp, input);
		free(input);
		if (line != old_line)
			free(line);
		line = tmp;
		if (is_quote_closed(line, *quote))
			*quote = 0;
	}
	return (line);
}

int	quote_and_token_handling(char *line, int quote, t_data **data)
{
	char	*result;

	result = NULL;
	if (quote)
	{
		result = read_until_quote_closed(line, line, &quote);
		if (!result)
			return (0);
		line = result;
	}
	(*data)->tokens = lexer(line, quote);
	print_tokens((*data)->tokens);
	if (!(*data)->tokens)
	{
		free(result);
		return (0);
	}
	if (result)
		free(result);
	return (1);
}
