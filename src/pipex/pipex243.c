/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex243.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: psirault <psirault@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/09 17:45:47 by psirault          #+#    #+#             */
/*   Updated: 2025/06/09 17:45:52 by psirault         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

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
