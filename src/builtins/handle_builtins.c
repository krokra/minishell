/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handle_builtins.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: psirault <psirault@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/08 11:18:48 by psirault          #+#    #+#             */
/*   Updated: 2025/05/29 19:31:59 by psirault         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

int	handle_builtins(char **envp, t_token *token, t_data *data)
{
	if (!token)
		return (0);
	if (ft_strncmp(token->content, "cd", 3) == 0 && token->next != NULL)
		ft_cd(token->next->content, envp, data);
	else if (ft_strncmp(token->content, "cd", 3) == 0 && token->next == NULL)
		ft_cd(NULL, envp, data);
	else if (ft_strncmp(token->content, "export", 7) == 0 && token->next != NULL)
		ft_export(envp, token->next->content, data);
	else if (ft_strncmp(token->content, "export", 7) == 0 && token->next == NULL)
		ft_export(envp, NULL, data);
	else if (ft_strncmp(token->content, "env", 4) == 0)
		ft_env(envp, data);
	else if (ft_strncmp(token->content, "pwd", 4) == 0)
		ft_pwd(data);
	else if (ft_strncmp(token->content, "exit", 5) == 0)
		ft_exit(envp, token, data);
	else if (ft_strncmp(token->content, "echo", 5) == 0)
		ft_echo(token->next, data);
	else if (ft_strncmp(token->content, "unset", 6) == 0)
		ft_unset(token->next->content, envp, data);
	else
		return (0);
	return (1);
}
