/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: psirault <psirault@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/29 09:54:14 by psirault          #+#    #+#             */
/*   Updated: 2025/04/29 09:54:14 by psirault         ###   ########.fr       */
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
	if (!ft_isalpha(*name) && *name != '_')
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

int	export_len(char *arg)
{
	int	i;

	i = 0;
	if (arg == NULL)
		return (-1);
	while (arg[i] && arg[i] != '=')
		i++;
	return (i);
}

char	*parse_export1(char *arg)
{
	char	*res;
	int		i;

	if (export_len(arg) <= 0 || arg == NULL)
		return (NULL);
	res = (char *)malloc(sizeof(char) * (export_len(arg) + 1));
	if (!res)
		return (NULL);
	i = 0;
	while (arg[i] && arg[i] != '=')
	{
		res[i] = arg[i];
		i++;
	}
	res[i] = '\0';
	if (!is_valid_identifier(res))
	{
		free(res);
		ft_putstr_fd("minishell: export: '", 2);
		ft_putstr_fd(arg, 2);
		ft_putstr_fd("': not a valid identifier\n", 2);
		return (NULL);
	}
	return (res);
}

char	*parse_export2(char *arg)
{
	int		i;
	int		j;
	char	*res;

	if (arg == NULL)
		return (NULL);
	i = export_len(arg);
	if (arg[i] != '=')
		return (NULL);
	j = 0;
	res = (char *)malloc(sizeof(char) * (ft_strlen(arg) - i));
	if (!res)
		return (NULL);
	i++;
	while (arg[i] != '\0')
	{
		res[j] = arg[i];
		i++;
		j++;
	}
	res[j] = '\0';
	return (res);
}
