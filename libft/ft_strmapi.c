/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strmapi.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: psirault <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/13 10:20:19 by psirault          #+#    #+#             */
/*   Updated: 2024/11/15 12:30:15 by psirault         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include "libft.h"

/*
char	f(unsigned int i, char c)
{
	(void)i;
	c += 1;
	return (c);
}
*/
char	*ft_strmapi(char const *s, char (*f)(unsigned int, char))
{
	int		i;
	char	*tab;

	i = 0;
	tab = (char *)malloc(sizeof(char) * ft_strlen(s) + 1);
	if (!tab)
		return (NULL);
	while (s[i] != '\0')
	{
		tab[i] = (*f)(i, s[i]);
		i++;
	}
	tab[i] = '\0';
	return (tab);
}
/*
#include <stdio.h>
int	main()
{
	printf("%s", ft_strmapi("abc", (*f)));
}*/
