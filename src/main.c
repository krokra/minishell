/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: psirault <psirault@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/07 09:34:28 by psirault          #+#    #+#             */
/*   Updated: 2025/04/29 10:13:26 by psirault         ###   ########.fr       */
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

void	readline_loop(char *str, char **envp)
{
	char	**arg;
	pid_t	pid;

	arg = ft_split(str, ' ');
	if (!arg)
	{
		free(str);
		return ;
	}
	if (!handle_builtins(arg, envp, ft_count_words(str, ' ')))
	{
		pid = fork();
		if (pid == 0)
		{
			ft_free(arg);
			exec_cmd(str, envp);
			ft_free(envp);
			free(str);
			exit(0);
		}
		waitpid(pid, NULL, 0);
	}
	ft_free(arg);
	free(str);
}

void	mainloop(char *str, char **envp)
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
		readline_loop(str, envp);
	}
	free(str);
}

int	main(int ac, char **av, char **envp)
{
	char	*str;
	char	**env_cpy;

	env_cpy = env_dup(envp);
	(void)ac;
	(void)av;
	disable_ctrl_backslash();
	signal_handler();
	str = NULL;
	mainloop(str, env_cpy);
	ft_free(env_cpy);
	free(str);
	rl_clear_history();
	return (0);
}
