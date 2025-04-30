/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token_print.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: psirault <psirault@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/30 07:59:01 by psirault          #+#    #+#             */
/*   Updated: 2025/04/30 08:10:51 by psirault         ###   ########.fr       */
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
		printf("Token %d: [%s] quote: [%c] type: [%d]\n", i++, current->content,
			current->quotes, current->type);
		current = current->next;
	}
}