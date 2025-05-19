/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: psirault <psirault@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/27 08:16:26 by psirault          #+#    #+#             */
/*   Updated: 2025/05/15 14:14:01 by psirault         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/pipex.h"

static t_token *find_command_start_from_segment(t_token *current_segment_token) {
    t_token *current = current_segment_token;

    while (current != NULL && current->type != T_PIPE) {
        // Check for redirection types that consume the next token if it's a T_WORD
        if (current->type == T_REDIR_IN || current->type == T_REDIR_OUT ||
            current->type == T_APPEND || current->type == T_HEREDOC) {
            
            // Check if there's a next token and if it's a T_WORD (filename/delimiter)
            if (current->next != NULL && current->next->type == T_WORD) {
                current = current->next->next; // Move past the operator AND its T_WORD argument
                continue; // Restart loop with the token after the redirection sequence
            } else {
                // Syntax error (e.g., ">" at end of command, or ">> |", or "<< EOF no_command")
                // Parser should ideally catch this. If execution reaches here with such a token,
                // it means there's no valid command following this broken redirection.
                return NULL; 
            }
        } else if (current->type == T_WORD) {
            // If it's not a redirection operator we're currently on, and it's a T_WORD,
            // this is the command.
            return current;
        } else {
            // If it's some other token type that's not a command or a known redirection operator
            // (e.g., an unexpected T_PIPE that segment_scanner should have stopped at, or another separator),
            // then it's not a command.
            return NULL; 
        }
    }
    return NULL; // Reached end of segment (or pipe) without finding a T_WORD command.
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
    t_token *current;
    int prev_pipe_read;
    current = tokens;
    prev_pipe_read = -1;

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
                int current_segment_heredoc_fd;
                t_token *segment_scanner;

                current_segment_heredoc_fd = -1;
                segment_scanner = tokens; // 'tokens' here is the start of the current command segment

                // Scan for heredoc for the current command segment
                while(segment_scanner != NULL && segment_scanner->type != T_PIPE) {
                    if (segment_scanner->type == T_HEREDOC && segment_scanner->heredoc_pipe_read_fd != -1) {
                        if (current_segment_heredoc_fd != -1) {
                            close(current_segment_heredoc_fd); // Close previous if multiple for this segment
                        }
                        current_segment_heredoc_fd = segment_scanner->heredoc_pipe_read_fd;
                    }
                    // Also need to check for T_REDIR_IN here for proper precedence if < file is implemented
                    // For now, only considering heredoc
                    segment_scanner = segment_scanner->next;
                }

                // Configuration des entrées/sorties
                if (current_segment_heredoc_fd != -1) {
                    if (dup2(current_segment_heredoc_fd, STDIN_FILENO) == -1) {
                        perror("minishell: dup2 heredoc stdin");
                        close(current_segment_heredoc_fd); // Ensure fd is closed on error
                        exit(1);
                    }
                    close(current_segment_heredoc_fd); // Close original fd after successful dup
                    if (prev_pipe_read != -1) {
                        close(prev_pipe_read); // Heredoc takes precedence over pipe input
                        prev_pipe_read = -1;
                    }
                }
                else if (prev_pipe_read != -1)
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

                t_token *actual_cmd_tokens;
                actual_cmd_tokens = find_command_start_from_segment(tokens); // 'tokens' is start of current segment

                if (actual_cmd_tokens == NULL) {
                    // No command found, segment might be only I/O redirections (e.g., "> out < in")
                    // or a syntax error like ">>" at the end.
                    // Exit gracefully. If redirections failed, perror would have been called.
                    cleanup(NULL, env, tokens); // Cleanup original segment tokens
                    exit(0); 
                }

                // Exécution de la commande (actual_cmd_tokens pointe vers le début de la commande réelle)
                if (!handle_builtins(env, actual_cmd_tokens)) 
                { 
                    char **cmdtab = tokens_to_argv(actual_cmd_tokens);
                    if (cmdtab && cmdtab[0])
                        exec_cmd_common(cmdtab, env, tokens); // exec_cmd_common uses original 'tokens' for its own full cleanup context
                    // else: If cmdtab is NULL but actual_cmd_tokens was not, tokens_to_argv failed for a supposedly valid command start.
                    // This would be an internal error or an unhandled edge case in tokens_to_argv.
                    // exec_cmd_common handles exit, so if it's not called, we need to exit.
                    // However, if cmdtab is NULL, perhaps we should exit with an error or 0 depending on policy.
                    // For now, if exec_cmd_common is not called because cmdtab is invalid, the process will fall through.
                }
                cleanup(NULL, env, tokens); // General cleanup for the child process, using original segment tokens
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
