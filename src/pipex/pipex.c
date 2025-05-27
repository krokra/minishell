/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: psirault <psirault@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/27 08:16:26 by psirault          #+#    #+#             */
/*   Updated: 2025/05/27 09:52:33 by psirault         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/pipex.h"
#include "../../includes/lexer.h"
#include "../../includes/minishell.h"

static void handle_pipe_redirections(t_token *tokens, int *prev_pipe_read)
{
    t_token *current = tokens;
    while (current && current->type != T_PIPE)
    {
        if (current->type == T_REDIR_IN)
        {
            if (handle_input_redirection(current) < 0)
            {
                exit(1);
            }
            if (prev_pipe_read && *prev_pipe_read != -1)
            {
                close(*prev_pipe_read);
                *prev_pipe_read = -1;
            }
        }
        else if (current->type == T_REDIR_OUT)
        {
            if (current->next && current->next->type == T_WORD)
            {
                int fd = open(current->next->content, O_WRONLY | O_CREAT | O_TRUNC, 0644);
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
        }
        current = current->next;
    }
}

static t_token *find_command_start_from_segment(t_token *current_segment_token)
{
    t_token *current;
    current = current_segment_token;
    while (current != NULL && current->type != T_PIPE) {
        if (current->type == T_REDIR_IN || current->type == T_REDIR_OUT ||
            current->type == T_APPEND || current->type == T_HEREDOC) {
            if (current->next != NULL && current->next->type == T_WORD) {
                current = current->next->next;
                continue;
            } else {
                return NULL;
            }
        } else if (current->type == T_WORD || current->type == T_ENVVAR) {
            return current;
        } else {
            return NULL;
        }
    }
    return NULL;
}

static void free_tokens_tab(t_token **tokens)
{
    if (!tokens) return;
    for (int i = 0; tokens[i]; i++)
    {
        free(tokens[i]);
    }
    free(tokens);
}

static void cleanup(char **cmdtab, char **env, t_token *tokens, t_data *data)
{
    // Debug prints
    fprintf(stderr, "Starting cleanup with: data=%p cmdtab=%p env=%p tokens=%p\n", 
            (void*)data, (void*)cmdtab, (void*)env, (void*)tokens);
    
    // Free data first
    if (data)
    {
        free(data);
        data = NULL;
    }

    // Free command table
    if (cmdtab)
    {
        ft_free(cmdtab);
        cmdtab = NULL;
    }

    // Free environment variables
    if (env)
    {
        ft_free(env);
        env = NULL;
    }

    // Only free tokens if they haven't been freed already
    if (tokens)
    {
        // Add this check to ensure tokens haven't been freed
        // This assumes tokens->content is always valid when tokens is valid
        if (tokens->content)  
        {
            free_tokens(tokens);
        }
        tokens = NULL;
    }
}

static void exec_cmd_common(char **cmdtab, char **env, t_data *data)
{
    char *path = path_of_cmd(cmdtab[0], ft_get_paths("PATH", env));
    if (!path)
    {
        ft_putstr_fd("minishell: command not found: ", 2);
        ft_putstr_fd_nl(cmdtab[0], 2);
        // Don't pass tokens to cleanup here, as they might be freed elsewhere
        cleanup(cmdtab, env, NULL, data);
        exit(127);
    }
    if (execve(path, cmdtab, env) == -1)
    {
        ft_putstr_fd("minishell: error executing command: ", 2);
        ft_putstr_fd_nl(cmdtab[0], 2);
        free(path);
        // Don't pass tokens to cleanup here, as they might be freed elsewhere
        cleanup(cmdtab, env, NULL, data);
        exit(126);
    }
}

// helper pour dup2 + fermer l'ancienne fd
static void dup2_and_close(int oldfd, int newfd)
{
    if (dup2(oldfd, newfd) < 0)
    {
        perror("minishell: dup2");
        exit(1);
    }
    close(oldfd);
}

// helper pour perror + exit
static void perror_exit(const char *msg)
{
    perror(msg);
    exit(1);
}

void exec_cmd_tokens(t_data *data, char **envp)
{
    int n_cmds = 0;
    t_token **cmds = split_tokens_by_pipe(data->tokens, &n_cmds);
    int prev_pipe_read = -1;
    pid_t pids[256];
    int pipefd[2];
    for (int i = 0; i < n_cmds; i++)
    {
        int has_next = (i < n_cmds - 1);
        if (has_next && pipe(pipefd) < 0)
            perror_exit("minishell: pipe");
        pid_t pid = fork();
        if (pid < 0)
            perror_exit("minishell: fork");
        if (pid == 0)
        {
            int heredoc_fd = get_heredoc_fd_from_segment(cmds[i]);
            if (heredoc_fd != -1)
                dup2_and_close(heredoc_fd, STDIN_FILENO);
            else if (prev_pipe_read != -1)
                dup2_and_close(prev_pipe_read, STDIN_FILENO);
            handle_pipe_redirections(cmds[i], &prev_pipe_read);
            if (!has_next && has_output_redirection(cmds[i])) {
                char *filename = get_output_filename(cmds[i]);
                int flags = is_append(cmds[i]) ? O_CREAT | O_WRONLY | O_APPEND
                                               : O_CREAT | O_WRONLY | O_TRUNC;
                int fd = open(filename, flags, 0644);
                if (fd < 0) { perror("open"); exit(1); }
                dup2(fd, STDOUT_FILENO);
                close(fd);
            }
            if (has_next)
                dup2_and_close(pipefd[1], STDOUT_FILENO);
            if (has_next)
                close(pipefd[0]);
            if (prev_pipe_read != -1)
                close(prev_pipe_read);
            t_token *cmd = find_command_start_from_segment(cmds[i]);
            if (cmd && handle_builtins(envp, cmd, data))
                exit(data->exit_status);
            t_token *cmd_start = find_command_start_from_segment(cmds[i]);
            char **argv = build_argv_from_tokens(cmd_start);
            free_tokens_tab(cmds);
            exec_cmd_common(argv, envp, data);
            exit(1);
        }
        pids[i] = pid;
        if (has_next)
            close(pipefd[1]);
        if (prev_pipe_read != -1)
            close(prev_pipe_read);
        prev_pipe_read = has_next ? pipefd[0] : -1;
    }
    for (int i = 0; i < n_cmds; i++)
    {
        waitpid(pids[i], &data->status_getter, 0);
        if (WIFEXITED(data->status_getter))
            data->exit_status = WEXITSTATUS(data->status_getter);
    }
    if (prev_pipe_read != -1)
        close(prev_pipe_read);
    free(cmds);
}

// void exec_cmd(char *cmd, char **env, t_token *tokens)
// {
//     char **cmdtab;
//     cmdtab = ft_split(cmd, ' ');
//     if (!cmdtab || !cmdtab[0]) return;
//     exec_cmd_common(cmdtab, env, tokens);
// }

// void exec_child(int *pipefd, char **argv, char **env)
// {
//     int fd;
//     fd = open(argv[1], O_RDONLY, 0777);
//     if (fd == -1 || argv[2][0] == '\0') return;
//     dup2(fd, 0);
//     dup2(pipefd[1], 1);
//     close(pipefd[0]);
//     close(pipefd[1]);
//     close(fd);
//     exec_cmd(argv[2], env);
// }

// void exec_parent(int *pipefd, char **argv, char **env)
// {
//     int fd;
//     fd = open(argv[4], O_WRONLY | O_CREAT | O_TRUNC, 0777);
//     if (fd == -1 || argv[3][0] == '\0') return;
//     dup2(fd, 1);
//     dup2(pipefd[0], 0);
//     close(pipefd[1]);
//     close(pipefd[0]);
//     close(fd);
//     exec_cmd(argv[3], env);
// }

// void fork_error(int *pipefd)
// {
//     close(pipefd[0]);
//     close(pipefd[1]);
//     exit(-1);
// }

// void pipex(int argc, char **argv, char **env)
// {
//     int pipefd[2];
//     pid_t pid1;
//     pid_t pid2;
//     (void)argc;  // Pour éviter l'erreur de compilation
//     if (pipe(pipefd) == -1) exit(-1);
//     pid1 = fork();
//     if (pid1 == -1) fork_error(pipefd);
//     if (pid1 == 0) exec_child(pipefd, argv, env);
//     pid2 = fork();
//     if (pid2 == -1) fork_error(pipefd);
//     if (pid2 == 0) exec_parent(pipefd, argv, env);
//     close(pipefd[1]);
//     close(pipefd[0]);
//     waitpid(pid1, NULL, 0);
//     waitpid(pid2, NULL, 0);
// }
