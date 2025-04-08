/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strrchr.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: psirault <psirault@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/08 12:16:18 by psirault          #+#    #+#             */
/*   Updated: 2024/11/15 15:02:02 by psirault         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

static int	count_occ(const char *str, char c)
{
	int	res;
	int	i;

	res = 0;
	i = 0;
	while (str[i] != '\0')
	{
		if (str[i] == c)
			res++;
		i++;
	}
	return (res);
}

char	*ft_strrchr(const char *s, int c)
{
	int	occ;
	int	j;

	occ = count_occ(s, (char)c);
	j = 0;
	while (*s != '\0')
	{
		if (*s == (char)c && occ != j)
		{
			j++;
		}
		if (*s == (char)c && occ == j)
		{
			return ((char *)(s));
		}
		++s;
	}
	if ((char)c == '\0')
		return ((char *)(s));
	return (0);
}
