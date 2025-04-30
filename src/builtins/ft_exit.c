/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_exit.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: psirault <psirault@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/08 09:28:18 by psirault          #+#    #+#             */
/*   Updated: 2025/04/30 08:34:48 by psirault         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

void	ft_exit(char **argv, char **envp, t_token *tokens)
{
	ft_free(argv);
	ft_free(envp);
	free_tokens(tokens);
	exit(0);
}
