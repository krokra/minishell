/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_vars.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: psirault <psirault@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/01 11:04:50 by psirault          #+#    #+#             */
/*   Updated: 2025/05/19 13:34:31 by psirault         ###   ########.fr       */
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

char *remove_quotes(char *str)
{
    int i = 0, j = 0;
    int len = ft_strlen(str);
    char *result = malloc(len + 1); // Worst case: no quotes removed

    if (!result)
        return (NULL);

    while (str[i])
    {
        if (str[i] != '"' && str[i] != '\'')
        {
            result[j] = str[i];
            j++;
        }
        i++;
    }
    result[j] = '\0';
    return (result);
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

static char *get_env_value(char *var_name, char **envp, t_data *data)
{
    char *value;

    if (var_name[0] == '?' && var_name[1] == '\0')
        return (ft_itoa(data->exit_status));
    value = ft_getenv(envp, var_name);
    if (value)
        return (ft_strdup(value));
    else
        return (ft_strdup(""));
}

static char *replace_vars_in_str(char *str, char **envp, t_data *data)
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
                value = get_env_value(var_name, envp, data);
                result = strjoin_and_free_s1(result, value);
                free(var_name);
                free(value);
                str += var_len;
            }
            else
                result = strjoin_and_free_s1(result, "$");
        }
    }
    return (result);
}

void replace_env_vars(t_token *tokens, char **envp, t_data *data)
{
    while (tokens)
    {
        // Ne jamais expand si quotes simples
        printf("tokens: %s\n", tokens->content);
        printf("tokens->quotes: %c\n", tokens->quotes);
        if (tokens->quotes == '\'')
        {
            char *stripped = remove_quotes(tokens->content);
            free(tokens->content);
            tokens->content = stripped;
            tokens = tokens->next;
            continue;
        }
        printf("tokens: %s\n", tokens->content);
        // D'abord, on retire les quotes (pas simples)
        char *stripped = remove_quotes(tokens->content);
        free(tokens->content);
        tokens->content = stripped;

        // Puis on fait l'expansion
        char *expanded = replace_vars_in_str(tokens->content, envp, data);
        free(tokens->content);
        tokens->content = expanded;

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
