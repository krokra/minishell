/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex5.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbariol- <nassimbariol@student.42.fr>>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/09 13:22:25 by nbariol-          #+#    #+#             */
/*   Updated: 2025/06/09 15:40:26 by nbariol-         ###   ########.fr       */
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
		exit(is_perm ? 126 : 127);
	}
	if (execve(path, cmdtab, env) == -1)
	{
		ft_putstr_fd("minishell: error executing command: ", 2);
		ft_putstr_fd_nl(cmdtab[0], 2);
		free(path);
		cleanup(cmdtab, env, data->tokens, data);
		exit(126);
	}
}

void	dup2_and_close(int oldfd, int newfd)
{
	if (dup2(oldfd, newfd) < 0)
	{
		perror("minishell: dup2");
		exit(1);
	}
	close(oldfd);
}

void	perror_exit(const char *msg)
{
	perror(msg);
	exit(1);
}

void	wait_for_children(t_execmeta *meta, t_data *data)
{
	int	i;
	int	sig;

	i = 0;
	free(meta->cmds);
	while (i < meta->n_cmds)
	{
		waitpid(meta->pids[i], &data->status_getter, 0);
		if (WIFEXITED(data->status_getter))
			data->exit_status = WEXITSTATUS(data->status_getter);
		else if (WIFSIGNALED(data->status_getter))
		{
			sig = WTERMSIG(data->status_getter);
			data->exit_status = 128 + sig;
			if (sig == SIGINT)
				write(1, "\n", 1);
		}
		i++;
	}
}