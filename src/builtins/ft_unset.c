/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_unset.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/08 09:28:01 by psirault          #+#    #+#             */
/*   Updated: 2025/04/14 19:34:16 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

void    ft_unset(char *name, char **env)
{
    int		i;
    int       len;

    len = ft_strlen(name);
    if (name == NULL || ft_strchr(name, '=') != NULL)
        return ;
    i = 0;
    while (env[i] != NULL)
    {
        if (ft_strncmp(env[i], name, ft_strlen(name)) == 0 && env[i][len] == '=')
        {
            free(env[i]);
            while (env[i + 1] != NULL)
            {
                env[i] = env[i + 1];
                i++;
            }
            break ;
        }
    }
}