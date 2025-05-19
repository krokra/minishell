#include "../includes/minishell.h"

// Fonction pour traiter les variables dans une ligne de heredoc
static char *process_heredoc_line(char *line, char **env)
{
    char *processed_line;
    t_token *temp_token;
    
    temp_token = malloc(sizeof(t_token));
    if (!temp_token)
        return (NULL);
    
    temp_token->content = ft_strdup(line);
    if (!temp_token->content)
    {
        free(temp_token);
        return (NULL);
    }
    temp_token->type = T_WORD;
    temp_token->next = NULL;
    
    replace_env_vars(temp_token, env);
    
    processed_line = ft_strdup(temp_token->content);
    
    free(temp_token->content);
    free(temp_token);
    
    return (processed_line);
}

int handle_heredocs(t_token *tokens, char **env)
{
    t_token     *current_token;
    char        *delimiter_str;
    char        *input_line;
    char        *processed_line;
    int         pipe_fds[2];
    int         last_heredoc_fd;

    last_heredoc_fd = -1;
    current_token = tokens;
    while (current_token)
    {
        if (current_token->type == T_HEREDOC)
        {
            if (current_token->next && current_token->next->type == T_WORD)
            {
                delimiter_str = current_token->next->content;
                printf("[DEBUG HEREDOC] Delimiter: [%s]\n", delimiter_str);

                if (last_heredoc_fd != -1) {
                    close(last_heredoc_fd);
                    last_heredoc_fd = -1;
                }

                if (pipe(pipe_fds) == -1) {
                    perror("minishell: pipe for heredoc");
                    return (-1);
                }
                printf("[DEBUG HEREDOC] Pipe created: read_fd=%d, write_fd=%d\n", pipe_fds[0], pipe_fds[1]);

                while (1)
                {
                    input_line = readline("> ");
                    if (input_line == NULL) {
                        fprintf(stderr, "[DEBUG HEREDOC] readline returned NULL (EOF or error)\n");
                        close(pipe_fds[0]);
                        close(pipe_fds[1]);
                        return (-1);
                    }
                    printf("[DEBUG HEREDOC] Read line: [%s]\n", input_line);

                    if (strcmp(input_line, delimiter_str) == 0) {
                        printf("[DEBUG HEREDOC] Delimiter found. Exiting read loop.\n");
                        free(input_line);
                        break;
                    }

                    processed_line = process_heredoc_line(input_line, env);
                    free(input_line);
                    
                    if (!processed_line) {
                        fprintf(stderr, "[DEBUG HEREDOC] processed_line is NULL\n");
                        close(pipe_fds[0]);
                        close(pipe_fds[1]);
                        return (-1);
                    }
                    printf("[DEBUG HEREDOC] Processed line to write: [%s]\n", processed_line);

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
                    
                    free(processed_line);
                }

                printf("[DEBUG HEREDOC] Closing pipe_fds[1] = %d\n", pipe_fds[1]);
                if (close(pipe_fds[1]) == -1) {
                    perror("minishell: close heredoc pipe write-end");
                    close(pipe_fds[0]);
                    return (-1);
                }

                last_heredoc_fd = pipe_fds[0];
                current_token->heredoc_pipe_read_fd = pipe_fds[0];
                printf("[DEBUG HEREDOC] Stored read_fd = %d in token.\n", pipe_fds[0]);
            }
            else
            {
                ft_putstr_fd("minishell: syntax error: heredoc operator needs a delimiter\n", 2);
                return (-1);
            }
        }
        current_token = current_token->next;
    }
    return (0);
}
