/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token_parser.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: psirault <psirault@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/30 07:59:04 by psirault          #+#    #+#             */
/*   Updated: 2025/05/30 17:02:41 by psirault         ###   ########.fr       */
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

t_token	*get_token(char *input, int *i, int *quote)
{
	int	start;
	int	in_quote = 0;
	char	quote_char = 0;
	t_token *token;

	printf("\n=== get_token ===\n");
	printf("Input: %s\n", input);
	printf("Position: %d\n", *i);

	if (!skip_spaces(input, i))
		return (NULL);
	start = *i;

	// Si c'est un opérateur, on le prend tout de suite
	if (input[*i] == '|' || input[*i] == '<' || input[*i] == '>')
	{
		printf("Found operator: %c\n", input[*i]);
		if (input[*i] == input[*i + 1]) // pour << ou >>
			(*i)++;
		(*i)++;
		*quote = 0;  // Pas de quote pour les opérateurs
		char *content = create_token_string(input, start, *i);
		printf("Created operator token: %s\n", content);
		t_token *token = create_token(content, 0);
		token->has_space_after = (input[*i] == ' ' || input[*i] == '\t');
		return token;
	}

	// Sinon, on lit un mot ou une séquence entre quotes
	while (input[*i])
	{
		if (!in_quote && (input[*i] == ' ' || input[*i] == '\t' ||
			input[*i] == '|' || input[*i] == '<' || input[*i] == '>'))
			break;
		if (!in_quote && (input[*i] == '"' || input[*i] == '\''))
		{
			printf("Found quote: %c\n", input[*i]);
			// Vérifier si c'est une séquence de guillemets vides
			if (*i > start)
			{
				char *content = create_token_string(input, start, *i);
				printf("Created token: %s (quote: %c)\n", content, *quote);
				token = create_token(content, *quote);
				return (token);
			}
			if (input[*i] == '\'' && input[*i + 1] == '\'')
			{
				// On ignore les guillemets simples vides et on continue avec le token suivant
				(*i) += 2;
				start = *i;  // On met à jour le début du token
				continue;
			}
			in_quote = 1;
			quote_char = input[*i];
			*quote = quote_char;  // On stocke le type de quote
			(*i)++;
			continue;
		}
		if (in_quote && input[*i] == quote_char)
		{
			printf("Closing quote: %c\n", quote_char);
			in_quote = 0;
			(*i)++;
			break;  // On coupe le token ici après la fermeture de la quote
		}
		(*i)++;
	}
	char *content = create_token_string(input, start, *i);
	printf("Created token: %s (quote: %c)\n", content, *quote);
	token = create_token(content, *quote);
	token->has_space_after = (input[*i] == ' ' || input[*i] == '\t');
	return token;
}
