/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_exit.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbariol- <nassimbariol@student.42.fr>>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/08 09:28:18 by psirault          #+#    #+#             */
/*   Updated: 2025/06/09 18:06:39 by nbariol-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

static int	is_valid_number(const char *str)
{
	int	i;

	i = 0;
	if (str[i] == '-' || str[i] == '+')
		i++;
	if (!str[i])
		return (0);
	while (str[i])
	{
		if (!ft_isdigit(str[i]))
			return (0);
		i++;
	}
	return (1);
}

static void	cleanup_and_exit(char **envp, t_token *tokens, t_data *data,
		int status)
{
	ft_putstr_fd("exit\n", 2);
	ft_free(envp);
	free(data);
	free_tokens(tokens->first);
	exit(status);
}

void	ft_exit(char **envp, t_token *tokens, t_data *data)
{
	t_token	*arg;
	int		exit_status;

	arg = tokens->next;
	exit_status = data->exit_status;
	if (!arg)
		cleanup_and_exit(envp, tokens, data, exit_status);
	if (!is_valid_number(arg->content))
	{
		ft_putstr_fd("minishell: exit: ", 2);
		ft_putstr_fd(arg->content, 2);
		ft_putstr_fd(": numeric argument required\n", 2);
		cleanup_and_exit(envp, tokens, data, 2);
	}
	exit_status = ft_atoi(arg->content);
	if (arg->next)
	{
		ft_putstr_fd("minishell: exit: too many arguments\n", 2);
		data->exit_status = 1;
		return ;
	}
	cleanup_and_exit(envp, tokens, data, exit_status % 256);
}
