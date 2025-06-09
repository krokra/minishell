/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex4.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbariol- <nassimbariol@student.42.fr>>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/09 13:21:01 by nbariol-          #+#    #+#             */
/*   Updated: 2025/06/09 13:21:04 by nbariol-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

void	setup_fds_and_redirections(t_execmeta *meta, char **env, t_data *data)
{
	int	fd;

	if (meta->has_next && !is_stdout_redirected(meta->cmds[meta->i]))
		dup2_and_close(meta->pipefd[1], STDOUT_FILENO);
	if (meta->prev_pipe_read != -1)
		dup2_and_close(meta->prev_pipe_read, STDIN_FILENO);
	fd = get_heredoc_fd_from_segment(meta->cmds[meta->i]);
	if (fd != -1)
		dup2_and_close(fd, STDIN_FILENO);
	if (handle_pipe_redirections(meta->cmds[meta->i], &meta->prev_pipe_read,
			env, data) == -1)
	{
		free(meta->cmds);
		exit(1);
	}
}

int	exec_or_builtin(t_execmeta *meta, t_data *data, char **envp)
{
	t_token	*cmd;
	t_token	*start;
	char	**argv;
	int		status;

	cmd = find_command_start_from_segment(meta->cmds[meta->i]);
	close_all_except_std();
	if (cmd && is_builtin(cmd->content))
	{
		free(meta->cmds);
		handle_builtins(envp, cmd, data);
		status = data->exit_status;
		cleanup(NULL, envp, data->tokens, data);
		exit(status);
	}
	start = find_command_start_from_segment(meta->cmds[meta->i]);
	argv = build_argv_from_tokens(start, 0, 0);
	if (!argv)
		return (-1);
	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);
	free(meta->cmds);
	exec_cmd_common(argv, envp, data);
	cleanup(argv, envp, data->tokens, data);
	exit(1);
}

void	handle_child_exec(t_execmeta *meta, t_data *data, char **envp)
{
	setup_fds_and_redirections(meta, envp, data);
	if (exec_or_builtin(meta, data, envp) == -1)
		return ;
}

void	while_exec(t_execmeta *meta, t_data *data, char **envp)
{
	while (meta->i < meta->n_cmds)
	{
		meta->has_next = (meta->i < meta->n_cmds - 1);
		if (meta->has_next && pipe(meta->pipefd) < 0)
			perror_exit("minishell: pipe");
		signal(SIGINT, SIG_IGN);
		meta->pid = fork();
		if (meta->pid < 0)
			perror_exit("minishell: fork");
		if (meta->pid == 0)
			handle_child_exec(meta, data, envp);
		meta->pids[meta->i] = meta->pid;
		if (meta->prev_pipe_read != -1)
			close(meta->prev_pipe_read);
		if (meta->has_next)
			close(meta->pipefd[1]);
		meta->prev_pipe_read = (meta->has_next) ? meta->pipefd[0] : -1;
		meta->i++;
	}
}

void	exec_cmd_tokens(t_data *data, char **envp)
{
	t_execmeta	meta;

	meta.i = 0;
	meta.prev_pipe_read = -1;
	meta.n_cmds = 0;
	meta.cmds = split_tokens_by_pipe(data->tokens, &meta.n_cmds, 0, 1);
	if (!meta.cmds)
		return ;
	while_exec(&meta, data, envp);
	wait_for_children(&meta, data);
	signal(SIGINT, sigint_prompt);
	if (meta.prev_pipe_read != -1)
		close(meta.prev_pipe_read);
}