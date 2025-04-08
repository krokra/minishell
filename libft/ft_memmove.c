/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memmove.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: psirault <psirault@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/12 11:02:28 by psirault          #+#    #+#             */
/*   Updated: 2024/11/13 13:39:45 by psirault         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stddef.h>

void	*ft_memmove(void *dest, const void *src, size_t n)
{
	char		*destf;
	const char	*ssrc;
	size_t		i;

	i = 0;
	destf = (char *)dest;
	ssrc = (char *)src;
	if (!destf && !ssrc)
		return (0);
	if (destf > ssrc)
	{
		i = n;
		while (i--)
		{
			destf[i] = ssrc[i];
		}
		return (dest);
	}
	while (i < n)
	{
		destf[i] = ssrc[i];
		i++;
	}
	return (dest);
}
