/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memchr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/09 09:45:38 by marvin            #+#    #+#             */
/*   Updated: 2024/11/13 08:29:59 by psirault         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stddef.h>

void	*ft_memchr(const void *s, int c, size_t n)
{
	unsigned char	*str;
	size_t			i;

	i = 0;
	str = (unsigned char *)s;
	while (i < n)
	{
		if (*str == (unsigned char)c)
			return ((void *)(str));
		++str;
		i++;
	}
	return (0);
}
/*
int main() {
    char data[] = { 10, 20, 30, 40, 50, 60, 70, 80, 90, 100 };
    const unsigned int size = 10;

    void * found = memchr( data, 57, size );
    printf( "57 is %s\n", ( found != NULL ? "found" : "not found" ) );
    void * found2 = ft_memchr( data, 57, size );
    printf( "57 is %s\n", ( found2 != NULL ? "found" : "not found" ) );

    found = memchr( data, 50, size );
    printf( "50 is %s\n", ( found != NULL ? "found" : "not found" ) );
    found2 = ft_memchr( data, 50, size );
    printf( "50 is %s\n", ( found2 != NULL ? "found" : "not found" ) );
    if ( found2 != NULL ) {
        printf( "La valeur à la position calculée est %d\n", *((char *) found) );
    }

    return 0;
}*/
