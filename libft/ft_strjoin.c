/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strjoin.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: psirault <psirault@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/12 15:21:26 by psirault          #+#    #+#             */
/*   Updated: 2024/11/15 12:33:11 by psirault         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include <stddef.h>
#include "libft.h"

char	*ft_strjoin(char const *s1, char const *s2)
{
	char	*tab;
	int		i;
	int		j;

	i = 0;
	j = ft_strlen(s1);
	tab = (char *)malloc(ft_strlen(s1) + ft_strlen(s2) + 1);
	if (!tab)
	{
		return (0);
	}
	while (s1[i] != '\0')
	{
		tab[i] = s1[i];
		i++;
	}
	i = 0;
	while (s2[i] != '\0')
	{
		tab[j] = s2[i];
		j++;
		i++;
	}
	tab[j] = '\0';
	return (tab);
}
/*
#include <stdio.h>
int	main()
{
	printf("%s", ft_strjoin("caca", "pipi"));
}
*/
