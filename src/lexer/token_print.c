/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token_print.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: psirault <psirault@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/30 07:59:01 by psirault          #+#    #+#             */
/*   Updated: 2025/06/09 15:53:11 by psirault         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/lexer.h"

void	print_tokens(t_token *tokens)
{
	t_token	*current;
	int		i;

	current = tokens;
	i = 0;
	while (current)
	{
		printf("Tkn %d: [%s] quote: [%c] space: [%d] type: [%d] index: [%d]\n",
			i++,
			current->content, current->quotes, current->has_space_after,
			current->type, current->index);
		current = current->next;
	}
}
