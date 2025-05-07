/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_vars.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: psirault <psirault@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/01 11:04:50 by psirault          #+#    #+#             */
/*   Updated: 2025/05/01 11:04:50 by psirault         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

void    replace_by_value(char **arg, char **envp)
{
    char *value;
    char *new_value;

    value = ft_getenv(envp, (*arg) + 1);
    if (value == NULL)
        return ;
    new_value = ft_strdup(value);
    if (new_value == NULL)
    {
        perror("Error allocating memory");
        exit(EXIT_FAILURE);
    }
    free(*arg);
    *arg = new_value;
}

static int    remove_quotes(char *arg)
{
    size_t    len;
    int        ret;

    len = ft_strlen(arg);
    if (arg[0] == '\'')
        ret = 1;
    else
        ret = 0;
    if (len > 1 && ((arg[0] == '\'' && arg[len - 1] == '\'') || (arg[0] == '\"' && arg[len - 1] == '\"')))
    {
        arg[len - 1] = '\0';
        ft_memmove(arg, arg + 1, len - 1);
    }
    return (ret);
}

void    replace_env_vars(t_token *tokens, char **envp)
{
    t_token    *current;
    int        single_quoted;

    current = tokens;
    while (current != NULL)
    {
        if (current->type == T_WORD)
        {
            single_quoted = remove_quotes(current->content);

            if (single_quoted == 0)
            {
                if (current->content && current->content[0] == '$')
                {
                    replace_by_value(&current->content, envp);
                }
            }
        }
        current = current->next;
    }
}

/*
void    replace_env_vars(t_token *tokens, char **args, char **envp)
{
    t_token    *current;

    current = tokens;
    while (current != NULL)
    {
        if (current->type == T_ENVVAR || current->type == T_WORD)
        {
            if (remove_quotes(args[current->index]) == 0)
                replace_by_value(&args[current->index], envp);
            else
                return ;
        }
        current = current->next;
    }
}
*/