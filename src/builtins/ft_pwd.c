/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_pwd.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbariol- <nassimbariol@student.42.fr>>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/08 09:28:00 by psirault          #+#    #+#             */
/*   Updated: 2025/06/09 15:33:36 by nbariol-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

void	pwd_update(char **env, char *new_pwd)
{
	char	*pwd;
	int		i;

	i = -1;
	pwd = (char *)malloc(ft_strlen("PWD=") + ft_strlen(new_pwd) + 1);
	if (!pwd)
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

int	while_oldpwd(char **env, char *pwd, int *i)
{
	while (env[*++i] != NULL)
	{
		if (ft_strncmp(env[*i], "OLDPWD=", 7) == 0)
		{
			free(env[*i]);
			env[*i] = ft_strdup(pwd);
			if (!env[*i])
				return (0);
			free(pwd);
			return (1);
		}
	}
	return (0);
}

void	oldpwd_update(char **env, char *new_oldpwd)
{
	char	*pwd;
	int		i;

	i = -1;
	pwd = (char *)malloc(ft_strlen("OLDPWD=") + ft_strlen(new_oldpwd) + 1);
	if (!pwd)
		return ;
	ft_strlcpy(pwd, "OLDPWD=", 8);
	ft_strlcat(pwd, new_oldpwd, ft_strlen("OLDPWD=") + ft_strlen(new_oldpwd)
		+ 1);
	if (while_oldpwd(env, pwd, &i) == 1)
		return ;
	while (env[++i] != NULL)
		i++;
	env[i] = ft_strdup(pwd);
	free(pwd);
	if (!env[i])
		return ;
	env[i + 1] = NULL;
}

void	ft_pwd(t_data *data)
{
	char	*buf;

	buf = getcwd(NULL, 0);
	if (buf != NULL)
	{
		printf("%s\n", buf);
		data->exit_status = 0;
	}
	else
	{
		perror("getcwd() error");
		data->exit_status = 1;
	}
	free(buf);
}
