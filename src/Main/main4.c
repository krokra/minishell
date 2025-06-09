/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main4.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: psirault <psirault@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/09 13:04:48 by nbariol-          #+#    #+#             */
/*   Updated: 2025/06/09 17:35:47 by psirault         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

static void	close_heredoc_fd(t_data *data)
{
    if (data->tokens->heredoc_pipe_read_fd != -1)
        close(data->tokens->heredoc_pipe_read_fd);
}

static void	cleanup_and_free(char *str, t_data *data)
{
    free(str);
    free_tokens(data->tokens->first);
    data->tokens = NULL;
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
        close_heredoc_fd(data);
    }
    else
	{
        execute_simple_redirection(str, envp, data, &append);
	}
	cleanup_and_free(str, data);
}
