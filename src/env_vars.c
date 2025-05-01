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

void	replace_by_value(char **arg, char **envp)
{
    char *value;

    value = ft_getenv(envp, (*arg) + 1);
    if (value == NULL)
        return ;
    char *new_value = ft_strdup(value);
    if (new_value == NULL)
    {
        perror("Error allocating memory");
        exit(EXIT_FAILURE);
    }
    free(*arg);
    *arg = new_value;
}

void	replace_env_vars(t_token *tokens, char **args, char **envp)
{
    t_token	*current;

    current = tokens;
    while (current != NULL)
    {
        if (current->type == T_ENVVAR)
            replace_by_value(&args[current->index], envp);
        current = current->next;
    }
}
