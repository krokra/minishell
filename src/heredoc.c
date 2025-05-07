#include "../includes/minishell.h"

// Implémenter la logique de lecture du heredoc et de stockage
// Gérer les erreurs (retourner -1)
// Décider comment stocker/passer le contenu lu (fichier temporaire ?)

int handle_heredocs(t_token *tokens)
{
    t_token     *current_token;
    char        *delimiter_str;
    char        *input_line;
    int         pipe_fds[2];

    current_token = tokens;
    while (current_token)
    {
        if (current_token->type == T_HEREDOC)
        {
            if (current_token->next && current_token->next->type == T_WORD)
            {
                delimiter_str = current_token->next->content;
                
                if (pipe(pipe_fds) == -1) {
                    perror("minishell: pipe for heredoc");
                    return (-1); 
                }

                while (1)
                {
                    input_line = readline("> ");
                    if (input_line == NULL) { 
                        break; 
                    }
                    if (strcmp(input_line, delimiter_str) == 0) { 
                        free(input_line);
                        break; 
                    }

                    int write_op_failed = 0;
                    if (write(pipe_fds[1], input_line, strlen(input_line)) == -1) { 
                        perror("minishell: write to heredoc pipe content");
                        write_op_failed = 1;
                    }
                    if (!write_op_failed && write(pipe_fds[1], "\n", 1) == -1) {
                        perror("minishell: write to heredoc pipe newline");
                        write_op_failed = 1;
                    }
                    
                    free(input_line);

                    if (write_op_failed) { 
                        close(pipe_fds[0]); 
                        close(pipe_fds[1]);
                        return(-1);
                    }
                }
				if (close(pipe_fds[1]) == -1) {
                    perror("minishell: close heredoc pipe write-end");
                    close(pipe_fds[0]); 
                    return (-1);
                }
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
