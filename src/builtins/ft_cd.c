/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_cd.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbariol- <nassimbariol@student.42.fr>>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/08 09:27:58 by psirault          #+#    #+#             */
/*   Updated: 2025/06/09 11:43:39 by nbariol-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

static int	special_cases(char *str, char **env)
{
	char	*getenv;

	getenv = NULL;
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

static int cd_check(char *str, char **env)
{
	if ((str == NULL || ft_strncmp(str, "~", 1) == 0
		|| ft_strncmp(str, "-", 1) == 0) && special_cases(str, env) != 0)
		return (0);
	else
		return (1);
}

static void	new_pwd(char **env, char *pwd, t_data *data)
{
	oldpwd_update(env, pwd);
		free(pwd);
		pwd = getcwd(NULL, 0);
		pwd_update(env, pwd);
		free(pwd);
		data->exit_status = 0;
}
void	ft_cd(char *str, char **env, t_data *data)
{
	char	*pwd;

	pwd = getcwd(NULL, 0);
	if (pwd == NULL)
	{
		data->exit_status = 1;
		return ;
	}
	if (cd_check(str, env) == 0)
	{
		free(pwd);
		data->exit_status = 0;
		return ;
	}
	else if (chdir(str) != -1 && str != NULL)
	{
		new_pwd(env, pwd, data);
		return ;
	}
	printf("cd :no such file or directory : %s\n", str);
	free(pwd);
	data->exit_status = 1;
}
