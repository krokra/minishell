/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strchr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: psirault <psirault@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/07 23:09:31 by psirault          #+#    #+#             */
/*   Updated: 2025/06/04 21:07:55 by psirault         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strchr(const char *s, int c)
{
	if (s == NULL)
		return (0);
	if (ft_strncmp(s, "//", 2) == 0 || ft_strncmp(s, "/", 2) == 0)
		return (NULL);
	while (*s != '\0')
	{
		if (*s == (char)c)
			return ((char *)(s));
		s++;
	}
	if ((char)c == '\0' && *s == '\0')
		return ((char *)(s));
	return (0);
}
