#include "../includes/minishell.h"

// Fonction pour traiter les variables dans une ligne de heredoc sans tokenisation
static char *expand_line_heredoc(char *line, char **env, t_data *data)
{
    if (!line)
        return (NULL);
    if (ft_strchr(line, '$') == NULL)
        return (ft_strdup(line));
    return (replace_vars_in_str(line, env, data));
}

int handle_heredocs(t_token *tokens, char **env, t_data *data)
{
    t_token     *current_token;
    char        *delimiter_str;
    char        *input_line;
    char        *processed_line;
    int         pipe_fds[2];
    int         last_heredoc_fd;

    printf("[HEREDOC] Starting heredoc handling\n");
    last_heredoc_fd = -1;
    current_token = tokens;
    while (current_token)
    {
        if (current_token->type == T_HEREDOC)
        {
            printf("[HEREDOC] Found heredoc token\n");
            if (current_token->next && current_token->next->type == T_WORD)
            {
                delimiter_str = current_token->next->content;
                printf("[HEREDOC] Delimiter: %s\n", delimiter_str);
                if (last_heredoc_fd != -1) {
                    printf("[HEREDOC] Closing previous heredoc fd: %d\n", last_heredoc_fd);
                    close(last_heredoc_fd);
                    last_heredoc_fd = -1;
                }
                
                if (pipe(pipe_fds) == -1) {
                    perror("minishell: pipe for heredoc");
                    return (-1);
                }
                printf("[HEREDOC] Created pipe: read=%d, write=%d\n", pipe_fds[0], pipe_fds[1]);

                // Store the read end of pipe to be closed later
                current_token->heredoc_pipe_read_fd = pipe_fds[0];

                while (1)
                {
                    input_line = readline("> ");
                    if (input_line == NULL) { 
                        printf("[HEREDOC] EOF received, closing pipe\n");
                        close(pipe_fds[0]);
                        close(pipe_fds[1]);
                        return (-1);
                    }
                    if (strcmp(input_line, delimiter_str) == 0) { 
                        printf("[HEREDOC] Delimiter matched, ending heredoc\n");
                        free(input_line);
                        break; 
                    }
                    printf("[HEREDOC] Processing line: %s\n", input_line);
                    processed_line = expand_line_heredoc(input_line, env, data);
                    if (!processed_line) {
                        printf("[HEREDOC] Error processing line\n");
                        close(pipe_fds[0]);
                        close(pipe_fds[1]);
                        return (-1);
                    }
                    if (write(pipe_fds[1], processed_line, strlen(processed_line)) == -1) { 
                        perror("minishell: write to heredoc pipe");
                        free(processed_line);
                        close(pipe_fds[0]);
                        close(pipe_fds[1]);
                        return(-1);
                    }
                    if (write(pipe_fds[1], "\n", 1) == -1) {
                        perror("minishell: write newline to heredoc pipe");
                        free(processed_line);
                        close(pipe_fds[0]);
                        close(pipe_fds[1]);
                        return(-1);
                    }
                    printf("[HEREDOC] Wrote line to pipe\n");
                    free(processed_line);
                }
                // Always close write end after heredoc content is written
                printf("[HEREDOC] Closing write end of pipe\n");
                close(pipe_fds[1]);
            }
        }
        current_token = current_token->next;
    }
    // Close any remaining heredoc fd before returning
    if (last_heredoc_fd != -1) {
        printf("[HEREDOC] Closing last heredoc fd: %d\n", last_heredoc_fd);
        close(last_heredoc_fd);
    }
    printf("[HEREDOC] Finished handling heredocs\n");
    return (0);
}
