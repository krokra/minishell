/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_itoa.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: psirault <psirault@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/12 16:03:35 by psirault          #+#    #+#             */
/*   Updated: 2024/11/15 12:23:44 by psirault         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>

static int	ft_abs(int n)
{
	if (n < 0)
		return (-n);
	return (n);
}

static int	count_digits(int n)
{
	int	i;

	i = n < 0;
	if (n == 0)
		return (1);
	while (n != 0)
	{
		i++;
		n /= 10;
	}
	return (i);
}

char	*ft_itoa(int n)
{
	int		digits;
	int		i;
	char	*tab;

	digits = count_digits(n);
	tab = (char *)malloc(sizeof(char) * digits + 1);
	if (!tab)
		return (0);
	i = 0;
	if (n < 0)
	{
		tab[0] = '-';
		i++;
	}
	tab[digits] = '\0';
	while (digits > i)
	{
		tab[digits - 1] = ft_abs((n % 10)) + 48;
		n /= 10;
		digits--;
	}
	return (tab);
}
/*
#include <stdio.h>
int	main()
{
	printf("%d", count_digits(-2147483648));
}*/
