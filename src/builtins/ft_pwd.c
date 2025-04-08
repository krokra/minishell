/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_pwd.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: psirault <psirault@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/08 09:28:00 by psirault          #+#    #+#             */
/*   Updated: 2025/04/08 09:39:47 by psirault         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

void	ft_pwd(void)
{
	char	*buf;

	buf = getcwd(NULL, 0);
	if (buf != NULL)
		printf("%s", buf);
	else
		perror("getcwd() error");
}
