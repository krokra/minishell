/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token_print.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: psirault <psirault@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/30 07:59:01 by psirault          #+#    #+#             */
/*   Updated: 2025/05/01 15:23:44 by psirault         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/lexer.h"

void	print_tokens(t_token *tokens)
{
	t_token *current = tokens;
	int i;
    i = 0;

	while (current)
	{
		printf("Token %d: [%s] quote: [%c] type: [%d] index: [%d]\n", i++, current->content,
			current->quotes, current->type, current->index);
		current = current->next;
	}
}