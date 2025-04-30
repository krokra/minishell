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

int	export_len(char *arg)
{
	int	i;

	i = 0;
	while (arg[i] && arg[i] != '=')
		i++;
	return (i);
}

char	*parse_export1(char *arg)
{
	char	*res;
	int		i;

	i = 0;
	res = (char *)malloc(sizeof(char) * (export_len(arg) + 1));
	if (!res)
	{
		free(res);
		return (NULL);
	}
	while (arg[i] && arg[i] != '=')
	{
		res[i] = arg[i];
		i++;
	}
	res[i] = '\0';
	return (res);
}

char	*parse_export2(char *arg)
{
	int		i;
	int		j;
	char	*res;

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
