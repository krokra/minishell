/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_vars_utils5.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: psirault <psirault@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/09 13:35:44 by nbariol-          #+#    #+#             */
/*   Updated: 2025/06/09 17:52:28 by psirault         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

char	*free_replace_vars(t_vars *vars)
{
	char	*res;

	res = vars->result;
	free(vars);
	return (res);
}

int	str_result(t_vars *vars, char c)
{
	vars->tmp = vars->result;
	vars->result = ft_strjoin(vars->result, (char []){c, 0});
	if (!vars->result)
		return (1);
	free(vars->tmp);
	return (0);
}
