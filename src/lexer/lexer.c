/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: psirault <psirault@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/30 07:58:59 by psirault          #+#    #+#             */
/*   Updated: 2025/05/19 12:29:39 by psirault         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/lexer.h"

t_token	*lexer(char *input, int quote)
{
	printf("\n[LEXER] Début de la tokenisation\n");
	printf("[LEXER] Input: '%s'\n", input);
	printf("[LEXER] Quote: %d\n", quote);
	
	t_token	*tokens_head;
	t_token	*new_token;
	char	*content_str;
	int		i;

	tokens_head = NULL;
	i = 0;
	while (input[i])
	{
		printf("[LEXER] Position %d: '%c'\n", i, input[i]);
		content_str = get_token(input, &i, &quote);
		if (!content_str)
		{
			printf("[LEXER] ERREUR: get_token a échoué\n");
			break;
		}
		printf("[LEXER] Token extrait: '%s'\n", content_str);
		new_token = create_token(content_str, quote);
		if (!new_token)
		{
			printf("[LEXER] ERREUR: create_token a échoué\n");
			free(content_str);
			free_tokens(tokens_head);
			return (NULL);
		}
		printf("[LEXER] Token créé: type=%d, content='%s'\n", new_token->type, new_token->content);
		printf("[ADD_TOKEN] Ajout d'un nouveau token\n");
		printf("[ADD_TOKEN] Content: '%s'\n", new_token->content);
		printf("[ADD_TOKEN] Type: %d\n", new_token->type);
		add_token(&tokens_head, new_token);
		printf("[ADD_TOKEN] Token ajouté avec succès\n");
	}

	if (tokens_head)
	{
		printf("\n[LEXER] Liste des tokens générés:\n");
		t_token *current = tokens_head;
		while (current)
		{
			printf("[LEXER] Token: type=%d, content='%s'\n", current->type, current->content);
			current->first = tokens_head;
			current = current->next;
		}
	}
	
	set_index(tokens_head);
	printf("[LEXER] Fin de la tokenisation\n\n");
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