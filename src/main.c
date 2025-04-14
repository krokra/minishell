/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: psirault <psirault@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/07 09:34:28 by psirault          #+#    #+#             */
/*   Updated: 2025/04/08 11:59:50 by psirault         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"
#include <signal.h>

void	sigint_prompt(int sig)
{
	if (sig == SIGINT)
	{
		write(1, "\n", 1);
		rl_on_new_line();
		rl_replace_line("", 0);
		rl_redisplay();
	}
}

void	readline_loop(char *str, char **envp)
{
	char	**arg;
	pid_t	pid;

	arg = ft_split(str, ' ');
	if (!handle_builtins(arg, envp, ft_count_words(str, ' ')))
	{
		if ((pid = fork()) == 0)
			exec_cmd(str, envp);
		waitpid(pid, NULL, 0);
	}
	printf("\n");
	ft_free(arg);
	free(str);
}

int	main(int ac, char **av, char **envp)
{
	char	*str;
	char **env_cpy;
    struct sigaction sa;

	env_cpy = env_dup(envp);

    sa.sa_handler = sigint_prompt;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGINT, &sa, NULL) == -1)
    {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }
	while (1)
	{
		str = readline("minishell$> ");
		readline_loop(str, env_cpy);
	}
	rl_clear_history();
}
