/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_vars.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: psirault <psirault@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/01 11:04:50 by psirault          #+#    #+#             */
/*   Updated: 2025/05/29 14:30:29 by psirault         ###   ########.fr       */
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
    char *result = malloc(len + 1);
    int in_quote = 0;
    char quote_char = 0;

    if (!result)
        return (NULL);

    while (str[i])
    {
        if ((str[i] == '"' || str[i] == '\'') && !in_quote)
        {
            in_quote = 1;
            quote_char = str[i];
            i++;
            continue;
        }
        if (str[i] == quote_char && in_quote)
        {
            in_quote = 0;
            i++;
            continue;
        }
        result[j] = str[i];
        j++;
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
    t_token *current = tokens;
	char *expanded;
	
    while (current)
    {
        printf("Processing token: %s (quote: %c)\n", current->content, current->quotes);
        
        // Si c'est une quote simple, on ne fait pas d'expansion
        if (current->quotes == '\'')
        {
            printf("Single quote detected, skipping expansion\n");
            current = current->next;
            continue;
        }
        
        // Pour les doubles quotes ou sans quote, on fait l'expansion
        expanded = replace_vars_in_str(current->content, envp, data);
        if (expanded)
        {
            // Si le contenu expansé contient des espaces, on le re-tokenise
            if (ft_strchr(expanded, ' '))
            {
                t_token *new_tokens = lexer(expanded, 0);
                if (new_tokens)
                {
                    // On remplace le token actuel par les nouveaux tokens
                    current->content = new_tokens->content;
                    current->next = new_tokens->next;
                    free(expanded);
                    free_tokens(new_tokens);
                }
            }
            else
            {
                free(current->content);
                current->content = expanded;
            }
        }
        current = current->next;
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

static int is_valid_shlvl(const char *value)
{
    int i = 0;
    
    if (!value)
        return (0);
    while (value[i])
    {
        if (!ft_isdigit(value[i]))
            return (0);
        i++;
    }
    return (1);
}

void handle_shlvl(char **envp)
{
    char *shlvl_value;
    int shlvl;
    char *new_value;
    
    shlvl_value = ft_getenv(envp, "SHLVL");
    if (!shlvl_value || !is_valid_shlvl(shlvl_value))
        shlvl = 1;
    else
    {
        shlvl = ft_atoi(shlvl_value) + 1;
        if (shlvl > 999)
            shlvl = 1;
    }
    new_value = ft_itoa(shlvl);
    if (new_value)
    {
        if (shlvl_value)
            ft_setenv(envp, "SHLVL", new_value, 1);
        else
            ft_setenv(envp, "SHLVL", new_value, 0);
        free(new_value);
    }
}

int ft_setenv(char **env, const char *name, const char *value, int overwrite)
{
    int i;
    char *new_entry;
    char **new_env;

    if (!name || !value)
        return (-1);
    
    // Vérifier si la variable existe déjà
    i = 0;
    while (env[i])
    {
        if (ft_strncmp(env[i], name, ft_strlen(name)) == 0 && env[i][ft_strlen(name)] == '=')
        {
            if (!overwrite)
                return (0);
            new_entry = ft_strjoin(name, "=");
            new_entry = strjoin_and_free_s1(new_entry, value);
            free(env[i]);
            env[i] = new_entry;
            return (0);
        }
        i++;
    }

    // Si la variable n'existe pas, l'ajouter
    new_entry = ft_strjoin(name, "=");
    new_entry = strjoin_and_free_s1(new_entry, value);
    
    // Trouver la première position NULL dans env
    i = 0;
    while (env[i])
        i++;
    
    // Réallouer env pour ajouter la nouvelle variable
    new_env = ft_realloc(env, (i + 1) * sizeof(char *), (i + 2) * sizeof(char *));
    if (!new_env)
    {
        free(new_entry);
        return (-1);
    }
    
    new_env[i] = new_entry;
    new_env[i + 1] = NULL;
	free(new_entry);
    return (0);
}
