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

char *strjoin_and_free_s1(char *s1, const char *s2)
{
    char *joined_str;

    if (!s1 && !s2)
        return (ft_strdup(""));
    if (!s1)
        return (ft_strdup(s2));
    if (!s2)
        return (s1);
    joined_str = ft_strjoin(s1, s2);
    free(s1);
    return (joined_str);
}

static int remove_quotes(char *arg)
{
    int len = ft_strlen(arg);
    if (len > 1 && ((arg[0] == '\'' && arg[len - 1] == '\'') || (arg[0] == '\"' && arg[len - 1] == '\"')))
    {
        char quote_char = arg[0]; 
        ft_memmove(arg, arg + 1, len - 2);
        arg[len - 2] = '\0';
        return (quote_char == '\'');
    }
    return (0);
}

static int is_valid_var_char(char c)
{
    return (ft_isalnum(c) || c == '_');
}

static size_t get_var_name_len(char *str)
{
    if (str[0] == '?')
        return 1;
    size_t len = 0;
    while (str[len] && is_valid_var_char(str[len]))
        len++;
    return len;
}

static char *get_env_value(char *var_name, char **envp)
{
    char *value;

    value = ft_getenv(envp, var_name);
    if (value)
        return (ft_strdup(value));
    else
        return (ft_strdup(""));
}

static char *replace_vars_in_str(char *str, char **envp)
{
    char *result;
    char *start;
    char *var_name;
    char *value;
    char *tmp;

    result = ft_strdup("");
    while (*str)
    {
        start = str;
        while (*str && *str != '$')
            str++;
        if (str > start)
        {
            tmp = ft_substr(start, 0, str - start);
            result = strjoin_and_free_s1(result, tmp);
            free(tmp);
        }
        if (*str == '$')
        {
            str++;
            size_t var_len = get_var_name_len(str);
            if (var_len > 0)
            {
                var_name = ft_substr(str, 0, var_len);
                value = get_env_value(var_name, envp);
                result = strjoin_and_free_s1(result, value);
                free(var_name);
                free(value);
                str += var_len;
            }
            else
                result = strjoin_and_free_s1(result, "$");
        }
    }
    return result;
}

void replace_env_vars(t_token *tokens, char **envp)
{
    while (tokens)
    {
        if ((tokens->type == T_ENVVAR || tokens->type == T_WORD) && tokens->content && tokens->content[0])
        {
            if (!remove_quotes(tokens->content))
            {
                char *new_content = replace_vars_in_str(tokens->content, envp);
                free(tokens->content);
                tokens->content = new_content;
            }
        }
        tokens = tokens->next;
    }
}

// void    replace_env_vars(t_token *tokens, char **args, char **envp)
// {
//     t_token    *current;

//     current = tokens;
//     while (current != NULL)
//     {
//         if (current->type == T_ENVVAR || current->type == T_WORD)
//         {
//             if (remove_quotes(args[current->index]) == 0)
//                 replace_by_value(&args[current->index], envp);
//             else
//                 return ;
//         }
//         current = current->next;
//     }
// }
