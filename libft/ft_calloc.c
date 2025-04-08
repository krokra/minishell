/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_calloc.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: psirault <psirault@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/12 11:13:36 by psirault          #+#    #+#             */
/*   Updated: 2024/11/15 14:13:23 by psirault         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stddef.h>
#include <stdlib.h>
#include "libft.h"

void	*ft_calloc(size_t nmemb, size_t size)
{
	void	*tab;

	tab = (void *)malloc(size * nmemb);
	if (!tab)
		return (NULL);
	ft_memset(tab, 0, size * nmemb);
	return (tab);
}
