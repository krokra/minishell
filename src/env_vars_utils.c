/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_vars_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbariol- <nassimbariol@student.42.fr>>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/04 11:18:50 by psirault          #+#    #+#             */
/*   Updated: 2025/06/06 15:47:16 by nbariol-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

void	free_token_list(t_token *token)
{
	t_token *tmp;

	while (token)
	{
		tmp = token->next;
		free(token->content);
		free(token);
		token = tmp;
	}
}
char	*merge_token_list(t_token *head)
{
	char	*result;
	char	*tmp;
	t_token	*current;

	if (!head || !head->content)
		return (NULL);
	result = ft_strdup(head->content);
	current = head->next;
	while (current)
	{
		tmp = ft_strjoin(result, " ");
		free(result);
		result = ft_strjoin(tmp, current->content);
		free(tmp);
		current = current->next;
	}
	return (result);
}

static void	expanded_handling(char *exp, t_token *new_tokens, t_token *current)
{
	t_token	*tmp_tokens;
	char	*merged;
	(void)new_tokens;
	if (!exp)
		return;
	if (current->quotes == '"')
	{
		free(current->content);
		current->content = exp;
		return;
	}
	if (ft_strchr(exp, ' '))
	{
		tmp_tokens = lexer(exp, 0);
		if (!tmp_tokens)
			return;
		merged = merge_token_list(tmp_tokens);
		free(current->content);
		current->content = merged;
		free_token_list(tmp_tokens); // ← à écrire si pas fait
		free(exp);
	}
	else
	{
		free(current->content);
		current->content = exp;
	}
}

void	replace_env_vars(t_token *tokens, char **envp, t_data *data)
{
	t_token	*current;
	char	*expanded;
	t_token	*new_tokens;

	new_tokens = NULL;
	current = tokens;
	while (current)
	{
		expanded = replace_vars_in_str(current, current->content, envp, data);
		expanded_handling(expanded, new_tokens, current);
		current = current->next;
	}
}
