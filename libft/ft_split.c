/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_split.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: psirault <psirault@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/13 10:41:29 by psirault          #+#    #+#             */
/*   Updated: 2025/03/03 12:57:25 by psirault         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static int	is_sep(char c, char sep)
{
	if (c == sep)
		return (1);
	return (0);
}

void	*ft_free(char **dest)
{
	int	i;

	i = 0;
	while (dest[i])
	{
		free(dest[i]);
		i++;
	}
	free(dest);
	return (NULL);
}

static char	*ft_strdupli(char const *str, int start, int end)
{
	char	*string;
	int		i;

	string = (char *)malloc(sizeof(char) * (end - start + 1));
	if (!string)
	{
		return (NULL);
	}
	i = 0;
	while (str[start] != '\0' && start < end)
	{
		string[i] = str[start];
		start++;
		i++;
	}
	string[i] = '\0';
	return (string);
}

// static int	count_words(char const *s, char sep)
// {
// 	int	i;
// 	int	res;

// 	i = 0;
// 	res = 0;
// 	while (s[i] != '\0')
// 	{
// 		while (is_sep(s[i], sep) == 1 && s[i] != '\0')
// 			i++;
// 		if (s[i] != '\0')
// 			res++;
// 		while (is_sep(s[i], sep) == 0 && s[i] != '\0')
// 			i++;
// 	}
// 	return (res);
// }

char	**ft_split(char const *s, char c)
{
	int		i;
	int		j;
	int		end;
	char	**tab;

	i = 0;
	j = 0;
	tab = (char **)malloc(sizeof(char *) * (ft_count_words(s, c) + 1));
	if (!tab)
		return (NULL);
	while (s[i] != '\0' && j < ft_count_words(s, c))
	{
		while (is_sep(s[i], c) == 1)
			i++;
		end = i;
		while (is_sep(s[end], c) == 0 && s[end] != '\0')
			end++;
		tab[j] = ft_strdupli(s, i, end);
		if (!tab[j])
			return (ft_free(tab));
		i = end;
		j++;
	}
	tab[j] = NULL;
	return (tab);
}
/*
#include <stdio.h>
int	main()
{	
	char **split;
	int	i;
	i = 0;
	split = ft_split("je sais pas", ' ');
	while (split[i] != 0)
	{
		printf("%s\n", split[i]);
		i++;
	}
	return (0);
}*/
