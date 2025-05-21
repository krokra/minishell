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
                fprintf(stderr, "[PID %d] open('< %s') = %d\n", getpid(), current->next->content, fd);
                if (fd == -1)
                {
                    perror("minishell: open");
                    exit(1);
                }
                fprintf(stderr, "[PID %d] dup2(%d -> %d)\n", getpid(), fd, STDIN_FILENO);
                if (dup2(fd, STDIN_FILENO) == -1)
                {
                    perror("minishell: dup2");
                    close(fd);
                    exit(1);
                }
                fprintf(stderr, "[PID %d] close(%d)\n", getpid(), fd);
                close(fd);
                if (*prev_pipe_read != -1)
                {
                    fprintf(stderr, "[PID %d] close(prev_pipe_read %d)\n", getpid(), *prev_pipe_read);
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
                fprintf(stderr, "[PID %d] open('> %s') = %d\n", getpid(), current->next->content, fd);
                if (fd == -1)
                {
                    perror("minishell: open");
                    exit(1);
                }
                fprintf(stderr, "[PID %d] dup2(%d -> %d)\n", getpid(), fd, STDOUT_FILENO);
                if (dup2(fd, STDOUT_FILENO) == -1)
                {
                    perror("minishell: dup2");
                    close(fd);
                    exit(1);
                }
                fprintf(stderr, "[PID %d] close(%d)\n", getpid(), fd);
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
            fprintf(stderr, "[PID %d] find_command_start: %s\n", getpid(), current->content);
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
    fprintf(stderr, "[PID %d] exec_cmd_common: execve(%s)\n", getpid(), path ? path : cmdtab[0]);
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

void exec_cmd_tokens(t_data *data, char **envp)
{
    printf("\n[EXEC_CMD] Début de l'exécution des commandes\n");
    t_token *seg = data->tokens;
    int prev_pipe_read = -1;
    pid_t pids[256];
    int n = 0;

    while (seg)
    {
        printf("\n[EXEC_CMD] Nouveau segment de commande\n");
        bool has_next = (seg->next && seg->next->type == T_PIPE);
        int pipefd[2];

        if (has_next && pipe(pipefd) < 0)
        {
            printf("[EXEC_CMD] ERREUR: Échec de la création du pipe\n");
            perror("minishell: pipe");
            return;
        }

        printf("[EXEC_CMD] Fork du processus\n");
        pid_t pid = fork();
        if (pid < 0)
        {
            printf("[EXEC_CMD] ERREUR: Échec du fork\n");
            perror("minishell: fork");
            if (has_next)
            {
                close(pipefd[0]);
                close(pipefd[1]);
            }
            return;
        }

        if (pid == 0)
        {
            printf("[EXEC_CMD] Processus enfant %d démarré\n", getpid());
            
            // entrée : heredoc ou pipe précédent
            if (seg->heredoc_pipe_read_fd != -1)
            {
                printf("[EXEC_CMD] Redirection depuis heredoc: fd=%d\n", seg->heredoc_pipe_read_fd);
                dup2(seg->heredoc_pipe_read_fd, STDIN_FILENO);
                close(seg->heredoc_pipe_read_fd);
            }
            else if (prev_pipe_read != -1)
            {
                printf("[EXEC_CMD] Redirection depuis pipe précédent: fd=%d\n", prev_pipe_read);
                dup2(prev_pipe_read, STDIN_FILENO);
                close(prev_pipe_read);
            }

            // redirections fichiers
            handle_pipe_redirections(seg, &prev_pipe_read);

            // sortie vers le pipe suivant si nécessaire
            if (has_next)
            {
                printf("[EXEC_CMD] Redirection vers nouveau pipe: fd=%d\n", pipefd[1]);
                dup2(pipefd[1], STDOUT_FILENO);
            }

            // on ferme tous les fds qu'il ne faut plus garder
            if (has_next)
            {
                close(pipefd[0]);
                close(pipefd[1]);
            }
            if (prev_pipe_read != -1)
                close(prev_pipe_read);

            // exécution
            t_token *cmd = find_command_start_from_segment(seg);
            printf("[EXEC_CMD] Commande trouvée: %s\n", cmd ? cmd->content : "NULL");
            
            if (cmd && handle_builtins(envp, cmd, data))
            {
                printf("[EXEC_CMD] Commande builtin exécutée\n");
                exit(data->exit_status);
            }

            char **argv = tokens_to_argv(cmd);
            if (argv && argv[0])
            {
                printf("[EXEC_CMD] Exécution de la commande: %s\n", argv[0]);
                exec_cmd_common(argv, envp, seg, data);
            }

            printf("[EXEC_CMD] Erreur d'exécution, exit avec status 1\n");
            exit(1);
        }

        printf("[EXEC_CMD] Processus parent: enfant %d créé\n", pid);
        pids[n++] = pid;
        if (has_next)
            close(pipefd[1]);
        if (prev_pipe_read != -1)
            close(prev_pipe_read);
        prev_pipe_read = has_next ? pipefd[0] : -1;

        // AVANCE : si on a une pipe, on saute le token `|`
        seg = has_next ? seg->next->next : NULL;
    }

    printf("\n[EXEC_CMD] Attente de la fin des processus enfants\n");
    for (int i = 0; i < n; i++)
    {
        printf("[EXEC_CMD] Attente du processus %d\n", pids[i]);
        waitpid(pids[i], &data->status_getter, 0);
        if (WIFEXITED(data->status_getter))
        {
            data->exit_status = WEXITSTATUS(data->status_getter);
            printf("[EXEC_CMD] Processus %d terminé avec status %d\n", pids[i], data->exit_status);
        }
    }

    if (prev_pipe_read != -1)
    {
        printf("[EXEC_CMD] Fermeture du dernier pipe de lecture: fd=%d\n", prev_pipe_read);
        close(prev_pipe_read);
    }
    
    printf("[EXEC_CMD] Fin de l'exécution des commandes\n\n");
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
