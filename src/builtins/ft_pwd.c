/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_pwd.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: psirault <psirault@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/08 09:28:00 by psirault          #+#    #+#             */
/*   Updated: 2025/04/18 18:38:38 by psirault         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

void	pwd_update(char **env, char *new_pwd)
{
	char	*pwd;
	int		i;

	i = -1;
	pwd = (char *)malloc(ft_strlen("PWD=") + ft_strlen(new_pwd) + 1);
	if  (!pwd)
		return ;
	ft_strlcpy(pwd, "PWD=", 5);
	ft_strlcat(pwd, new_pwd, ft_strlen("PWD=") + ft_strlen(new_pwd) + 1);
	while (env[++i] != NULL)
	{
		if (ft_strncmp(env[i], "PWD=", 4) == 0)
		{
			free(env[i]);
			env[i] = ft_strdup(pwd);
			free(pwd);
			return ;
		}
	}
	while (env[++i] != NULL)
		i++;
	env[i] = ft_strdup(pwd);
	env[i + 1] = NULL;
	free(pwd);
}

void	oldpwd_update(char **env, char *new_oldpwd)
{
	char	*pwd;
	int		i;

	i = -1;
	pwd = (char *)malloc(ft_strlen("OLDPWD=") + ft_strlen(new_oldpwd) + 1);
	if  (!pwd)
		return ;
	ft_strlcpy(pwd, "OLDPWD=", 8);
	ft_strlcat(pwd, new_oldpwd, ft_strlen("OLDPWD=") + ft_strlen(new_oldpwd) + 1);	
	while (env[++i] != NULL)
	{
		if (ft_strncmp(env[i], "OLDPWD=", 7) == 0)
		{
			free(env[i]);
			env[i] = ft_strdup(pwd);
			free(pwd);
			return ;
		}
	}
	while (env[++i] != NULL)
		i++;
	env[i] = ft_strdup(pwd);
	env[i + 1] = NULL;
	free(pwd);
}

void	ft_pwd(void)
{
	char	*buf;

	buf = getcwd(NULL, 0);
	if (buf != NULL)
		printf("%s\n", buf);
	else
		perror("getcwd() error");
	free(buf);
}
