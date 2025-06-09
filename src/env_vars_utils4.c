/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_vars_utils4.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: psirault <psirault@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/04 13:25:53 by psirault          #+#    #+#             */
/*   Updated: 2025/06/09 15:56:45 by psirault         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

static int	quote_helper(char **str, t_vars *vars)
{
	if (vars->is_heredoc)
		return (0);
	if ((**str == '"' || **str == '\'') && !vars->in_quote)
	{
		vars->quote_char = **str;
		vars->in_quote = 1;
		(*str)++;
		return (1);
	}
	if (**str == vars->quote_char && vars->in_quote)
	{
		vars->in_quote = 0;
		vars->quote_char = 0;
		(*str)++;
		return (1);
	}
	return (0);
}

static int	quote_helper2(char **str, t_vars *vars)
{
	if (vars->in_quote && vars->quote_char == '\'')
	{
		if (vars->is_heredoc)
			return (0);
		vars->tmp = vars->result;
		vars->result = ft_strjoin(vars->result, (char []){**str, '\0'});
		if (!vars->result)
			return (2);
		free(vars->tmp);
		(*str)++;
		return (1);
	}
	return (0);
}

static int	quote_helper3(char **str, t_vars *vars, char **envp, t_data *data)
{
	if (!vars->is_heredoc && vars->in_quote && vars->quote_char == '\'')
		return (0);
	if (**str == '$' && (*(*str + 1) == '?' || ft_isalnum(*(*str + 1)) || *(*str
				+ 1) == '_'))
	{
		(*str)++;
		vars->var_name = malloc(get_var_name_len(*str) + 1);
		if (!vars->var_name)
			return (2);
		ft_strlcpy(vars->var_name, *str, get_var_name_len(*str) + 1);
		vars->value = get_env_value(vars->var_name, envp, data);
		free(vars->var_name);
		if (!vars->value)
			return (free(vars->value), 2);
		vars->tmp = vars->result;
		vars->result = ft_strjoin(vars->result, vars->value);
		free(vars->value);
		free(vars->tmp);
		if (!vars->result)
			return (2);
		(*str) += get_var_name_len(*str);
		return (1);
	}
	return (0);
}

static char	*free_replace_vars(t_vars *vars)
{
	char	*res;

	res = vars->result;
	free(vars);
	return (res);
}

static int	str_result(t_vars *vars, char c)
{
	vars->tmp = vars->result;
	vars->result = ft_strjoin(vars->result, (char []){c, 0});
	if (!vars->result)
		return (1);
	free(vars->tmp);
	return (0);
}

char	*replace_vars_in_str(t_token *tok, char *str, char **envp, t_data *data)
{
	t_vars	*vars;
	int		i;

	vars = malloc(sizeof(t_vars));
	if (!vars)
		return (NULL);
	vars->result = ft_strdup("");
	if (!vars->result)
		return (free(vars), NULL);
	vars->in_quote = 0;
	vars->quote_char = 0;
	vars->is_heredoc = (tok && tok->type == T_HEREDOC);
	while (*str)
	{
		if (quote_helper(&str, vars) || quote_helper2(&str, vars))
			continue ;
		if (ft_strncmp(str, "$", 2) == 0 && !tok->has_space_after && tok->next)
			return (free_replace_vars(vars));
		i = quote_helper3(&str, vars, envp, data);
		if (i == 2)
			return (free_replace_vars(vars), NULL);
		if (i == 1 || str_result(vars, *str++))
			continue ;
	}
	return (free_replace_vars(vars));
}
