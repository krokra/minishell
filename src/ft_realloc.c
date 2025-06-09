/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_realloc.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+        
	+:+     */
/*   By: psirault <psirault@student.42.fr>          +#+  +:+      
	+#+        */
/*                                                +#+#+#+#+#+  
	+#+           */
/*   Created: 2025/04/18 15:11:57 by psirault          #+#    #+#             */
/*   Updated: 2025/04/18 15:11:57 by psirault         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "../includes/minishell.h"


void	*ft_realloc(void *dst, size_t old_size, size_t new_size)
{
	void *new_ptr;
	size_t copy_size;

	if (new_size == 0)
	{
		free(dst);
		return (NULL);
	}
	new_ptr = malloc(new_size);
	if (!new_ptr)
		return (NULL);
	if (dst != NULL)
	{
		if (old_size < new_size)
		{
			copy_size = old_size;
		}
		else
		{
			copy_size = new_size;
		}
		ft_memcpy(new_ptr, dst, copy_size);
	}
	return (new_ptr);
}
