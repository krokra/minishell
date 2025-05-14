/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: psirault <psirault@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/27 08:16:26 by psirault          #+#    #+#             */
/*   Updated: 2025/05/14 11:37:46 by psirault         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/pipex.h"

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

static void cleanup(char **cmdtab, char **env, t_token *tokens)
{
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
    if (arg_count == 0) return NULL;
    argv = malloc(sizeof(char *) * (arg_count + 1));
    if (!argv) return NULL;
    i = 0;
    while (tokens && i < arg_count)
    {
        if (is_token_separator(tokens)) break;
        if (is_token_argument(tokens))
        {
            argv[i] = ft_strdup(tokens->content);
            if (!argv[i]) return NULL;
            i++;
        }
        tokens = tokens->next;
    }
    argv[i] = NULL;
    return argv;
}

static void exec_cmd_common(char **cmdtab, char **env, t_token *tokens)
{
    char *path = path_of_cmd(cmdtab[0], ft_get_paths("PATH", env));
    if (!path)
    {
        ft_putstr_fd("minishell: command not found: ", 2);
        ft_putstr_fd_nl(cmdtab[0], 2);
        free(path);
        cleanup(cmdtab, env, tokens);
        exit(127);
    }
    if (execve(path, cmdtab, env) == -1)
    {
        ft_putstr_fd("minishell: error executing command: ", 2);
        ft_putstr_fd_nl(cmdtab[0], 2);
        free(path);
        cleanup(cmdtab, env, tokens);
        exit(126);
    }
    free(path);
    ft_free(cmdtab);
    exit(0);
}

void exec_cmd_tokens(t_token *tokens, char **env)
{
    int pipefd[2];
    pid_t pid;
    int last_heredoc_fd;
    t_token *current;
    int prev_pipe_read;
    last_heredoc_fd = -1;
    current = tokens;
    prev_pipe_read = -1;

    // 1. Gestion des heredocs
    while (current)
    {
        if (current->type == T_HEREDOC) 
            last_heredoc_fd = current->heredoc_pipe_read_fd;
        if (current->type == T_PIPE)
            break;
        current = current->next;
    }

    // 2. Si on a un heredoc, le gérer
    if (last_heredoc_fd != -1)
    {
        pid = fork();
        if (pid == -1)
        {
            perror("minishell: fork");
            close(last_heredoc_fd);
            return;
        }
        if (pid == 0)
        {
            if (dup2(last_heredoc_fd, STDIN_FILENO) == -1)
            {
                perror("minishell: dup2 heredoc");
                close(last_heredoc_fd);
                cleanup(NULL, env, tokens);
                exit(1);
            }
            close(last_heredoc_fd);
            char **cmdtab = tokens_to_argv(tokens);
            if (cmdtab && cmdtab[0])
                exec_cmd_common(cmdtab, env, tokens);
            cleanup(cmdtab, env, tokens);
            exit(0);
        }
        close(last_heredoc_fd);
        waitpid(pid, NULL, 0); 
        return;
    }

    // 3. Gestion des pipes multiples
    current = tokens;
    while (current)
    {
        if (current->type == T_PIPE || current->next == NULL)
        {
            if (pipe(pipefd) == -1)
            {
                perror("minishell: pipe");
                return;
            }

            pid = fork();
            if (pid == -1)
            {
                perror("minishell: fork");
                close(pipefd[0]);
                close(pipefd[1]);
                return;
            }

            if (pid == 0)
            {
                // Configuration des entrées/sorties
                if (prev_pipe_read != -1)
                {
                    if (dup2(prev_pipe_read, STDIN_FILENO) == -1)
                    {
                        perror("minishell: dup2 stdin");
                        exit(1);
                    }
                    close(prev_pipe_read);
                }

                if (current->next != NULL)
                {
                    if (dup2(pipefd[1], STDOUT_FILENO) == -1)
                    {
                        perror("minishell: dup2 stdout");
                        exit(1);
                    }
                }

                close(pipefd[0]);
                close(pipefd[1]);

                // Exécution de la commande (tokens pointe vers le début du segment actuel)
                if (!handle_builtins(env, tokens)) 
                { 
                    char **cmdtab = tokens_to_argv(tokens);
                    if (cmdtab && cmdtab[0])
                        exec_cmd_common(cmdtab, env, tokens); // exec_cmd_common gère l'exit
                }
                cleanup(NULL, env, tokens);
                exit(0); 
            }

            // Dans le parent
            if (prev_pipe_read != -1)
                close(prev_pipe_read);
            close(pipefd[1]);
            close(pipefd[0]);
            prev_pipe_read = pipefd[0];

            // Mise à jour des tokens pour la prochaine commande
            if (current->next)
            {
                tokens = current->next;
                current = tokens;
                continue;
            }
            break;
        }
        current = current->next;
    }

    // Attente de la fin de tous les processus
    while (waitpid(-1, NULL, 0) > 0);
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
