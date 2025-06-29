/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: psirault <psirault@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/30 07:59:09 by psirault          #+#    #+#             */
/*   Updated: 2025/06/04 17:54:56 by psirault         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/lexer.h"

static char	**build_argv_loop(t_token *tmp, char **argv, int i)
{
	while (tmp && tmp->type != T_PIPE)
	{
		if (tmp->type == T_WORD || tmp->type == T_ENVVAR)
			argv[i++] = ft_strdup(tmp->content);
		if ((tmp->type == T_REDIR_OUT || tmp->type == T_APPEND
				|| tmp->type == T_REDIR_IN
				|| tmp->type == T_HEREDOC) && tmp->next)
			tmp = tmp->next;
		tmp = tmp->next;
	}
	argv[i] = NULL;
	return (argv);
}

char	**build_argv_from_tokens(t_token *cmd, int argc, int i)
{
	t_token	*tmp;
	char	**argv;

	tmp = cmd;
	while (tmp && tmp->type != T_PIPE)
	{
		if (tmp->type == T_WORD || tmp->type == T_ENVVAR)
			argc++;
		if ((tmp->type == T_REDIR_OUT || tmp->type == T_APPEND
				|| tmp->type == T_REDIR_IN
				|| tmp->type == T_HEREDOC) && tmp->next)
			tmp = tmp->next;
		tmp = tmp->next;
	}
	argv = malloc((argc + 1) * sizeof(char *));
	tmp = cmd;
	return (build_argv_loop(tmp, argv, i));
}

int	get_heredoc_fd_from_segment(t_token *segment)
{
	t_token	*current;
	int		last_fd;

	last_fd = -1;
	current = segment;
	while (current && current->type != T_PIPE)
	{
		if (current->type == T_HEREDOC)
			if (current->heredoc_pipe_read_fd != -1)
				last_fd = current->heredoc_pipe_read_fd;
		current = current->next;
	}
	return (last_fd);
}

void	remove_quotes_after_expansion(t_token *tokens)
{
	char	*stripped;

	while (tokens)
	{
		if (tokens->quotes == '\'')
		{
			stripped = remove_quotes(tokens->content);
			if (stripped)
			{
				free(tokens->content);
				tokens->content = stripped;
			}
		}
		else if (tokens->quotes == '"')
		{
			stripped = remove_quotes(tokens->content);
			if (stripped)
			{
				free(tokens->content);
				tokens->content = stripped;
			}
		}
		tokens = tokens->next;
	}
}
