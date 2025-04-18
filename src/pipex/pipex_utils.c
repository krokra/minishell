/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/27 08:16:28 by psirault          #+#    #+#             */
/*   Updated: 2025/04/14 20:45:48 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/pipex.h"

void	wrong_args(void)
{
	ft_putstr_fd("use pipex as follows : ./pipex file1 \"cmd1\" \"cmd2\" file2", 2);
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
	int		i;
	char	*str;
	char	*str2;

	i = 0;
	if (!cmd || !paths)
		return (NULL);
	while (paths[i] != NULL)
	{
		str = ft_strjoin(paths[i], "/");
		str2 = ft_strjoin(str, cmd);
		if (access(str2, F_OK) != -1)
			return (str2);
		free(str);
		free(str2);
		i++;
	}
	ft_free(paths);
	return (NULL);
}

char	*append_newline(char *str)
{
	char *new_str;
	int len;

	len = ft_strlen(str);
	new_str = malloc(len + 2);
	if (!new_str)
		return (NULL);
	ft_strlcpy(new_str, str, len + 1);
	new_str[len] = '\n';
	new_str[len + 1] = '\0';
	return (new_str);
}
