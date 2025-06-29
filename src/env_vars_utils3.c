/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_vars_utils3.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: psirault <psirault@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/04 11:26:21 by psirault          #+#    #+#             */
/*   Updated: 2025/06/04 12:05:16 by psirault         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

static int	is_valid_shlvl(const char *value)
{
	int	i;

	i = 0;
	if (!value)
		return (0);
	while (value[i])
	{
		if (!ft_isdigit(value[i]))
			return (0);
		i++;
	}
	return (1);
}

void	handle_shlvl(char **envp)
{
	char	*shlvl_value;
	int		shlvl;
	char	*new_value;

	shlvl_value = ft_getenv(envp, "SHLVL");
	if (!shlvl_value || !is_valid_shlvl(shlvl_value))
		shlvl = 1;
	else
	{
		shlvl = ft_atoi(shlvl_value) + 1;
		if (shlvl > 999)
			shlvl = 1;
	}
	new_value = ft_itoa(shlvl);
	if (new_value)
	{
		if (shlvl_value)
			ft_setenv(envp, "SHLVL", new_value, 1);
		else
			ft_setenv(envp, "SHLVL", new_value, 0);
		free(new_value);
	}
}

static int	setenv_loop(char **env, const char *name, const char *value, int ow)
{
	int		i;
	char	*new_entry;

	i = 0;
	while (env[i])
	{
		if (ft_strncmp(env[i], name, ft_strlen(name)) == 0
			&& env[i][ft_strlen(name)] == '=')
		{
			if (!ow)
				return (0);
			new_entry = ft_strjoin(name, "=");
			new_entry = strjoin_and_free_s1(new_entry, value);
			free(env[i]);
			env[i] = new_entry;
			return (0);
		}
		i++;
	}
	return (1);
}

int	ft_setenv(char **env, const char *name, const char *value, int overwrite)
{
	int		i;
	char	*new_entry;
	char	**new_env;

	if (!name || !value)
		return (-1);
	if (setenv_loop(env, name, value, overwrite) == 0)
		return (0);
	new_entry = ft_strjoin(name, "=");
	new_entry = strjoin_and_free_s1(new_entry, value);
	i = 0;
	while (env[i])
		i++;
	new_env = ft_realloc(env, (i + 1) * sizeof(char *),
			(i + 2) * sizeof(char *));
	if (!new_env)
	{
		free(new_entry);
		return (-1);
	}
	new_env[i] = new_entry;
	new_env[i + 1] = NULL;
	free(new_entry);
	return (0);
}
