/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbariol- <nassimbariol@student.42.fr>>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/27 08:16:26 by psirault          #+#    #+#             */
/*   Updated: 2025/06/09 13:22:54 by nbariol-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/lexer.h"
#include "../../includes/minishell.h"
#include "../../includes/pipex.h"

void	close_all_except_std(void)
{
	int	fd;

	fd = 3;
	while (fd < 1024)
	{
		close(fd);
		fd++;
	}
}

static int	is_stdout_redirected(t_token *tokens)
{
	t_token	*current;

	current = tokens;
	while (current && current->type != T_PIPE)
	{
		if ((current->type == T_REDIR_OUT || current->type == T_APPEND)
			&& current->next && current->next->type == T_WORD)
		{
			return (1);
		}
		current = current->next;
	}
	return (0);
}

void	handle_stdout_redirection(t_token *current)
{
	char	*filename;
	int		flags;
	int		fd;

	filename = current->next->content;
	if (current->type == T_APPEND)
		flags = O_WRONLY | O_CREAT | O_APPEND;
	else
		flags = O_WRONLY | O_CREAT | O_TRUNC;
	fd = open(filename, flags, 0644);
	if (fd == -1)
	{
		perror("minishell: open");
		exit(1);
	}
	if (dup2(fd, STDOUT_FILENO) == -1)
	{
		perror("minishell: dup2");
		close(fd);
		exit(1);
	}
	close(fd);
}

static int	handle_pipe_redirections(t_token *tokens, int *prev_pipe_read,
		char **env, t_data *data)
{
	t_token	*current;

	current = tokens;
	while (current && current->type != T_PIPE)
	{
		if (current->type == T_REDIR_IN && current->next)
		{
			if (handle_input_redirection(current) == -1)
			{
				cleanup(NULL, env, data->tokens, data);
				return (-1);
			}
			if (prev_pipe_read && *prev_pipe_read != -1)
			{
				close(*prev_pipe_read);
				*prev_pipe_read = -1;
			}
		}
		else if ((current->type == T_REDIR_OUT || current->type == T_APPEND)
			&& current->next && current->next->type == T_WORD)
			handle_stdout_redirection(current);
		current = current->next;
	}
	return (0);
}

static t_token	*find_command_start_from_segment(t_token *current_segment_token)
{
	t_token	*current;

	current = current_segment_token;
	while (current != NULL && current->type != T_PIPE)
	{
		if (current->type == T_REDIR_IN || current->type == T_REDIR_OUT
			|| current->type == T_APPEND || current->type == T_HEREDOC)
		{
			if (current->next != NULL && current->next->type == T_WORD)
			{
				current = current->next->next;
				continue ;
			}
			else
				return (NULL);
		}
		else if (current->type == T_WORD || current->type == T_ENVVAR)
			return (current);
		else
			return (NULL);
	}
	return (NULL);
}
