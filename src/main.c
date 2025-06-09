/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbariol- <nassimbariol@student.42.fr>>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/07 09:34:28 by psirault          #+#    #+#             */
/*   Updated: 2025/06/09 11:15:06 by nbariol-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"
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
				ft_putstr_fd("minishell: syntax error near unexpected token '|'\n",
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

int	skip_empty(char *str)
{
	if (str[0] == '\n' || str[0] == '\0')
	{
		free(str);
		return (1);
	}
	return (0);
}

int	skip_if_bad_quotes(char *str, t_data **data)
{
	if (!quote_and_token_handling(str, find_first_quote(str), data))
	{
		free(str);
		return (1);
	}
	return (0);
}

int	skip_if_syntax_error(char *str, t_data *data)
{
	if (syntax_checker(data->tokens, data) == 1)
	{
		free(str);
		free_tokens(data->tokens->first);
		data->tokens = NULL;
		return (1);
	}
	return (0);
}

void	mainloop(char *str, char **envp, t_data *data)
{
	while (1)
	{
		str = readline("minishell$> ");
		if (!str)
			break ;
		if (skip_empty(str))
			continue ;
		add_history(str);
		data->tokens = NULL;
		if (skip_if_bad_quotes(str, &data))
			continue ;
		if (skip_if_syntax_error(str, data))
			continue ;
		readline_loop(str, envp, data);
	}
}

int	init_minishell(t_data **data, char ***env_cpy, char **envp)
{
	*data = malloc(sizeof(t_data));
	if (!*data)
		return (1);
	(*data)->tokens = NULL;
	(*data)->exit_status = 0;
	(*data)->status_getter = 0;
	*env_cpy = env_dup(envp);
	if (!*env_cpy)
	{
		free(*data);
		return (1);
	}
	disable_ctrl_backslash();
	signal_handler();
	return (0);
}

int	main(int argc, char **argv, char **envp)
{
	char	*str;
	char	**env_cpy;
	t_data	*data;

	(void)argc;
	(void)argv;
	str = NULL;
	if (!init_minishell(&data, &env_cpy, envp))
	{
		mainloop(str, env_cpy, data);
		ft_free(env_cpy);
		free(str);
		free(data);
		clear_history();
		return (0);
	}
	return (1);
}
