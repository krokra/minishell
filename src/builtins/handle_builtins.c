/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handle_builtins.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: psirault <psirault@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/08 11:18:48 by psirault          #+#    #+#             */
/*   Updated: 2025/04/18 18:39:03 by psirault         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

int	handle_builtins(char **arg, char **envp, int i)
{
	if (!arg || !arg[0])
		return (0);
	if (ft_strncmp(arg[0], "cd", 3) == 0)
		ft_cd(arg[1], envp);
	else if (ft_strncmp(arg[0], "export", 7) == 0)
		ft_export(envp, arg[1]);
	else if (ft_strncmp(arg[0], "env", 4) == 0)
		ft_env(envp);
	else if (ft_strncmp(arg[0], "pwd", 4) == 0)
		ft_pwd();
	else if (ft_strncmp(arg[0], "exit", 5) == 0)
		ft_exit(arg);
	else if (ft_strncmp(arg[0], "echo", 5) == 0)
		ft_echo(i, arg + 1);
	else if (ft_strncmp(arg[0], "unset", 6) == 0)
		ft_unset(arg[1], envp);
	else
		return (0);
	return (1);
}
