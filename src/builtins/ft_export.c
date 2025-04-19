/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_export.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: psirault <psirault@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/08 09:28:27 by psirault          #+#    #+#             */
/*   Updated: 2025/04/18 18:48:46 by psirault         ###   ########.fr       */
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
    res = (char *)malloc(sizeof(char) * (export_len(arg) + 1));
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
    if (arg[i] != '=')
        return (NULL);

    j = 0;
    res = (char *)malloc(sizeof(char) * (ft_strlen(arg) - i));
    if (!res)
        return (NULL);

    i++;
    while (arg[i] != '\0')
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
void    ft_export(char **env, char *arg)
{
    char    *name;
    char    *value;
    char    *var;
    int     i;

    i = 0;
    name = parse_export1(arg);
    value = parse_export2(arg);
    if (!name || !value)
    {
        free(name);
        return ;
    }
    var = export_var(name, value);
    if (!var)
    {
        free(name);
        free(value);
        free(var);
        return ;
    }
    while (env[i] != NULL)
    {
        if (ft_strncmp(env[i], name, ft_strlen(name)) == 0)
        {
            free(env[i]);
            env[i] = ft_strdup(var);
            free(name);
            free(value);
            free(var);
            return ;
        }
        i++;
    }
    env = ft_realloc(env, sizeof(char *) * (i + 1), sizeof(char *) * (i + 2));
    env[i] = var;
    env[i + 1] = NULL;
    free(name);
    free(new_env);
    free(value);
    free(var);
}
