/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_echo.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbariol- <nassimbariol@student.42.fr>>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/08 09:27:57 by psirault          #+#    #+#             */
/*   Updated: 2025/06/09 13:11:07 by nbariol-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

static int	is_n_flag(const char *str)
{
	int	i;

	if (!str || str[0] != '-' || str[1] != 'n')
		return (0);
	i = 2;
	while (str[i])
	{
		if (str[i] != 'n')
			return (0);
		i++;
	}
	return (1);
}

void	ft_echo(t_token *token, t_data *data)
{
	int	skip_newline;

	skip_newline = 0;
	while (token && is_n_flag(token->content))
	{
		skip_newline = 1;
		token = token->next;
	}
	while (token && (token->type == T_WORD || token->type == T_ENVVAR))
	{
		ft_putstr_fd(token->content, 1);
		if (token->type == T_ENVVAR && token->content[0] == '\0'
			&& token->quotes == 0 && token->next)
		{
			token = token->next;
			continue ;
		}
		if (token->has_space_after && token->next
			&& (token->next->type == T_WORD || token->next->type == T_ENVVAR))
			ft_putchar_fd(' ', 1);
		token = token->next;
	}
	if (!skip_newline)
		ft_putchar_fd('\n', 1);
	data->exit_status = 0;
}
