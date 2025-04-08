/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_substr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: psirault <psirault@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/12 15:02:25 by psirault          #+#    #+#             */
/*   Updated: 2024/11/15 13:08:15 by psirault         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stddef.h>
#include <stdlib.h>
#include "libft.h"

char	*ft_substr(char const *s, unsigned int start, size_t len)
{
	char	*tab;
	size_t	i;
	size_t	slen;

	i = 0;
	slen = ft_strlen(s);
	if (start >= slen)
	{
		return (ft_strdup(""));
	}
	if (slen - start < len)
		len = slen - start;
	tab = (char *)malloc(sizeof(char) * len + 1);
	if (!tab || !s)
		return (NULL);
	while (s[start] != '\0' && i < len)
	{
		tab[i] = s[start];
		i++;
		start++;
	}
	tab[len] = '\0';
	return (tab);
}
/*
#include <stdio.h>
int	main()
{
	printf("%s", ft_substr("hola", 0, 18446744073709551615));
}*/
