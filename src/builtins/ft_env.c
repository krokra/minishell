/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_env.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: psirault <psirault@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/08 09:27:49 by psirault          #+#    #+#             */
/*   Updated: 2025/04/08 10:19:50 by psirault         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

char	**env_dup(char **envp)
{
	char **env_cpy;
	int i = 0;

	while (envp[i])
		i++;
	env_cpy = malloc(sizeof(char *) * (i + 1));
	if (!env_cpy)
		return (NULL);
	for (int j = 0; j < i; j++)
	{
		env_cpy[j] = ft_strdup(envp[j]);
		if (!env_cpy[j])
		{
			while (j-- > 0)
				free(env_cpy[j]);
			free(env_cpy);
			return (NULL);
		}
	}
	env_cpy[i] = NULL;
	return (env_cpy);
}

void	ft_env(char **envp)
{
	char	*str;

	while (*envp != 0)
	{
		str = *envp;
		printf("%s\n", str);
		envp++;
	}
}
