#include "../includes/minishell.h"

// Fonction pour traiter les variables dans une ligne de heredoc
static char *process_heredoc_line(char *line, char **env, t_data *data)
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
    
    replace_env_vars(temp_token, env, data);
    
    processed_line = ft_strdup(temp_token->content);
    
    free(temp_token->content);
    free(temp_token);
    
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
                    free(input_line);
                    
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

                if (close(pipe_fds[1]) == -1) {
                    perror("minishell: close heredoc pipe write-end");
                    close(pipe_fds[0]);
                    return (-1);
                }

                last_heredoc_fd = pipe_fds[0];
                current_token->heredoc_pipe_read_fd = pipe_fds[0];
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
