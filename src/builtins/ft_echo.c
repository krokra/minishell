/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_echo.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: psirault <psirault@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/08 09:27:57 by psirault          #+#    #+#             */
<<<<<<< HEAD
/*   Updated: 2025/05/15 14:11:22 by psirault         ###   ########.fr       */
=======
/*   Updated: 2025/05/19 13:14:53 by psirault         ###   ########.fr       */
>>>>>>> 55534cf (data struct + exit statuses done)
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

static int	is_n_flag(const char *str)
{
<<<<<<< HEAD
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
=======
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
        if (!first_arg)
            ft_putchar_fd(' ', 1);
        ft_putstr_fd(token->content, 1);
        first_arg = 0;
        token = token->next;
    }
    if (!skip_newline)
        ft_putchar_fd('\n', 1);
    data->exit_status = 0;
>>>>>>> 55534cf (data struct + exit statuses done)
}
