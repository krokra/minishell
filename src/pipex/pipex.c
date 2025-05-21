/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: psirault <psirault@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/27 08:16:26 by psirault          #+#    #+#             */
/*   Updated: 2025/05/21 11:32:12 by psirault         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/pipex.h"

static void handle_pipe_redirections(t_token *tokens, int *prev_pipe_read)
{
    t_token *current = tokens;
    int fd;
    while (current && current->type != T_PIPE)
    {
        if (current->type == T_REDIR_IN)
        {
            if (current->next && current->next->type == T_WORD)
            {
                fd = open(current->next->content, O_RDONLY);
                if (fd == -1)
                {
                    perror("minishell: open");
                    exit(1);
                }
                if (dup2(fd, STDIN_FILENO) == -1)
                {
                    perror("minishell: dup2");
                    close(fd);
                    exit(1);
                }
                close(fd);
                if (*prev_pipe_read != -1)
                {
                    close(*prev_pipe_read);
                    *prev_pipe_read = -1;
                }
            }
        }
        else if (current->type == T_REDIR_OUT)
        {
            if (current->next && current->next->type == T_WORD)
            {
                fd = open(current->next->content, O_WRONLY | O_CREAT | O_TRUNC, 0644);
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

static int is_token_argument(t_token *token)
{
    return (token->type == T_WORD || token->type == T_ENVVAR) && token->content && token->content[0];
}

static int is_token_separator(t_token *token)
{
    return token->type == T_PIPE || token->type == T_REDIR_IN ||
           token->type == T_REDIR_OUT || token->type == T_APPEND || token->type == T_HEREDOC;
}

static int count_arg_tokens(t_token *tokens)
{
    int count;
    count = 0;
    while (tokens && !is_token_separator(tokens))
    {
        if (is_token_argument(tokens)) count++;
        tokens = tokens->next;
    }
    return count;
}

static void cleanup(char **cmdtab, char **env, t_token *tokens, t_data *data)
{
    if (data)
        free(data);
    if (cmdtab)
        ft_free(cmdtab);
    if (env)
        ft_free(env);
    if (tokens)
        free_tokens(tokens);
}

static char **tokens_to_argv(t_token *tokens)
{
    int arg_count;
    char **argv;
    int i;
    arg_count = count_arg_tokens(tokens);
    if (arg_count == 0)
        return NULL;
    argv = malloc(sizeof(char *) * (arg_count + 1));
    if (!argv)
        return NULL;
    i = 0;
    while (tokens && i < arg_count)
    {
        if (is_token_separator(tokens))
            break;
        if (is_token_argument(tokens))
        {
            argv[i] = ft_strdup(tokens->content);
            if (!argv[i])
                return NULL;
            i++;
        }
        tokens = tokens->next;
    }
    argv[i] = NULL;
    return (argv);
}

static void exec_cmd_common(char **cmdtab, char **env, t_token *tokens, t_data *data)
{
    char *path = path_of_cmd(cmdtab[0], ft_get_paths("PATH", env));
    if (!path)
    {
        ft_putstr_fd("minishell: command not found: ", 2);
        ft_putstr_fd_nl(cmdtab[0], 2);
        cleanup(cmdtab, env, tokens->first, data);
        exit(127);
    }
    if (execve(path, cmdtab, env) == -1)
    {
        ft_putstr_fd("minishell: error executing command: ", 2);
        ft_putstr_fd_nl(cmdtab[0], 2);
        free(path);
        cleanup(cmdtab, env, tokens->first, data);
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
    t_token *seg = data->tokens;
    int      prev_pipe_read = -1;
    pid_t    pids[256];
    int      n = 0;

    while (seg)
    {
        // 1) trouver le prochain '|' dans toute la liste
        t_token *next_pipe = seg;
        while (next_pipe && next_pipe->type != T_PIPE)
            next_pipe = next_pipe->next;
        bool has_next = (next_pipe != NULL);

        // 2) créer le pipe si besoin
        int pipefd[2];
        if (has_next && pipe(pipefd) < 0)
            perror_exit("minishell: pipe");

        // 3) fork
        pid_t pid = fork();
        if (pid < 0)
            perror_exit("minishell: fork");

        if (pid == 0)
        {
            // enfant : STDIN ← heredoc ou prev_pipe_read
            if (seg->heredoc_pipe_read_fd != -1)
                dup2_and_close(seg->heredoc_pipe_read_fd, STDIN_FILENO);
            else if (prev_pipe_read != -1)
                dup2_and_close(prev_pipe_read, STDIN_FILENO);

            // redirections fichiers (<, >, >>)
            handle_pipe_redirections(seg, &prev_pipe_read);

            // STDOUT → pipefd[1] si on a un segment suivant
            if (has_next)
                dup2_and_close(pipefd[1], STDOUT_FILENO);

            // fermer les descripteurs non utilisés
            if (has_next)
                close(pipefd[0]);
            if (prev_pipe_read != -1)
                close(prev_pipe_read);

            // 4) exécuter la commande
            t_token *cmd = find_command_start_from_segment(seg);
            if (cmd && handle_builtins(envp, cmd, data))
                exit(data->exit_status);
            char **argv = tokens_to_argv(cmd);
            exec_cmd_common(argv, envp, seg, data);

            // si exec échoue
            exit(1);
        }

        // parent : on garde le pid, on ferme la sortie du pipe et l'ancien prev_pipe_read
        pids[n++] = pid;
        if (has_next)
            close(pipefd[1]);
        if (prev_pipe_read != -1)
            close(prev_pipe_read);
        prev_pipe_read = has_next ? pipefd[0] : -1;

        // 5) avancer au segment suivant (on saute le token '|')
        seg = has_next ? next_pipe->next : NULL;
    }

    // 6) attendre tous les enfants
    for (int i = 0; i < n; i++)
    {
        waitpid(pids[i], &data->status_getter, 0);
        if (WIFEXITED(data->status_getter))
            data->exit_status = WEXITSTATUS(data->status_getter);
    }

    if (prev_pipe_read != -1)
        close(prev_pipe_read);
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
