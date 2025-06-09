/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: psirault <psirault@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/07 09:34:28 by psirault          #+#    #+#             */
/*   Updated: 2025/06/09 16:10:02 by psirault         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"
#include <signal.h>

int	is_empty_input(char *str)
{
	return (!str || str[0] == '\n' || str[0] == '\0');
}

void	disable_ctrl_backslash(void)
{
	struct termios	term;

	if (tcgetattr(STDIN_FILENO, &term) == -1)
	{
		perror("tcgetattr");
		exit(EXIT_FAILURE);
	}
	term.c_cc[VQUIT] = 0;
	term.c_cc[VSUSP] = 0;
	if (tcsetattr(STDIN_FILENO, TCSANOW, &term) == -1)
	{
		perror("tcsetattr");
		exit(EXIT_FAILURE);
	}
}

int	gestion_heredocs(t_data *data, char **envp, char *str)
{
	replace_env_vars(data->tokens, envp, data);
	merge_tokens_without_space(&data->tokens);
	close_all_except_std();
	if (handle_heredocs(data->tokens, envp, data) == -1)
	{
		ft_putstr_fd("minishell: error handling heredocs\n", 2);
		free(str);
		free_tokens(data->tokens->first);
		data->tokens = NULL;
		return (-1);
	}
	return (0);
}

int	check_pipe_syntax_error(t_token *current, t_data *data, char *str)
{
	int	pipe_count;

	pipe_count = 0;
	while (current)
	{
		if (current->type == T_PIPE)
		{
			pipe_count++;
			if (!current->next || current->next->type == T_PIPE)
			{
				ft_putstr_fd("minishell: syntax error unexpected token '|'\n",
					2);
				free(str);
				free_tokens(data->tokens->first);
				data->tokens = NULL;
				break ;
			}
		}
		current = current->next;
	}
	return (pipe_count);
}

void	redirection_exec(t_data *data, t_append *append, t_token *redir)
{
	if (data->saved_stdout == -1)
		data->saved_stdout = dup(STDOUT_FILENO);
	if (redir->type == T_APPEND)
	{
		append->fd = handle_append_redirection(redir);
		if (append->fd < 0)
		{
			append->last_redir = 1;
			return ;
		}
		close(append->fd);
	}
	else
	{
		append->fd = handle_redirections(redir);
		if (append->fd < 0)
		{
			append->last_redir = 1;
			return ;
		}
		close(append->fd);
	}
	append->redir_applied = 1;
}
