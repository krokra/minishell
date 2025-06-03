/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: psirault <psirault@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/27 08:16:28 by psirault          #+#    #+#             */
/*   Updated: 2025/06/02 10:11:58 by psirault         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/pipex.h"

void	wrong_args(void)
{
	ft_putstr_fd("use pipex as follows : ./pipex file1", 2);
	ft_putstr_fd(" \"cmd1\" \"cmd2\" file2", 2);
	exit(1);
}

char	**ft_get_paths(char *var, char **env)
{
	int		i;
	int		j;
	char	*str;

	i = 0;
	if (!var || !env)
		return (NULL);
	while (env[i] != NULL)
	{
		j = 0;
		while (env[i][j] != '=')
			j++;
		str = ft_substr(env[i], 0, j);
		if (ft_strncmp(str, var, ft_strlen(str)) == 0)
		{
			free(str);
			return (ft_split(env[i] + j + 1, ':'));
		}
		free(str);
		i++;
	}
	return (NULL);
}

char	*path_of_cmd(char *cmd, char **paths)
{
	char	*path;
	char	*tmp;
	int		i;

	i = 0;
	if (!cmd)
	{
		ft_free(paths);
		return (NULL);
	}
	if (!paths)
		return (ft_strdup(cmd));
	if (strchr(cmd, '/') != NULL)
	{
		if (access(cmd, F_OK | X_OK) == 0)
			return (ft_strdup(cmd));
		else
			return (NULL);
	}
	while (paths[i])
	{
		tmp = ft_strjoin(paths[i], "/");
		path = ft_strjoin(tmp, cmd);
		free(tmp);
		if (access(path, F_OK | X_OK) == 0)
		{
			ft_free(paths);
			return (path);
		}
		free(path);
		i++;
	}
	ft_free(paths);
	return (NULL);
}
