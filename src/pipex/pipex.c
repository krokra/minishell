/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbariol- <nassimbariol@student.42.fr>>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/27 08:16:26 by psirault          #+#    #+#             */
/*   Updated: 2025/05/29 15:55:43 by nbariol-         ###   ########.fr       */
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
        if (current->type == T_REDIR_IN && current->next)
        {
            if (handle_input_redirection(current) < 0)
                exit(1);
            if (prev_pipe_read && *prev_pipe_read != -1)
            {
                close(*prev_pipe_read);
                *prev_pipe_read = -1;
            }
        }
        else if ((current->type == T_REDIR_OUT || current->type == T_APPEND)
            && current->next && current->next->type == T_WORD)
        {
            char *filename = current->next->content;
            int flags = (current->type == T_APPEND)
                ? O_WRONLY | O_CREAT | O_APPEND
                : O_WRONLY | O_CREAT | O_TRUNC;
            int fd = open(filename, flags, 0644);
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
    fprintf(stderr, "Starting cleanup with: data=%p cmdtab=%p env=%p tokens=%p\n", 
            (void*)data, (void*)cmdtab, (void*)env, (void*)tokens);
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
}

static void exec_cmd_common(char **cmdtab, char **env, t_data *data)
{
    char *path = path_of_cmd(cmdtab[0], ft_get_paths("PATH", env));

    if (!path || path == NULL)
    {
        ft_putstr_fd("minishell: command not found: ", 2);
        ft_putstr_fd_nl(cmdtab[0], 2);
        cleanup(cmdtab, env, NULL, data);
        exit(127);
    }
    if (execve(path, cmdtab, env) == -1)
    {
        ft_putstr_fd("minishell: error executing command: ", 2);
        ft_putstr_fd_nl(cmdtab[0], 2);
        free(path);
        cleanup(cmdtab, env, NULL, data);
        exit(126);
    }
	cleanup(cmdtab, env, NULL, data);
}

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

void    exec_cmd_tokens(t_data *data, char **envp)
{
    int n_cmds = 0;
    t_token **cmds = split_tokens_by_pipe(data->tokens, &n_cmds);
    int prev_pipe_read = -1;
    pid_t pids[256];
    int pipefd[2];
    int sig;
    
    printf("[PIPEX] Starting execution of %d commands\n", n_cmds);
    
    for (int i = 0; i < n_cmds; i++)
    {
        int has_next = (i < n_cmds - 1);
        printf("[PIPEX] Processing command %d/%d\n", i + 1, n_cmds);
        
        if (has_next && pipe(pipefd) < 0)
            perror_exit("minishell: pipe");
        if (has_next)
            printf("[PIPEX] Created pipe: read=%d, write=%d\n", pipefd[0], pipefd[1]);
            
        signal(SIGINT, SIG_IGN);
        pid_t pid = fork();
        if (pid < 0)
            perror_exit("minishell: fork");
        if (pid == 0)
        {
            printf("[PIPEX] Child process %d started\n", getpid());
            int heredoc_fd = get_heredoc_fd_from_segment(cmds[i]);
            printf("[PIPEX] Child %d: heredoc fd = %d\n", getpid(), heredoc_fd);
            
            if (heredoc_fd != -1) {
                printf("[PIPEX] Child %d: Redirecting heredoc to stdin\n", getpid());
                dup2_and_close(heredoc_fd, STDIN_FILENO);
            }
            else if (prev_pipe_read != -1) {
                printf("[PIPEX] Child %d: Redirecting previous pipe to stdin\n", getpid());
                dup2_and_close(prev_pipe_read, STDIN_FILENO);
            }
            
            handle_pipe_redirections(cmds[i], &prev_pipe_read);
            
            if (has_next) {
                printf("[PIPEX] Child %d: Redirecting stdout to pipe\n", getpid());
                dup2_and_close(pipefd[1], STDOUT_FILENO);
            }
            if (has_next)
                close(pipefd[0]);
            if (prev_pipe_read != -1)
                close(prev_pipe_read);
            
            t_token *cmd = find_command_start_from_segment(cmds[i]);
            if (cmd && handle_builtins(envp, cmd, data)) {
                printf("[PIPEX] Child %d: Executing builtin command\n", getpid());
                exit(data->exit_status);
            }
            
            t_token *cmd_start = find_command_start_from_segment(cmds[i]);
            char **argv = build_argv_from_tokens(cmd_start);
            printf("[PIPEX] Child %d: Executing command: %s\n", getpid(), argv[0]);
            free_tokens_tab(cmds);
            signal(SIGINT, SIG_DFL);
            signal(SIGQUIT, SIG_DFL);
            if (argv[0] != NULL)
                exec_cmd_common(argv, envp, data);
            cleanup(argv, envp, NULL, data);
            exit(1);
        }
        printf("[PIPEX] Parent: Created child process %d\n", pid);
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
        else if (WIFSIGNALED(data->status_getter))
        {
            sig = WTERMSIG(data->status_getter);
            data->exit_status = 128 + sig;
            if (sig == SIGINT)
                write(1, "\n", 1);
        }
    }
    signal(SIGINT, sigint_prompt);
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
