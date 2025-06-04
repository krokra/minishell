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

int	verify_syntax_refactor(t_token *tokens, int after_pipe)
{
	while (tokens)
	{
		if (ft_strncmp(tokens->content, "||", 2) == 0)
			return (1);
		if (tokens->type == T_PIPE)
		{
			after_pipe = 1;
			if (!tokens->next || tokens->next->type != T_WORD
				|| tokens->next->type == T_ENVVAR)
				return (1);
		}
		if (after_pipe && tokens->type == T_HEREDOC)
			return (1);
		if ((tokens->type == T_REDIR_IN || tokens->type == T_REDIR_OUT
				|| tokens->type == T_APPEND)
			&& (!tokens->next || (tokens->next->type != T_WORD
					&& tokens->next->type != T_ENVVAR)))
			return (1);
		if (tokens->type == T_HEREDOC && (!tokens->next
				|| (tokens->next->type != T_WORD
					&& tokens->next->type != T_ENVVAR)))
			return (1);
		tokens = tokens->next;
	}
	return (0);
}

int	verify_syntax(t_token *tokens)
{
	t_token	*current;
	int		after_pipe;

	current = tokens;
	after_pipe = 0;
	if (tokens->type == T_PIPE)
		return (1);
	current = current->next;
	return (verify_syntax_refactor(current, after_pipe));
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
