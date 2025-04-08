/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memcmp.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/09 12:05:29 by marvin            #+#    #+#             */
/*   Updated: 2024/11/13 15:26:21 by psirault         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

//#include <stdio.h>
#include <stddef.h>
//#include <string.h>

int	ft_memcmp(const void *s1, const void *s2, size_t n)
{
	size_t			i;
	unsigned char	*str1;
	unsigned char	*str2;

	if (n == 0)
		return (0);
	i = 0;
	str1 = (unsigned char *)s1;
	str2 = (unsigned char *)s2;
	while (i < n - 1 && *str1 == *str2)
	{
		++str1;
		++str2;
		i++;
	}
	return (*str1 - *str2);
}
/*
int main () 
{
   char str1[15];
   char str2[15];
   int ret;
   int ret2;

   memcpy(str1, "abcdeF", 6);
   memcpy(str2, "abcdef", 6);

   ret = memcmp(str1, str2, 6);
   ret2 = ft_memcmp(str1, str2, 6);

   if(ret > 0) {
      printf("str2 is less than str1");
   } 
   else if(ret < 0) {
      printf("str1 is less than str2");
   } 
   else {
      printf("str1 is equal to str2");
   }
    printf("\n");
   if(ret2 > 0) {
      printf("str2 is less than str1");
   } 
   else if(ret2 < 0) {
      printf("str1 is less than str2");
   } 
   else {
      printf("str1 is equal to str2");
   }
   return(0);
}
*/
