/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strtrim.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: psirault <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/13 09:51:47 by psirault          #+#    #+#             */
/*   Updated: 2024/11/15 16:22:54 by psirault         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include "libft.h"

static int	is_set(char c, char const *set)
{
	int	i;

	i = 0;
	while (set[i] != '\0')
	{
		if (c == set[i])
			return (1);
		i++;
	}
	return (0);
}

char	*ft_strtrim(char const *s1, char const *set)
{
	int		i;
	int		j;
	int		k;
	char	*tab;

	i = 0;
	j = ft_strlen(s1) - 1;
	k = 0;
	while (is_set(s1[i], set) == 1)
		i++;
	while (is_set(s1[j], set) == 1)
		j--;
	if (i > j)
		return (ft_strdup(""));
	tab = (char *)malloc(sizeof(char) * ((j - i) + 2));
	if (!tab)
		return (NULL);
	while (i <= j)
	{
		tab[k] = s1[i];
		k++;
		i++;
	}
	tab[k] = '\0';
	return (tab);
}
/*
#include <stdio.h>

int	main()
{
	printf("|%s|", ft_strtrim("", ""));
}*/
