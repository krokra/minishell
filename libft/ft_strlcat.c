/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strlcat.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: psirault <psirault@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/10 16:52:17 by marvin            #+#    #+#             */
/*   Updated: 2025/06/04 13:31:08 by psirault         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stddef.h>
#include "libft.h"

size_t	ft_strlcat(char *dst, const char *src, size_t dstsize)
{
	size_t	slen;
	size_t	dlen;

	slen = ft_strlen(src);
	dlen = ft_strlen(dst);
	if (dlen >= dstsize)
		dlen = dstsize;
	if (dlen == dstsize)
		return (dstsize + slen);
	if (slen < dstsize - dlen)
		ft_memcpy(dst + dlen, src, slen + 1);
	else
	{
		ft_memcpy(dst + dlen, src, dstsize - dlen - 1);
		dst[dstsize - 1] = '\0';
	}
	return (dlen + slen);
}
/*
#include <stdio.h>

int main()
{
    char buffer[3] = "eee";
    strlcat(buffer,"eeee", 4);
    printf("%s", buffer);
    printf("%d", strlcat(buffer, "eeee", 4));
}*/
