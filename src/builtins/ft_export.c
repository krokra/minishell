/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_export.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/08 09:28:27 by psirault          #+#    #+#             */
/*   Updated: 2025/04/14 19:33:25 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"	

int export_len(char *arg)
{
    int i;

    i = 0;
    while(arg[i] && arg[i] != '=')
        i++;
    return (i);
}

char *parse_export1(char *arg)
{
    char    *res;
    int     i;

    i = 0;  
    res = malloc(sizeof(char) * (export_len(arg) + 1));
    if (!res)
    {
        free(res);
        return (NULL);
    }
    while (arg[i] && arg[i] != '=')
    {
        res[i] = arg[i];
        i++;
    }
    res[i] = '\0';
    return (res);
}

char *parse_export2(char *arg)
{
    int     i;
    int     j;
    char    *res;

    i = export_len(arg);
    j = 0;
    res = malloc (sizeof(char) * (ft_strlen(arg) - i + 1));
    if (!res)
    {
        free(res);
        return (NULL);
    }
    i++;
    while (arg[i])
    {
        res[j] = arg[i];
        i++;
        j++;
    }
    res[j] = '\0';
    return (res);
}

char *export_var(char *name, char *value)
{
    char    *var;

    var = ft_strjoin(name, "=");
    var = ft_strjoin(var, value);
    if (!var)
    {
        free(name);
        free(value);
        return (NULL);
    }
    return (var);
}

char    **ft_export(char **env, char *arg)
{
    char    *name;
    char    *value;
    char    *var;
    int     i;

    i = -1;
    name = parse_export1(arg);
    value = parse_export2(arg);
    var = export_var(name, value);
    if (!name || !var || !value)
        return (NULL);
    while (env[++i] != NULL)
    {
        if (ft_strncmp(env[++i], name, export_len(arg)) == 0)
        {
            free(env[++i]);
            env[++i] = var;
            free(name);
            free(value);
            return (env);
        }
    }
    env[i] = var;
    env[i + 1] = NULL;
    return (env);
}
