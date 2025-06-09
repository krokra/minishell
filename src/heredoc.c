/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: psirault <psirault@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/30 18:19:55 by psirault          #+#    #+#             */
/*   Updated: 2025/06/09 18:15:07 by psirault         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

static char	*expand_line_heredoc(char *line, char **env, t_data *data)
{
	if (!line)
		return (NULL);
	if (ft_strchr(line, '$') == NULL)
		return (ft_strdup(line));
	return (replace_vars_in_str(NULL, line, env, data));
}

static int	processed_line_check(char *processed_line, int pipe_fds[2])
{
	if (!processed_line)
	{
		close(pipe_fds[0]);
		close(pipe_fds[1]);
		return (0);
	}
	if (write(pipe_fds[1], processed_line, ft_strlen(processed_line)) == -1)
	{
		perror("minishell: write to heredoc pipe");
		free(processed_line);
		close(pipe_fds[0]);
		close(pipe_fds[1]);
		return (0);
	}
	if (write(pipe_fds[1], "\n", 1) == -1)
	{
		perror("minishell: write newline to heredoc pipe");
		free(processed_line);
		close(pipe_fds[0]);
		close(pipe_fds[1]);
		return (0);
	}
	free(processed_line);
	return (1);
}

int	hdoc_loop(t_token *current_token, int pipe_fds[2], char **env, t_data *data)
{
	char	*input_line;
	char	*processed_line;
	char	*delimiter_str;

	current_token->heredoc_pipe_read_fd = pipe_fds[0];
	delimiter_str = current_token->next->content;
	while (1)
	{
		input_line = readline("> ");
		if (input_line == NULL)
		{
			close(pipe_fds[0]);
			close(pipe_fds[1]);
			return (-1);
		}
		if (ft_strcmp(input_line, delimiter_str) == 0)
		{
			free(input_line);
			break ;
		}
		processed_line = expand_line_heredoc(input_line, env, data);
		if (processed_line_check(processed_line, pipe_fds) == 0)
			return (-1);
	}
	return (0);
}

int	handle_heredocs(t_token *tokens, char **env, t_data *data)
{
	t_token	*current_token;
	int		pipe_fds[2];

	current_token = tokens;
	while (current_token)
	{
		if (current_token->type == T_HEREDOC)
		{
			if (current_token->next && current_token->next->type == T_WORD)
			{
				if (pipe(pipe_fds) == -1)
				{
					perror("minishell: pipe for heredoc");
					return (-1);
				}
				if (hdoc_loop(current_token, pipe_fds, env, data) == -1)
					return (-1);
				close(pipe_fds[1]);
			}
		}
		current_token = current_token->next;
	}
	return (0);
}
