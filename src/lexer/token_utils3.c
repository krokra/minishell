/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token_utils3.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: psirault <psirault@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/04 10:44:36 by psirault          #+#    #+#             */
/*   Updated: 2025/06/04 10:56:13 by psirault         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/lexer.h"

int	has_output_redirection(t_token *tokens)
{
	while (tokens)
	{
		if (tokens->type == T_REDIR_OUT || tokens->type == T_APPEND)
			return (1);
		tokens = tokens->next;
	}
	return (0);
}

int	is_append(t_token *tokens)
{
	while (tokens)
	{
		if (tokens->type == T_APPEND)
			return (1);
		tokens = tokens->next;
	}
	return (0);
}

char	*get_output_filename(t_token *tokens)
{
	while (tokens)
	{
		if ((tokens->type == T_REDIR_OUT || tokens->type == T_APPEND)
			&& tokens->next && tokens->next->type == T_WORD)
			return (tokens->next->content);
		tokens = tokens->next;
	}
	return (NULL);
}

t_token	**split_tokens_by_pipe(t_token *tokens, int *count, int i, int n)
{
	t_token	*tmp;
	t_token	**cmds;

	tmp = tokens;
	while (tmp)
	{
		if (tmp->type == T_PIPE)
			n++;
		tmp = tmp->next;
	}
	cmds = malloc((n + 1) * sizeof(t_token *));
	tmp = tokens;
	cmds[i++] = tmp;
	while (tmp)
	{
		if (tmp->type == T_PIPE && tmp->next)
			cmds[i++] = tmp->next;
		tmp = tmp->next;
	}
	cmds[i] = NULL;
	if (count)
		*count = n;
	return (cmds);
}
