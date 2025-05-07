/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: psirault <psirault@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/27 08:16:26 by psirault          #+#    #+#             */
/*   Updated: 2025/05/07 14:00:56 by psirault         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/pipex.h"

static int is_token_argument(t_token *token)
{
    return (
        (token->type == T_WORD || token->type == T_ENVVAR) &&
        token->content &&
        token->content[0]
    );
} 

static int is_token_separator(t_token *token)
{
    return (
        token->type == T_PIPE ||
        token->type == T_REDIR_IN ||
        token->type == T_REDIR_OUT ||
        token->type == T_APPEND ||
        token->type == T_HEREDOC
    );
}

static int count_arg_tokens(t_token *tokens)
{
    int count = 0;

    while (tokens)
    {
        if (is_token_separator(tokens))
            break;

        if (is_token_argument(tokens))
            count++;

        tokens = tokens->next;
    }
    return count;
}

static char **tokens_to_argv(t_token *tokens)
{
    int arg_count = count_arg_tokens(tokens);
    if (arg_count == 0)
        return NULL;
    char **argv = malloc(sizeof(char *) * (arg_count + 1));
    if (!argv)
    {
        perror("malloc error in tokens_to_argv");
        return NULL;
    }
    int i = 0;
    t_token *head_tokens = tokens;
    while (head_tokens && i < arg_count)
    {
        if (is_token_separator(head_tokens))
            break;

        if (is_token_argument(head_tokens))
        {
            argv[i] = ft_strdup(head_tokens->content);
            if (!argv[i])
            {
                perror("ft_strdup error in tokens_to_argv");
                while (--i >= 0)
                    free(argv[i]);
                free(argv);
                return NULL;
            }
            i++;
        }
        head_tokens = head_tokens->next;
    }
    argv[i] = NULL;
    return argv;
} 

void    exec_cmd_tokens(t_token *tokens, char **env)
{
    char    *path;
    char    **cmdtab;

    cmdtab = tokens_to_argv(tokens);
    if (cmdtab == NULL || cmdtab[0] == NULL)
    {
        if (cmdtab != NULL && cmdtab[0] == NULL)
            ft_putstr_fd("minishell: command not found\n", 2);
        else if (cmdtab == NULL)
            ft_putstr_fd("minishell: error preparing command for execution\n", 2);
        if (cmdtab)
            ft_free(cmdtab);
        return; 
    }
    path = path_of_cmd(cmdtab[0], ft_get_paths("PATH", env));
    if (path == NULL)
    {
        ft_putstr_fd("minishell: command not found: ", 2);
        ft_putstr_fd_nl(cmdtab[0], 2);
        ft_free(cmdtab); 
        return;
    }
    if (execve(path, cmdtab, env) == -1)
    {
        ft_putstr_fd("minishell: ", 2);
        ft_putstr_fd(cmdtab[0], 2);
        ft_putstr_fd(": ", 2);
        perror(""); 
        free(path);
        ft_free(cmdtab);
    }
}

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
			ft_putstr_fd_nl(cmdtab[0], 2);
		}
		free(path);
		ft_free(cmdtab);
		return ;
	}
	if (execve(path, cmdtab, env) == -1)
	{
		ft_putstr_fd("command not found : ", 2);
		ft_putstr_fd_nl(cmdtab[0], 2);
		free(path);
		ft_free(cmdtab);
		return ;
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

	(void)argc;
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
