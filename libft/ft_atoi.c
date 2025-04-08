/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_atoi.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: psirault <psirault@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/10 11:28:35 by marvin            #+#    #+#             */
/*   Updated: 2025/01/07 11:51:23 by psirault         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

static int	is_space(char c)
{
	if (c == ' '
		|| c == '\f'
		|| c == '\n'
		|| c == '\r'
		|| c == '\t'
		|| c == '\v')
		return (1);
	return (0);
}

static int	is_sign(char c)
{
	if (c == '-' || c == '+')
		return (1);
	return (0);
}

long	ft_atoi(const char *nptr)
{
	long	sign;
	long	i;
	long	res;

	sign = 1;
	i = 0;
	res = 0;
	while (is_space(nptr[i]) == 1 && nptr[i] != '\0')
		i++;
	if (nptr[i + 1] == '-' || nptr[i + 1] == '+')
		return (0);
	while (is_sign(nptr[i]) == 1 && nptr[i] != '\0')
	{
		if (nptr[i] == '-')
			sign *= -1;
		i++;
	}
	while (nptr[i] >= '0' && nptr[i] <= '9' && nptr[i] != '\0')
	{
		res += nptr[i] - '0';
		if (nptr[i + 1] >= '0' && nptr[i + 1] <= '9')
			res *= 10;
		i++;
	}
	return (res * sign);
}
