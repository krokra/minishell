/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_pwd.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: psirault <psirault@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/08 09:28:00 by psirault          #+#    #+#             */
/*   Updated: 2025/04/08 09:39:47 by psirault         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

void	pwd_update(char **env, char *new_pwd)
{
	char	*pwd;
	int		i;

	i = -1;
	pwd = malloc(ft_strlen("PWD=") + ft_strlen(new_pwd) + 1);
	if  (!pwd)
		return (NULL);
	ft_strcpy(pwd, "PWD=");
	ft_strcat(pwd, new_pwd);
	while (env[++i] != NULL)
	{
		if (ft_strncmp(env[i], "PWD=", 4) == 0)
		{
			free(env[i]);
			env[i] = pwd;
			free(pwd);
			return (env);
		}
	}
	while (env[++i] != NULL)
		i++;
	env[i] = pwd;
	env[i + 1] = NULL;
	free(pwd);
	return (env);
}

void	oldpwd_update(char **env, char *new_oldpwd)
{
	char	*pwd;
	int		i;

	i = -1;
	pwd = malloc(ft_strlen("OLDPWD=") + ft_strlen(new_pwd) + 1);
	if  (!pwd)
		return (NULL);
	ft_strcpy(pwd, "OLDPWD=");
	ft_strcat(pwd, new_pwd);
	while (env[++i] != NULL)
	{
		if (ft_strncmp(env[i], "OLDPWD=", 7) == 0)
		{
			free(env[i]);
			env[i] = pwd;
			free(pwd);
			return (env);
		}
	}
	while (env[++i] != NULL)
		i++;
	env[i] = pwd;
	env[i + 1] = NULL;
	free(pwd);
	return (env);
}

void	ft_pwd(char **env)
{
	char	*buf;

	buf = getcwd(NULL, 0);
	if (buf != NULL)
		printf("%s", buf);
	else
		perror("getcwd() error");
}
