/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strncmp.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: psirault <psirault@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/08 12:33:28 by psirault          #+#    #+#             */
/*   Updated: 2024/11/13 13:40:21 by psirault         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stddef.h>

int	ft_strncmp(const char *s1, const char *s2, size_t n)
{
	size_t			i;
	unsigned char	*us1;
	unsigned char	*us2;

	us1 = (unsigned char *)s1;
	us2 = (unsigned char *)s2;
	if (n == 0)
		return (0);
	i = 0;
	while (*us1 && *us2 && *us1 == *us2 && i < n - 1)
	{
		i++;
		++us1;
		++us2;
	}
	return (*us1 - *us2);
}
