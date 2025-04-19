/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: psirault <psirault@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/07 09:34:28 by psirault          #+#    #+#             */
/*   Updated: 2025/04/18 18:38:16 by psirault         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"
#include <signal.h>

void	disable_ctrl_backslash(void)
{
    struct termios term;

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

void	signal_handler(void)
{
	struct sigaction sa;

    sa.sa_handler = sigint_prompt;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGINT, &sa, NULL) == -1)
    {
        perror("sigaction");
        exit(EXIT_FAILURE);
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

void	mainloop(char *str, char **envp)
{
	while (1)
	{
		str = readline("minishell$> ");
		add_history(str);
		if (str == NULL)
		{
			free(str);
			printf("\n");
			break ;
		}
		if (str[0] == '\n' || str[0] == '\0')
		{
			free(str);
			printf("\n");
			continue ;
		}
		readline_loop(str, envp);
	}
	rl_clear_history();
}

int	main(int ac, char **av, char **envp)
{
	char	*str;
	char **env_cpy;

	env_cpy = env_dup(envp);
	(void)ac;
	(void)av;
	disable_ctrl_backslash();
	signal_handler();
	str = NULL;
	mainloop(str, env_cpy);
	ft_free(env_cpy);
	free(str);
	return (0);
}
