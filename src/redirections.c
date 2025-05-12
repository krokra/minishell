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