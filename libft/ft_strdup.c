/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strdup.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/09 14:47:27 by marvin            #+#    #+#             */
/*   Updated: 2024/11/15 15:01:08 by psirault         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include "libft.h"
//#include <string.h>
//#include <assert.h>

char	*ft_strdup(const char *s)
{
	char	*tab;
	int		i;

	tab = (char *)malloc(sizeof(char) * (ft_strlen(s) + 1));
	if (!tab)
		return (NULL);
	i = 0;
	while (s[i])
	{
		tab[i] = s[i];
		i++;
	}
	tab[i] = '\0';
	return (tab);
}
/*
int	main()
{
	assert(strcmp("", strdup("")) == 0);
}*/
