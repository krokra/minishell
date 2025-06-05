/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_export.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: psirault <psirault@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/08 09:28:27 by psirault          #+#    #+#             */
/*   Updated: 2025/06/05 09:54:24 by psirault         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"	

char	*export_var(char *name, char *value)
{
	char	*var;
	char	*temp;

	temp = ft_strjoin(name, "=");
	if (!temp)
	{
		free(name);
		free(value);
		return (NULL);
	}
	var = ft_strjoin(temp, value);
	free(temp);
	if (!var)
	{
		free(name);
		free(value);
		return (NULL);
	}
	return (var);
}

static void	free_resources(char *name, char *value, char *var)
{
	free(name);
	free(value);
	free(var);
}

static void	update_or_add_env_var(char **env, char *name, char *val, char *var)
{
	int	i;

	i = 0;
	while (env[i] != NULL)
	{
		if (ft_strncmp(env[i], name, ft_strlen(name)) == 0)
		{
			free(env[i]);
			env[i] = ft_strdup(var);
			free_resources(name, val, var);
			return ;
		}
		i++;
	}
	env[i] = ft_strdup(var);
	env[i + 1] = NULL;
	free_resources(name, val, var);
}

static void	export_loop(char **env, t_token *token, t_data *data)
{
	char	*name;
	char	*value;
	char	*var;

	while (token)
	{
		name = parse_export1(token->content);
		value = parse_export2(token->content);
		if (!name || !value)
		{
			free(name);
			data->exit_status = 1;
			return ;
		}
		var = export_var(name, value);
		if (!var)
		{
			free_resources(name, value, NULL);
			data->exit_status = 1;
			return ;
		}
		update_or_add_env_var(env, name, value, var);
		token = token->next;
	}
}

void	ft_export(char **env, t_token *token, t_data *data)
{
	if (!token || token->type == T_PIPE || token->type == T_REDIR_OUT || token->type == T_APPEND)
	{
		ft_env(env, data);
		return ;
	}
	export_loop(env, token, data);
	data->exit_status = 0;
}
