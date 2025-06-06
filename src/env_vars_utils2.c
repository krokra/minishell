/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_vars_utils2.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbariol- <nassimbariol@student.42.fr>>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/04 11:26:25 by psirault          #+#    #+#             */
/*   Updated: 2025/06/06 18:58:25 by nbariol-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

int	is_valid_var_char(char c)
{
	return (ft_isalnum(c) || c == '_');
}

size_t	get_var_name_len(char *str)
{
	size_t	len;

	if (str[0] == '?' && str[1] == '$' && str[2] == '\0')
		return (2);
	if (str[0] == '?')
		return (1);
	len = 0;
	while (str[len] && is_valid_var_char(str[len]))
		len++;
	return (len);
}

char	*get_env_value(char *var_name, char **envp, t_data *data)
{
	char	*value;

	
	if (var_name[0] == '?' && var_name[1] == '$')
		return (strjoin_and_free_s1(ft_itoa(data->exit_status), "$"));
	else if (var_name[0] == '?' && var_name[1] == '\0')
		return (ft_itoa(data->exit_status));
	value = ft_getenv(envp, var_name);
	if (value)
		return (ft_strdup(value));
	else
		return (ft_strdup(""));
}
