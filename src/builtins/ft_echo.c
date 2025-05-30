/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_echo.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: psirault <psirault@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/08 09:27:57 by psirault          #+#    #+#             */
/*   Updated: 2025/05/30 18:04:45 by psirault         ###   ########.fr       */
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
    int	first_arg;
    int	skip_newline;

    first_arg = 1;
    skip_newline = 0;
    while (token && is_n_flag(token->content))
    {
        skip_newline = 1;
        token = token->next;
        first_arg = 1;
    }
    while (token != NULL && (token->type == T_WORD || token->type == T_ENVVAR))
    {
        if (first_arg == 0)
            ft_putchar_fd(' ', 1);
        ft_putstr_fd(token->content, 1);
		if (ft_strncmp(token->content, "$", 2) == 0)
        	first_arg = 0;
        token = token->next;
    }
    if (!skip_newline)
        ft_putchar_fd('\n', 1);
    data->exit_status = 0;
}
