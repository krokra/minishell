/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_vars_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbariol- <nassimbariol@student.42.fr>>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/04 11:18:50 by psirault          #+#    #+#             */
/*   Updated: 2025/06/06 15:17:57 by nbariol-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

static void	expanded_handling(char *exp, t_token *new_tokens, t_token *current)
{
	(void)new_tokens;
	if (exp)
	{
		if (current->quotes == '"')
		{
			free(current->content);
			current->content = exp;
		}
		else if (ft_strchr(exp, ' '))
		{
			new_tokens = lexer(exp, 0);
			if (new_tokens)
			{
				current->content = new_tokens->content;
				current->next = new_tokens->next;
				free(exp);
				free(new_tokens);
			}
		}
		else
		{
			free(current->content);
			current->content = exp;
		}
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
