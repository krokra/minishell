/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main_start.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbariol- <nassimbariol@student.42.fr>>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/09 13:03:32 by nbariol-          #+#    #+#             */
/*   Updated: 2025/06/09 13:03:43 by nbariol-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

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