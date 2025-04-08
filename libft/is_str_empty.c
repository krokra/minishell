/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   is_str_empty.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: psirault <psirault@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/03 14:27:39 by psirault          #+#    #+#             */
/*   Updated: 2025/02/03 15:04:46 by psirault         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

int	is_empty(char *str)
{
	int	i;

	i = 0;
	while (str[i] == '+' || str[i] == '-')
		i++;
	if (str[i] == '\0')
		return (1);
	return (0);
}
