/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token_syntax.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: psirault <psirault@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/30 10:08:55 by psirault          #+#    #+#             */
/*   Updated: 2025/04/30 10:08:55 by psirault         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

int	verify_syntax(t_token *tokens)
{
	t_token	*current;
	int		after_pipe = 0;

	current = tokens;
	if (tokens->type == T_PIPE)
		return (1);
	current = current->next;
	while (current)
	{
		if (current->type == T_PIPE)
		{
			after_pipe = 1;
			if (!current->next || current->next->type != T_WORD || current->next->type == T_ENVVAR)
				return (1);
		}
		if (after_pipe && current->type == T_HEREDOC)
			return (1);
		if ((current->type == T_REDIR_IN || current->type == T_REDIR_OUT || current->type == T_APPEND)
				&& (!current->next || (current->next->type != T_WORD && current->next->type != T_ENVVAR)))
			return (1);
		if (current->type == T_HEREDOC && (!current->next || (current->next->type != T_WORD && current->next->type != T_ENVVAR)))
			return (1);
		current = current->next;
	}
	return (0);
}

int	syntax_checker(t_token *tokens)
{
	if (tokens == NULL)
		return (1);
	if (verify_syntax(tokens))
	{
		printf("Syntax error\n");
		return (1);
	}
	return (0);
}
