/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_token.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+        
	+:+     */
/*   By: psirault <psirault@student.42.fr>          +#+  +:+      
	+#+        */
/*                                                +#+#+#+#+#+  
	+#+           */
/*   Created: 2025/06/03 15:07:07 by psirault          #+#    #+#             */
/*   Updated: 2025/06/03 15:07:07 by psirault         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/lexer.h"

static int	skip_spaces(char *input, int *i)
{
	while (input[*i] && (input[*i] == ' ' || input[*i] == '\t'))
		(*i)++;
	return (input[*i] != '\0');
}

static int	is_token_delim(char c)
{
	return (c == ' ' || c == '\t' || c == '|' || c == '<' || c == '>');
}

static int	handle_quotes(char *input, int *i, t_token_state *state,
		int *quote)
{
	if (input[*i] == '\'' && input[*i + 1] == '\'')
	{
		*i += 2;
		state->start = *i;
		return (1);
	}
	state->in_quote = 1;
	state->quote_char = input[*i];
	*quote = state->quote_char;
	(*i)++;
	return (1);
}

static int	close_quote(char *input, int *i, t_token_state *state)
{
	if (state->in_quote && input[*i] == state->quote_char)
	{
		state->in_quote = 0;
		(*i)++;
		return (1);
	}
	return (0);
}

t_token	*get_token(char *input, int *i, int *quote)
{
	t_token_state	state;

	state.in_quote = 0;
	state.quote_char = 0;
	if (!skip_spaces(input, i))
		return (NULL);
	state.start = *i;
	if (is_token_delim(input[*i]))
		return (get_token_operator(input, i, quote, state.start));
	while (input[*i])
	{
		if (!state.in_quote && is_token_delim(input[*i]))
			return (get_token_general(input, i, quote, state.start));
		if (!state.in_quote && (input[*i] == '\"' || input[*i] == '\''))
		{
			if (*i > state.start)
				return (get_token_general(input, i, quote, state.start));
			if (handle_quotes(input, i, &state, quote))
				continue ;
		}
		if (close_quote(input, i, &state))
			break ;
		(*i)++;
	}
	return (get_token_end(input, i, quote, state.start));
}
