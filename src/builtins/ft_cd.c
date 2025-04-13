/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_cd.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: psirault <psirault@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/08 09:27:58 by psirault          #+#    #+#             */
/*   Updated: 2025/04/08 11:41:06 by psirault         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

void	ft_cd(char *str)
{
	if (str == NULL)
		if (chdir(getenv("HOME")) != -1)
		{
			oldpwd_update(env, getenv("PWD"));
			pwd_update(env, getenv("HOME"));
			return ;
		}
	if (chdir(str) != -1 && str != NULL)
	{
		oldpwd_update(env, getenv("PWD"));
		pwd_update(env, str);
		return ;
	}
	else
		perror("chdir() error");
}
