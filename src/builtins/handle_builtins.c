/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handle_builtins.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: psirault <psirault@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/08 11:18:48 by psirault          #+#    #+#             */
/*   Updated: 2025/05/07 11:14:42 by psirault         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

int	handle_builtins(char **envp, t_token *token)
{
	if (!token)
		return (0);
	if (ft_strncmp(token->content, "cd", 3) == 0)
		ft_cd(token->next->content, envp);
	else if (ft_strncmp(token->content, "export", 7) == 0)
		ft_export(envp, token->next->content);
	else if (ft_strncmp(token->content, "env", 4) == 0)
		ft_env(envp);
	else if (ft_strncmp(token->content, "pwd", 4) == 0)
		ft_pwd();
	else if (ft_strncmp(token->content, "exit", 5) == 0)
		ft_exit(envp, token);
	else if (ft_strncmp(token->content, "echo", 5) == 0)
		ft_echo(token->next);
	else if (ft_strncmp(token->content, "unset", 6) == 0)
		ft_unset(token->next->content, envp);
	else
		return (0);
	return (1);
}
