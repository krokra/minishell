#include "../includes/minishell.h"

// Fonction pour traiter les variables dans une ligne de heredoc
static char *process_heredoc_line(char *line, char **env, t_data *data)
{
    char *processed_line;
    t_token *temp_token;
    
    temp_token = create_token(line, 0);
    if (!temp_token)
        return (NULL);
    if (!temp_token->content)
    {
        free_tokens(temp_token);
        return (NULL);
    }
    temp_token->type = T_WORD;
    temp_token->next = NULL;
    replace_env_vars(temp_token, env, data);
    processed_line = ft_strdup(temp_token->content);
    free_tokens(temp_token);
    return (processed_line);
}

int handle_heredocs(t_token *tokens, char **env, t_data *data)
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
                if (last_heredoc_fd != -1) {
                    close(last_heredoc_fd);
                    last_heredoc_fd = -1;
                }
                
                if (pipe(pipe_fds) == -1) {
                    perror("minishell: pipe for heredoc");
                    return (-1);
                }

                // Store the read end of pipe to be closed later
                current_token->heredoc_pipe_read_fd = pipe_fds[0];

                while (1)
                {
                    input_line = readline("> ");
                    if (input_line == NULL) { 
                        close(pipe_fds[0]);
                        close(pipe_fds[1]);
                        return (-1);
                    }
                    if (strcmp(input_line, delimiter_str) == 0) { 
                        free(input_line);
                        break; 
                    }
                    processed_line = process_heredoc_line(input_line, env, data);
                    if (!processed_line) {
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
                    free(processed_line);
                }
                // Always close write end after heredoc content is written
                close(pipe_fds[1]);
            }
        }
        current_token = current_token->next;
    }
    // Close any remaining heredoc fd before returning
    if (last_heredoc_fd != -1)
        close(last_heredoc_fd);
    return (0);
}
