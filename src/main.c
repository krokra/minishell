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

int	main(int ac, char **av, char **envp)
{
	char	*str;
	char	**arg;
	pid_t	pid;

	(void)ac, (void)av;
	while (1)
	{
		str = readline("\x1b[34m\nminishell$>\x1b[m");
		if (!str)
			exit(0);
		add_history(str);
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
	rl_clear_history();
}
