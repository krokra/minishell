/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: psirault <psirault@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/27 08:16:26 by psirault          #+#    #+#             */
/*   Updated: 2025/06/08 10:07:03 by psirault         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/lexer.h"
#include "../../includes/minishell.h"
#include "../../includes/pipex.h"

void    close_all_except_std(void)
{
	int fd;

	fd = 3;
    while (fd < 1024)
	{
		close(fd);
		fd++;
	}
}

static int	is_stdout_redirected(t_token *tokens)
{
	t_token	*current;

	current = tokens;
	while (current && current->type != T_PIPE)
	{
		if ((current->type == T_REDIR_OUT || current->type == T_APPEND)
			&& current->next && current->next->type == T_WORD)
		{
			return (1);
		}
		current = current->next;
	}
	return (0);
}

void	handle_stdout_redirection(t_token *current)
{
	char	*filename;
	int		flags;
	int		fd;

	filename = current->next->content;
	if (current->type == T_APPEND)
		flags = O_WRONLY | O_CREAT | O_APPEND;
	else
		flags = O_WRONLY | O_CREAT | O_TRUNC;
	fd = open(filename, flags, 0644);
	if (fd == -1)
	{
		perror("minishell: open");
		exit(1);
	}
	if (dup2(fd, STDOUT_FILENO) == -1)
	{
		perror("minishell: dup2");
		close(fd);
		exit(1);
	}
	close(fd);
}
static int	handle_pipe_redirections(t_token *tokens, int *prev_pipe_read, char **env, t_data *data)
{
	t_token	*current;

	current = tokens;
	while (current && current->type != T_PIPE)
	{
		if (current->type == T_REDIR_IN && current->next)
		{
			if (handle_input_redirection(current) == -1)
			{
				cleanup(NULL, env, data->tokens, data);
				return (-1);
			}
			if (prev_pipe_read && *prev_pipe_read != -1)
			{
				close(*prev_pipe_read);
				*prev_pipe_read = -1;
			}
		}
		else if ((current->type == T_REDIR_OUT || current->type == T_APPEND)
			&& current->next && current->next->type == T_WORD)
		{
			handle_stdout_redirection(current);
		}
		current = current->next;
	}
	return (0);
}

static t_token	*find_command_start_from_segment(t_token *current_segment_token)
{
	t_token	*current;

	current = current_segment_token;
	while (current != NULL && current->type != T_PIPE)
	{
		if (current->type == T_REDIR_IN || current->type == T_REDIR_OUT
			|| current->type == T_APPEND || current->type == T_HEREDOC)
		{
			if (current->next != NULL && current->next->type == T_WORD)
			{
				current = current->next->next;
				continue ;
			}
			else
				return (NULL);
		}
		else if (current->type == T_WORD || current->type == T_ENVVAR)
			return (current);
		else
			return (NULL);
	}
	return (NULL);
}

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

static void    exec_cmd_common(char **cmdtab, char **env, t_data *data)
{
    char    *path;

    path = path_of_cmd(cmdtab[0], ft_get_paths("PATH", env));
    close_all_except_std();
    if (path == NULL || ft_strchr(path, '/') == 0 || (path != NULL && access(path, X_OK) == -1))
    {
        if (path != NULL && access(path, X_OK) == -1)
            printf("minishell: permission denied: %s\n", cmdtab[0]);
        else
            printf("minishell: command not found: %s\n", cmdtab[0]);
        cleanup(cmdtab, env, data->tokens, data);
        if (path != NULL && access(path, X_OK) == -1)
		{
			free(path);
            exit(126);
		}
		else if (path)
			free(path);
        exit(127);
    }
    else if (execve(path, cmdtab, env) == -1)
    {
        ft_putstr_fd("minishell: error executing command: ", 2);
        ft_putstr_fd_nl(cmdtab[0], 2);
        free(path);
        cleanup(cmdtab, env, data->tokens, data);
        exit(126);
    }
}

static void	dup2_and_close(int oldfd, int newfd)
{
	if (dup2(oldfd, newfd) < 0)
	{
		perror("minishell: dup2");
		exit(1);
	}
	close(oldfd);
}

static void	perror_exit(const char *msg)
{
	perror(msg);
	exit(1);
}

void	wait_for_children(pid_t *pids, int n_cmds, t_data *data)
{
	int	i;
	int	sig;

	i = 0;
	while (i < n_cmds)
	{
		waitpid(pids[i], &data->status_getter, 0);
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

	if (handle_pipe_redirections(meta->cmds[meta->i], &meta->prev_pipe_read, env, data) == -1)
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
	if (cmd && handle_builtins(envp, cmd, data))
	{
		status = data->exit_status;
		cleanup(NULL, envp, data->tokens, data);
		free(meta->cmds);
		exit(status);
	}
	start = find_command_start_from_segment(meta->cmds[meta->i]);
	argv = build_argv_from_tokens(start, 0, 0);
	if (!argv)
		return (-1);
	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);
	free(meta->cmds);
	if (argv[0])
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

		if (meta->pid == 0) // CHILD
			handle_child_exec(meta, data, envp);

		// PARENT
		meta->pids[meta->i] = meta->pid;

		// Ferme l'ancien pipe read (plus utilisé)
		if (meta->prev_pipe_read != -1)
			close(meta->prev_pipe_read);

		// Ferme l'écriture actuelle (child l'a déjà utilisé)
		if (meta->has_next)
			close(meta->pipefd[1]);

		// Prépare le prochain pipe read
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
	wait_for_children(meta.pids, meta.n_cmds, data);
	signal(SIGINT, sigint_prompt);
	free(meta.cmds);
	if (meta.prev_pipe_read != -1)
		close(meta.prev_pipe_read);
}
