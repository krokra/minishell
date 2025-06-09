/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token_utils3.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: psirault <psirault@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/04 10:44:36 by psirault          #+#    #+#             */
/*   Updated: 2025/06/09 17:46:47 by psirault         ###   ########.fr       */
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
	if (!cmds)
		return (NULL);
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

void	merge_tokens_without_space(t_token **tokens)
{
	t_token	*cur;
	char	*merged;
	t_token	*to_free;

	cur = *tokens;
	while (cur && cur->next)
	{
		if (!cur->has_space_after && ((cur->type == T_WORD || cur->quotes != 0
					|| cur->type == T_ENVVAR) && (cur->next->type == T_WORD
					|| cur->next->quotes != 0 || cur->next->type == T_ENVVAR)))
		{
			merged = ft_strjoin(cur->content, cur->next->content);
			free(cur->content);
			cur->content = merged;
			cur->has_space_after = cur->next->has_space_after;
			to_free = cur->next;
			cur->next = to_free->next;
			free(to_free->content);
			free(to_free);
			continue ;
		}
		cur = cur->next;
	}
}
