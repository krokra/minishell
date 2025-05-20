/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_echo.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: psirault <psirault@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/08 09:27:57 by psirault          #+#    #+#             */
/*   Updated: 2025/05/15 14:11:22 by psirault         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

void	ft_echo(t_token *token)
{
	int first_arg = 1;
	while (token != NULL && 
		   token->type != T_PIPE && 
		   token->type != T_REDIR_IN &&
		   token->type != T_REDIR_OUT &&
		   token->type != T_APPEND &&
		   token->type != T_HEREDOC)
	{
		if (!first_arg)
			write(STDOUT_FILENO, " ", 1);
		write(STDOUT_FILENO, token->content, ft_strlen(token->content));
		first_arg = 0;
		token = token->next;
	}
	write(STDOUT_FILENO, "\n", 1);
}
