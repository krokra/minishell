/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_isdigit.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: psirault <psirault@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/07 20:28:00 by psirault          #+#    #+#             */
/*   Updated: 2025/02/03 15:29:41 by psirault         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

int	ft_isdigit(int c)
{
	if ((c < 58 && c > 47) || c == '-' || c == '+' || c == ' ')
		return (1);
	return (0);
}

int	ft_isdigit_without_space(int c)
{
	if ((c < 58 && c > 47) || c == '-' || c == '+')
		return (1);
	return (0);
}
