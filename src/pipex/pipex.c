/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/27 08:16:26 by psirault          #+#    #+#             */
/*   Updated: 2025/04/14 20:45:36 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/pipex.h"

void	exec_cmd(char *cmd, char **env)
{
	char	*path;
	char	**cmdtab;

	cmdtab = ft_split(cmd, ' ');
	path = path_of_cmd(cmdtab[0], ft_get_paths("PATH", env));
	if (cmdtab == NULL || path == NULL)
	{
		if (cmdtab != NULL && cmdtab[0] != NULL)
		{
			ft_putstr_fd("command not found : ", 2);
			ft_putstr_fd(append_newline(cmdtab[0]), 2);
		}
		free(path);
		ft_free(cmdtab);
		exit(1);
	}
	if (execve(path, cmdtab, env) == -1)
	{
		ft_putstr_fd("command not found : ", 2);
		ft_putstr_fd(append_newline(cmdtab[0]), 2);
		free(path);
		ft_free(cmdtab);
		exit(1);
	}
}

void	exec_child(int *pipefd, char **argv, char **env)
{
	int	fd;

	fd = open(argv[1], O_RDONLY, 0777);
	if (fd == -1 || argv[2][0] == '\0')
		return ;
	dup2(fd, 0);
	dup2(pipefd[1], 1);
	close(pipefd[0]);
	close(pipefd[1]);
	close(fd);
	exec_cmd(argv[2], env);
}

void	exec_parent(int *pipefd, char **argv, char **env)
{
	int	fd;

	fd = open(argv[4], O_WRONLY | O_CREAT | O_TRUNC, 0777);
	if (fd == -1 || argv[3][0] == '\0')
		return ;
	dup2(fd, 1);
	dup2(pipefd[0], 0);
	close(pipefd[1]);
	close(pipefd[0]);
	close(fd);
	exec_cmd(argv[3], env);
}

void	fork_error(int	*pipefd)
{
	close (pipefd[0]);
	close (pipefd[1]);
	exit(-1);
}

void	pipex(int argc, char **argv, char **env)
{
	int		pipefd[2];
	pid_t	pid1;
	pid_t	pid2;

	if (argc != 5)
		wrong_args();
	if (pipe(pipefd) == -1)
		exit(-1);
	pid1 = fork();
	if (pid1 == -1)
		fork_error(pipefd);
	if (pid1 == 0)
		exec_child(pipefd, argv, env);
	pid2 = fork();
	if (pid2 == -1)
		fork_error(pipefd);
	if (pid2 == 0)
		exec_parent(pipefd, argv, env);
	close(pipefd[1]);
	close(pipefd[0]);
	waitpid(pid1, NULL, 0);
	waitpid(pid2, NULL, 0);
}
