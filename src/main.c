/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: psirault <psirault@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/07 09:34:28 by psirault          #+#    #+#             */
/*   Updated: 2025/04/30 11:53:22 by psirault         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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
	char	**arg;
	pid_t	pid;

	arg = ft_split(str, ' ');
	if (!arg)
	{
		free(str);
		free_tokens(tokens);
		return ;
	}
	if (!handle_builtins(arg, envp, ft_count_words(str, ' '), tokens))
	{
		pid = fork();
		if (pid == 0)
		{
			exec_cmd(str, envp);
			ft_exit(arg, envp, tokens);
			free(str);
			exit(0);
		}
		waitpid(pid, NULL, 0);
	}
	ft_free(arg);
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
		if (syntax_checker(tokens))
		{
			free(str);
			free_tokens(tokens);
			continue ;
		}
		readline_loop(str, envp, tokens);
		free_tokens(tokens);
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
	free_tokens(tokens);
#ifdef __APPLE__
	clear_history();
#else
	rl_clear_history();
#endif
	return (0);
}
