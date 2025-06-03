/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: psirault <psirault@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/07 09:34:28 by psirault          #+#    #+#             */
/*   Updated: 2025/06/02 12:09:58 by psirault         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

//double leaks

#include "../includes/minishell.h"
#include <signal.h>

void	disable_ctrl_backslash(void)
{
	struct termios	term;

	if (tcgetattr(STDIN_FILENO, &term) == -1)
	{
		perror("tcgetattr");
		exit(EXIT_FAILURE);
	}
	term.c_cc[VQUIT] = 0;
	if (tcsetattr(STDIN_FILENO, TCSANOW, &term) == -1)
	{
		perror("tcsetattr");
		exit(EXIT_FAILURE);
	}
}

void	readline_loop(char *str, char **envp, t_data *data)
{
	int redir_applied;
	int last_redir;
	t_token *redir;
	int	fd;	

	replace_env_vars(data->tokens, envp, data);
	remove_quotes_after_expansion(data->tokens);
	merge_tokens_without_space(&data->tokens);
	if (handle_heredocs(data->tokens, envp, data) == -1)
	{
		ft_putstr_fd("minishell: error handling heredocs\n", 2);
		free(str);
		free_tokens(data->tokens->first);
		data->tokens = NULL;
		return;
	}
	t_token *current = data->tokens->first;
	int pipe_count = 0;
	while (current)
	{
		if (current->type == T_PIPE)
		{
			pipe_count++;
			if (!current->next || current->next->type == T_PIPE)
			{
				ft_putstr_fd("minishell: syntax error near unexpected token '|'\n", 2);
				free(str);
				free_tokens(data->tokens->first);
				data->tokens = NULL;
				return;
			}
		}
		current = current->next;
	}	
	if (pipe_count > 0)
	{
		data->saved_stdout = dup(STDOUT_FILENO);
		exec_cmd_tokens(data, envp);
		dup2(data->saved_stdout, STDOUT_FILENO);
		close(data->saved_stdout);
		if (data->tokens->heredoc_pipe_read_fd != -1)
			close(data->tokens->heredoc_pipe_read_fd);
	}
	else
	{
		data->saved_stdout = -1;
		redir_applied = 0;
		redir = data->tokens->first;
		while (redir)
		{
			last_redir = 0;
			if (redir->type == T_APPEND || redir->type == T_REDIR_OUT)
			{
				if (data->saved_stdout == -1)
					data->saved_stdout = dup(STDOUT_FILENO);
				if (redir->type == T_APPEND)
				{
					fd = handle_append_redirection(redir);
					if (fd < 0)
						last_redir = 1;
					else
						last_redir = 0;
					close(fd);
				}
				else
				{
					fd = handle_redirections(redir);
					if (fd < 0)
						last_redir = 1;
					else
						last_redir = 0;
					close(fd);
				}
				redir_applied = 1;
			}
			redir = redir->next;
		}
		if (last_redir < 0)
		{
			if (redir_applied && data->saved_stdout != -1)
				close(data->saved_stdout);
			free(str);
			free_tokens(data->tokens->first);
			data->tokens = NULL;
			return;
		}
		if (!handle_builtins(envp, data->tokens->first, data))
			exec_cmd_tokens(data, envp);
		if (redir_applied && data->saved_stdout != -1)
		{
			dup2(data->saved_stdout, STDOUT_FILENO);
			close(data->saved_stdout);
		}
		if (data->tokens->heredoc_pipe_read_fd != -1)
			close(data->tokens->heredoc_pipe_read_fd);
	}
	free(str);
	free_tokens(data->tokens->first);
	data->tokens = NULL;
}

// int	is_valid_command(t_token *tokens, char **envp)
// {
// 	t_token *current;

// 	current = tokens;
// 	while (current->next)
// 	{
// 		if (current->type == T_WORD && current->next->type != T_REDIR_IN)
// 		{
// 			char *path = path_of_cmd(current->content, ft_get_paths("PATH", envp));
// 			if (path == NULL && !is_builtin(current->content))
// 			{
// 				ft_putstr_fd("command not found : ", 1);
// 				ft_putstr_fd(current->content, 1);
// 				write(1, "\n", 1);
// 				free(path);
// 				return (0);
// 			}
// 		}
// 		if (((current->type == T_WORD && current->next->type == T_WORD) ||
// 				((current->type == T_HEREDOC || current->type == T_REDIR_IN || current->type == T_REDIR_OUT || current->type == T_APPEND) &&
// 					current->next->type == T_WORD)) && current->next != NULL)
// 			current = current->next;
// 		if (current->next != NULL)
// 			current = current->next;
// 	}
// 	return (1);
// }

void	mainloop(char *str, char **envp, t_data *data)
{
	while (1)
	{
		str = readline("minishell$> ");
		if (str == NULL)
		{
			free(str);
			break;
		}
		add_history(str);
		if (str[0] == '\n' || str[0] == '\0')
		{
			free(str);
			continue;
		}
		data->tokens = NULL;
		quote_and_token_handling(str, find_first_quote(str), &data);
		// print_tokens(data->tokens);
		// if (!is_valid_command(data->tokens, envp))
		// {
		// 	free(str);
		// 	free_tokens(data->tokens->first);
		// 	continue;
		// }
		if (syntax_checker(data->tokens))
		{
			free(str);
			if (data->tokens != NULL)
			{
				free_tokens(data->tokens->first);
				data->tokens = NULL;
			}
			continue;
		}
		readline_loop(str, envp, data);
	}
}

int	main(int argc, char **argv, char **envp)
{
	char *str;
	char **env_cpy;
	t_token *tokens;
	t_data *data;

	data = malloc(sizeof(t_data));
	if (!data)
	{
		perror("malloc");
		return (1);
	}
	
	(void)argc;
	(void)argv;
	tokens = NULL;
	data->tokens = tokens;
	data->exit_status = 0;
	data->status_getter = 0;
	
	env_cpy = env_dup(envp);
	
	disable_ctrl_backslash();
	signal_handler();
	
	str = NULL;
	mainloop(str, env_cpy, data);
	
	ft_free(env_cpy);
	free(str);
	free(data);
#ifdef __APPLE__
	clear_history();
#else
	rl_clear_history();
#endif
	return (0);
}
