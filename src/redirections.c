#include "../includes/minishell.h"

int handle_redirections(t_token *tokens)
{
    int fd;
    
    fd = 0;
    if (tokens->next == NULL || tokens->next->type != T_WORD)
    {
        perror("syntax error: redirection needs a file name");
        return (-1);
    }
    fd = open(tokens->next->content, O_WRONLY | O_CREAT | O_TRUNC, 0644); //0_TRUNC si el fichier existe on le vide
    if (fd < 0)
    {
        perror("open");
        return (-1);
    }
    dup2(fd, 1);
    return (fd);
}

int handle_append_redirection(t_token *tokens)
{
    int fd;

    if (tokens->next == NULL || tokens->next->type != T_WORD)
    {
        perror("syntax error: append redirection needs a file name");
        return -1;
    }
    fd = open(tokens->next->content, O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (fd < 0)
    {
        perror("minishell: open (append)");
        return -1;
    }
    if (dup2(fd, STDOUT_FILENO) < 0)
    {
        perror("minishell: dup2 (append)");
        close(fd);
        return -1;
    }
    close(fd);
    return fd;
}

int handle_input_redirection(t_token *tokens)
{
    int fd;
    if (tokens->next == NULL || tokens->next->type != T_WORD)
    {
        perror("syntax error: input redirection needs a file name");
        return (-1);
    }
    fd = open(tokens->next->content, O_RDONLY);
    if (fd < 0)
    {
        perror("minishell: open (input)");
        return (-1);
    }
    if (dup2(fd, STDIN_FILENO) < 0)
    {
        perror("minishell: dup2 (input)");
        close(fd);
        return (-1);
    }
    close(fd);
    return 0;
}