/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_cd.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/08 09:27:58 by psirault          #+#    #+#             */
/*   Updated: 2025/04/14 19:55:05 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

static int	special_cases(char *str, char **env)
{
	if (str == NULL || ft_strncmp(str, "~", 1) == 0)
		if (chdir(ft_getenv(env, "HOME")) != -1)
		{
			oldpwd_update(env, ft_getenv(env, "PWD"));
			pwd_update(env, ft_getenv(env, "HOME"));
			return (1);
		}
	if (ft_strncmp(str, "-", 1) == 0)
		if (chdir(ft_getenv(env, "OLDPWD")) != -1)
		{
			oldpwd_update(env, ft_getenv(env, "PWD"));
			pwd_update(env, ft_getenv(env, "OLDPWD"));
			return (1);
		}
	return (0);
}

void	ft_cd(char *str, char **env)
{
	if ((str == NULL || ft_strncmp(str, "~", 1) == 0 || ft_strncmp(str, "-", 1) == 0) && special_cases(str, env) != 0)
		return ;
	else if (chdir(str) != -1 && str != NULL)
	{
		oldpwd_update(env, ft_getenv(env, "PWD"));
		pwd_update(env, str);
		return ;
	}
	else
		perror("chdir() error");
}
