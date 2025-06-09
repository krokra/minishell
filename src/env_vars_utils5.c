/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_vars_utils5.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbariol- <nassimbariol@student.42.fr>>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/09 13:35:44 by nbariol-          #+#    #+#             */
/*   Updated: 2025/06/09 13:36:08 by nbariol-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

char	*replace_vars_in_str(t_token *tok, char *str, char **envp,
			t_data *data)
{
	t_vars	*vars;
	int		i;

	if (!(vars = malloc(sizeof(t_vars))))
		return (NULL);
	if (!(vars->result = ft_strdup("")))
		return (free(vars), NULL);
	vars->in_quote = 0;
	vars->quote_char = 0;
	vars->is_heredoc = (tok && tok->type == T_HEREDOC);
	while (*str)
	{
		if (quote_helper(&str, vars) || quote_helper2(&str, vars))
			continue ;
		if (ft_strncmp(str, "$", 2) == 0 && tok && !tok->has_space_after && tok->next)
			return (free_replace_vars(vars));
		i = quote_helper3(&str, vars, envp, data);
		if (i == 2)
			return (free_replace_vars(vars), NULL);
		if (i == 1 || str_result(vars, *str++))
			continue ;
	}
	return (free_replace_vars(vars));
}