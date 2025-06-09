/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex5.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: psirault <psirault@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/09 13:22:25 by nbariol-          #+#    #+#             */
/*   Updated: 2025/06/09 17:45:43 by psirault         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

void	cleanup(char **cmdtab, char **env, t_token *tokens, t_data *data)
{
	if (data)
	{
		free(data);
		data = NULL;
	}
	if (cmdtab)
	{
		ft_free(cmdtab);
		cmdtab = NULL;
	}
	if (env)
	{
		ft_free(env);
		env = NULL;
	}
	if (tokens)
	{
		if (tokens->content)
		{
			free_tokens(tokens);
		}
		tokens = NULL;
	}
	if (tokens)
		free_tokens(tokens);
}

static void	exec_cmd_common_exec(char **cmdtab, char **env,
	t_data *data, char *path)
{
	if (execve(path, cmdtab, env) == -1)
	{
		ft_putstr_fd("minishell: error executing command: ", 2);
		ft_putstr_fd_nl(cmdtab[0], 2);
		free(path);
		cleanup(cmdtab, env, data->tokens, data);
		exit(126);
	}
}

void	exec_cmd_common(char **cmdtab, char **env, t_data *data)
{
	char	*path;
	int		is_perm;

	path = path_of_cmd(cmdtab[0], ft_get_paths("PATH", env));
	close_all_except_std();
	is_perm = (path && access(path, X_OK) == -1);
	if (!path || !ft_strchr(path, '/') || is_perm)
	{
		if (is_perm)
			printf("minishell: permission denied: %s\n", cmdtab[0]);
		else
			printf("minishell: command not found: %s\n", cmdtab[0]);
		cleanup(cmdtab, env, data->tokens, data);
		if (path)
			free(path);
		if (is_perm)
			exit(126);
		else
			exit(127);
	}
	else
		exec_cmd_common_exec(cmdtab, env, data, path);
}
