/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_env.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: psirault <psirault@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/08 09:27:49 by psirault          #+#    #+#             */
/*   Updated: 2025/05/19 12:53:23 by psirault         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

char	**env_dup(char **envp)
{
	char	**new_env;
	size_t	i;

	i = 0;
	new_env = (char **)malloc(sizeof(char *) * (MAX_ENV_VARS + 1));
	if (new_env == NULL)
		return (NULL);
	while (envp[i] != NULL)
	{
		new_env[i] = ft_strdup(envp[i]);
		if (new_env[i] == NULL || i >= MAX_ENV_VARS - 1)
		{
			ft_free(new_env);
			return (NULL);
		}
		i++;
	}
	new_env[i] = NULL;
	return (new_env);
}

void	ft_env(char **strs, t_data *data)
{
	int	i;

	i = 0;
	while (strs[i] != NULL)
	{
		printf("%s\n", strs[i]);
		i++;
	}
	data->exit_status = 0;
}
