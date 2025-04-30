/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_cd.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: psirault <psirault@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/08 09:27:58 by psirault          #+#    #+#             */
/*   Updated: 2025/04/29 10:20:08 by psirault         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

static int	special_cases(char *str, char **env)
{
	char	*getenv;

	if (str == NULL || ft_strncmp(str, "~", 1) == 0)
	{
		getenv = ft_getenv(env, "HOME");
		if (chdir(getenv) != -1)
		{
			oldpwd_update(env, ft_getenv(env, "PWD"));
			pwd_update(env, ft_getenv(env, "HOME"));
			return (1);
		}
	}
	if (ft_strncmp(str, "-", 1) == 0)
	{
		getenv = ft_getenv(env, "OLDPWD");
		if (chdir(getenv) != -1)
		{
			oldpwd_update(env, ft_getenv(env, "PWD"));
			pwd_update(env, ft_getenv(env, "OLDPWD"));
			return (1);
		}
	}
	return (0);
}

void	ft_cd(char *str, char **env)
{
	char	*pwd;

	pwd = getcwd(NULL, 0);
	if (pwd == NULL)
	{
		perror("getcwd() error");
		return ;
	}
	if ((str == NULL || ft_strncmp(str, "~", 1) == 0
			|| ft_strncmp(str, "-", 1) == 0) && special_cases(str, env) != 0)
	{
		free(pwd);
		return ;
	}
	else if (chdir(str) != -1 && str != NULL)
	{
		oldpwd_update(env, pwd);
		pwd = getcwd(NULL, 0);
		pwd_update(env, pwd);
		free(pwd);
		return ;
	}
	else
		perror("chdir() error");
}
