/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main3.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbariol- <nassimbariol@student.42.fr>>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/09 13:04:48 by nbariol-          #+#    #+#             */
/*   Updated: 2025/06/09 13:04:56 by nbariol-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

int	skip_empty(char *str)
{
	if (str[0] == '\n' || str[0] == '\0')
	{
		free(str);
		return (1);
	}
	return (0);
}

int	skip_if_bad_quotes(char *str, t_data **data)
{
	if (!quote_and_token_handling(str, find_first_quote(str), data))
	{
		free(str);
		return (1);
	}
	return (0);
}

int	skip_if_syntax_error(char *str, t_data *data)
{
	if (syntax_checker(data->tokens, data) == 1)
	{
		free(str);
		free_tokens(data->tokens->first);
		data->tokens = NULL;
		return (1);
	}
	return (0);
}

void	mainloop(char *str, char **envp, t_data *data)
{
	while (1)
	{
		str = readline("minishell$> ");
		if (!str)
			break ;
		if (skip_empty(str))
			continue ;
		add_history(str);
		data->tokens = NULL;
		if (skip_if_bad_quotes(str, &data))
			continue ;
		if (skip_if_syntax_error(str, data))
			continue ;
		readline_loop(str, envp, data);
	}
}
