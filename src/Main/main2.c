/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main2.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbariol- <nassimbariol@student.42.fr>>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/09 12:53:24 by nbariol-          #+#    #+#             */
/*   Updated: 2025/06/09 12:54:48 by nbariol-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

void	handle_out_redirections(t_data *data, t_append *append, t_token *redir)
{
	while (redir)
	{
		append->last_redir = 0;
		if (redir->type == T_APPEND || redir->type == T_REDIR_OUT)
		{
			redirection_exec(data, append, redir);
			if (append->last_redir == 1)
				return ;
		}
		redir = redir->next;
	}
}

void	handle_failed_redir(char *str, t_data *data, t_append *append)
{
	if (append->redir_applied && data->saved_stdout != -1)
		close(data->saved_stdout);
	free(str);
	free_tokens(data->tokens->first);
	data->tokens = NULL;
}
void	restore_stdout(t_data *data, t_append *append, int is_builtin)
{
	if (append->redir_applied && data->saved_stdout != -1)
	{
		dup2(data->saved_stdout, STDOUT_FILENO);
		close(data->saved_stdout);
		if (is_builtin)
			data->saved_stdout = -1;
	}
}

void	execute_simple_redirection(char *str, char **envp, t_data *data,
		t_append *append)
{
	t_token	*redir;

	data->saved_stdout = -1;
	append->redir_applied = 0;
	redir = data->tokens->first;
	handle_out_redirections(data, append, redir);
	if (append->last_redir < 0)
	{
		handle_failed_redir(str, data, append);
		return ;
	}
	if (handle_builtins(envp, data->tokens->first, data))
		restore_stdout(data, append, 1);
	else
	{
		restore_stdout(data, append, 0);
		exec_cmd_tokens(data, envp);
	}
	if (data->tokens->heredoc_pipe_read_fd != -1)
		close(data->tokens->heredoc_pipe_read_fd);
}

void	readline_loop(char *str, char **envp, t_data *data)
{
	t_append	append;
	t_token		*current;

	if (gestion_heredocs(data, envp, str) == -1)
		return ;
	current = data->tokens->first;
	if (check_pipe_syntax_error(current, data, str) > 0)
	{
		data->saved_stdout = dup(STDOUT_FILENO);
		if (data->saved_stdout != -1)
		{
			dup2(data->saved_stdout, STDOUT_FILENO);
			close(data->saved_stdout);
		}
		exec_cmd_tokens(data, envp);
		dup2(data->saved_stdout, STDOUT_FILENO);
		close(data->saved_stdout);
		if (data->tokens->heredoc_pipe_read_fd != -1)
			close(data->tokens->heredoc_pipe_read_fd);
	}
	else
		execute_simple_redirection(str, envp, data, &append);
	free(str);
	free_tokens(data->tokens->first);
	data->tokens = NULL;
}