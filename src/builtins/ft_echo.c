/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_echo.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: psirault <psirault@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/08 09:27:57 by psirault          #+#    #+#             */
/*   Updated: 2025/04/08 11:14:00 by psirault         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

int	ft_echo(int argc, char **argv)
{
	int	i;

	i = 0;
	if (argc == 1)
		return (printf("\n"));
	if (ft_strncmp(argv[0], "-n", 3) == 0)
		i++;
	while (argv[i] != NULL)
	{
		printf("%s ", argv[i]);
		i++;
	}
	if (ft_strncmp(argv[0], "-n", 3) == 0)
		;
	else
		printf("\n");
	return (1);
}
