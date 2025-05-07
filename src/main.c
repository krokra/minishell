/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: psirault <psirault@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/07 09:34:28 by psirault          #+#    #+#             */
/*   Updated: 2025/05/07 13:54:55 by psirault         ###   ########.fr       */
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

void	readline_loop(char *str, char **envp, t_token *tokens)
{
	pid_t	pid;

	replace_env_vars(tokens, envp);
	if (handle_heredocs(tokens) == -1)
	{
		ft_putstr_fd("minishell: error handling heredocs\n", 2);
		free(str);
		free_tokens(tokens->first); // Sera remis à NULL dans mainloop
		return;
	}
	if (!handle_builtins(envp, tokens))
	{		
		pid = fork();
		if (pid == 0)
		{
			exec_cmd_tokens(tokens, envp);
			ft_exit(envp, tokens);
			free(str);
			exit(0);
		}
		waitpid(pid, NULL, 0);
	}
	free(str);
}

void	mainloop(char *str, char **envp, t_token *tokens)
{
	while (1)
	{
		str = readline("minishell$> ");
		add_history(str);
		if (str == NULL)
		{
			free(str);
			break ;
		}
		if (str[0] == '\n' || str[0] == '\0')
		{
			free(str);
			continue ;
		}
		quote_and_token_handling(str, find_first_quote(str), &tokens);
		// print_tokens(tokens);
		if (syntax_checker(tokens))
		{
			free(str);
			free_tokens(tokens->first);
			continue ;
		}
		readline_loop(str, envp, tokens);
		free_tokens(tokens->first);
	}
	free(str);
}

int	main(int ac, char **av, char **envp)
{
	char		*str;
	char		**env_cpy;
	t_token		*tokens;

	tokens = NULL;
	env_cpy = env_dup(envp);
	(void)ac;
	(void)av;
	disable_ctrl_backslash();
	signal_handler();
	str = NULL;
	mainloop(str, env_cpy, tokens);
	ft_free(env_cpy);
	free(str);
	//free_tokens(tokens);
#ifdef __APPLE__
	clear_history();
#else
	rl_clear_history();
#endif
	return (0);
}
