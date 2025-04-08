/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_count_words.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: psirault <psirault@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/09 08:39:53 by psirault          #+#    #+#             */
/*   Updated: 2024/12/09 09:19:35 by psirault         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

static int	is_sep(char c, char sep)
{
	if (c == sep)
		return (1);
	return (0);
}

int	ft_count_words(char const *s, char sep)
{
	int	i;
	int	res;

	i = 0;
	res = 0;
	while (s[i] != '\0')
	{
		while (is_sep(s[i], sep) == 1 && s[i] != '\0')
			i++;
		if (s[i] != '\0')
			res++;
		while (is_sep(s[i], sep) == 0 && s[i] != '\0')
			i++;
	}
	return (res);
}
