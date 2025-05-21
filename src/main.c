/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: psirault <psirault@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/07 09:34:28 by psirault          #+#    #+#             */
/*   Updated: 2025/05/21 08:40:07 by psirault         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

//double leaks

#include "../includes/minishell.h"
#include <signal.h>

// Nouvelle fonction helper
static int has_pipe(t_token *tokens)
{
	t_token *current;
	current = tokens;
	while (current != NULL)
	{
		if (current->type == T_PIPE)
			return (1);
		current = current->next;
	}
	return (0);
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
	if (tcsetattr(STDIN_FILENO, TCSANOW, &term) == -1)
	{
		perror("tcsetattr");
		exit(EXIT_FAILURE);
	}
}

void	readline_loop(char *str, char **envp, t_data *data)
{
	int saved_stdout;
	int redir_applied;
	int last_redir;
	t_token *redir;
	int	fd;

	printf("[DEBUG] Avant handle_heredocs\n");
	replace_env_vars(data->tokens, envp, data);
	if (handle_heredocs(data->tokens, envp, data) == -1)
	{
		ft_putstr_fd("minishell: error handling heredocs\n", 2);
		free(str);
		free_tokens(data->tokens->first);
		if (data->tokens->heredoc_pipe_read_fd != -1)
			close(data->tokens->heredoc_pipe_read_fd);
		return;
	}
	printf("[DEBUG] Après handle_heredocs\n");
	if (has_pipe(data->tokens->first))
	{
		printf("[DEBUG] has_pipe=1, appel exec_cmd_tokens\n");
		exec_cmd_tokens(data, envp);
	}
	else
	{
		printf("[DEBUG] has_pipe=0\n");
		saved_stdout = -1;
		redir_applied = 0;
		redir = data->tokens->first;
		while (redir)
		{
			last_redir = 0;
			if (redir->type == T_APPEND || redir->type == T_REDIR_OUT)
			{
				if (saved_stdout == -1)
					saved_stdout = dup(STDOUT_FILENO); //sauvergade sortie standard
				if (redir->type == T_APPEND)
				{
					if (handle_append_redirection(redir) < 0)
					{
						last_redir = 1; // si echou
					}
					else
						last_redir = 0;
				}
				else // T_REDIR_OUT
				{
					fd = handle_redirections(redir);
					if (fd < 0)
					{
						last_redir = 1; // si echou
					}
					else
						last_redir = 0;
					close(fd);
				}
				redir_applied = 1;
			}
			redir = redir->next;
		}
		if (last_redir < 0) //verifie si la redirection a echouer
		{
    		if (redir_applied && saved_stdout != -1)
    		{
                close(saved_stdout);
    		}
    		free(str);
    		free_tokens(data->tokens->first);
			return;
		}
		// Exécuter le builtin (ou la commande) pendant la redirection
		if (!handle_builtins(envp, data->tokens->first, data))
		{
			printf("[DEBUG] exec_cmd_tokens appelé dans else (pas de pipe)\n");
			exec_cmd_tokens(data, envp);
		}
		// Restaurer la sortie standard après
		if (redir_applied && saved_stdout != -1)
		{
			dup2(saved_stdout, STDOUT_FILENO);
			close(saved_stdout);
		}
	}
	free(str);
	free_tokens(data->tokens->first);
}

void	mainloop(char *str, char **envp, t_data *data)
{
	while (1)
	{
		str = readline("minishell$> ");
		if (str == NULL)
		{
			free(str);
			break ;
		}
		add_history(str);
		if (str[0] == '\n' || str[0] == '\0')
		{
			free(str);
			continue ;
		}
		quote_and_token_handling(str, find_first_quote(str), &data);
		if (syntax_checker(data->tokens))
		{
			free(str);
			if (data->tokens != NULL)
				free_tokens(data->tokens->first);
			continue;
		}
		readline_loop(str, envp, data);
	}
	free(str);
}

int	main(int argc, char **argv, char **envp)
{
	char		*str;
	char		**env_cpy;
	t_token		*tokens;
	t_data		*data;

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
