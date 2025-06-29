/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_vars.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: psirault <psirault@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/01 11:04:50 by psirault          #+#    #+#             */
/*   Updated: 2025/06/04 13:22:08 by psirault         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

char	*strjoin_and_free_s1(char *s1, const char *s2)
{
	char	*joined_str;

	if (!s1 && !s2)
		return (ft_strdup(""));
	if (!s1)
		return (ft_strdup(s2));
	if (!s2)
		return (s1);
	joined_str = ft_strjoin(s1, s2);
	free(s1);
	return (joined_str);
}

static char	*remove_quotes_loop(char *str, char *res, int qchar, int in_quote)
{
	int	i;
	int	j;

	i = -1;
	j = 0;
	while (str[++i])
	{
		if ((str[i] == '"' || str[i] == '\'') && !in_quote)
		{
			in_quote = 1;
			qchar = str[i];
			continue ;
		}
		if (str[i] == qchar && in_quote)
		{
			in_quote = 0;
			continue ;
		}
		res[j] = str[i];
		j++;
	}
	res[j] = '\0';
	return (res);
}

char	*remove_quotes(char *str)
{
	char	*result;

	result = (char *)malloc(ft_strlen(str) + 1);
	if (!result)
		return (NULL);
	return (remove_quotes_loop(str, result, 0, 0));
}
