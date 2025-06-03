/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_unset.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: psirault <psirault@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/08 09:28:01 by psirault          #+#    #+#             */
/*   Updated: 2025/06/02 14:05:03 by psirault         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

static int	is_valid_identifier_char(char c)
{
	return (ft_isalnum(c) || c == '_');
}

static int	is_valid_identifier(const char *name)
{
	int	i;

	if (!name || !*name || (ft_isdigit(*name)))
		return (0);
	if (!ft_isalpha(*name))
		return (0);
	i = 1;
	while (name[i])
	{
		if (!is_valid_identifier_char(name[i]))
			return (0);
		i++;
	}
	return (1);
}

static void	unset_loop(t_token *token, char **env, t_data *data, int i)
{
	int	len;

	while (env[i] != NULL)
	{
		len = ft_strlen(token->content);
		if (ft_strncmp(env[i], token->content, len) == 0
			&& env[i][len] == '=')
		{
			free(env[i]);
			while (env[i] != NULL)
			{
				env[i] = env[i + 1];
				i++;
			}
			data->exit_status = 0;
			break ;
		}
		i++;
	}
}

void	ft_unset(t_token *token, char **env, t_data *data)
{
	if (!token)
	{
		data->exit_status = 1;
		ft_putstr_fd("unset : not enough arguments\n", 1);
		return ;
	}
	while (token)
	{
		if (!is_valid_identifier(token->content))
		{
			ft_putstr_fd("minishell: unset: `", 2);
			ft_putstr_fd(token->content, 2);
			ft_putstr_fd("': not a valid identifier\n", 2);
			return ;
		}
		if (token->content == NULL || ft_strchr(token->content, '=') != NULL)
			return ;
		unset_loop(token, env, data, 0);
		token = token->next;
	}
	data->exit_status = 1;
}
