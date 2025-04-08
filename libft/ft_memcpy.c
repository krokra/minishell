/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memcpy.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: psirault <psirault@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/12 10:53:06 by psirault          #+#    #+#             */
/*   Updated: 2024/11/15 14:02:24 by psirault         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stddef.h>

void	*ft_memcpy(void *dest, const void *src, size_t n)
{
	size_t		i;
	char		*destf;
	const char	*ssrc;

	if (!dest && !src)
		return (NULL);
	destf = (char *)dest;
	ssrc = (const char *)src;
	i = 0;
	while (i < n)
	{
		*destf = *ssrc;
		++ssrc;
		++destf;
		i++;
	}
	return (dest);
}
