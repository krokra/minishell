/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: psirault <psirault@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/30 07:59:09 by psirault          #+#    #+#             */
/*   Updated: 2025/05/01 15:21:28 by psirault         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/lexer.h"

void    set_token_type(t_token *token)
{
    if ((token->quotes == '"' || token->quotes == '\'') && token->content[1] != '$')
        token->type = T_WORD;
    else if (ft_strncmp(token->content, "|", 2) == 0)
	{
        token->type = T_PIPE;
	}
    else if (ft_strncmp(token->content, "<", 2) == 0)
        token->type = T_REDIR_IN;
    else if (ft_strncmp(token->content, ">", 2) == 0)
        token->type = T_REDIR_OUT;
    else if (ft_strncmp(token->content, ">>", 3) == 0)
        token->type = T_APPEND;
    else if (ft_strncmp(token->content, "<<", 3) == 0)
        token->type = T_HEREDOC;
    else if (ft_strncmp(token->content, "$", 1) == 0
            || ft_strncmp(token->content, "\"$", 2) == 0
            || ft_strncmp(token->content, "'$", 2) == 0)
        token->type = T_ENVVAR;
    else
        token->type = T_WORD;
}

t_token	*create_token(char *content, char quote)
{
	t_token	*new;

	new = malloc(sizeof(t_token));
	if (!new)
		return (NULL);
	new->content = content;
	printf("quote: %c\n", quote);
	new->quotes = quote;
	new->heredoc_pipe_read_fd = -1;
	new->has_space_after = 0;  // Initialisé à 0 par défaut
	new->next = NULL;
	set_token_type(new);
	return (new);
}

void	set_index(t_token *tokens)
{
	t_token	*current;
	int		i;

	current = tokens;
	i = 0;
	while (current)
	{
		current->index = i++;
		current = current->next;
	}
}

void	add_token(t_token **tokens, t_token *new)
{
	t_token *current;

	if (!*tokens)
	{
		*tokens = new;
		new->index = 0;
		return ;
	}
	current = *tokens;
	while (current->next)
		current = current->next;
	current->next = new;
}

void merge_tokens_without_space(t_token **tokens)
{
    printf("\n=== merge_tokens_without_space ===\n");
    t_token *cur = *tokens;
    while (cur && cur->next)
    {
        printf("Current token: %s (type: %d, quote: %c, has_space_after: %d)\n", 
            cur->content, cur->type, cur->quotes, cur->has_space_after);
        printf("Next token: %s (type: %d, quote: %c)\n", 
            cur->next->content, cur->next->type, cur->next->quotes);

        // On ne merge pas si :
        // 1. Il y a un espace entre les tokens
        // 2. Le token actuel est un opérateur (PIPE, REDIR_IN, etc.)
        // 3. Le token suivant est un opérateur
        if (!cur->has_space_after && 
            cur->type != T_PIPE && cur->type != T_REDIR_IN && 
            cur->type != T_REDIR_OUT && cur->type != T_APPEND && 
            cur->type != T_HEREDOC &&
            cur->next->type != T_PIPE && cur->next->type != T_REDIR_IN && 
            cur->next->type != T_REDIR_OUT && cur->next->type != T_APPEND && 
            cur->next->type != T_HEREDOC)
        {
            printf("Merging tokens...\n");
            char *merged = ft_strjoin(cur->content, cur->next->content);
            printf("Merged result: %s\n", merged);
            free(cur->content);
            cur->content = merged;
            cur->has_space_after = cur->next->has_space_after;  // Conserver l'info d'espace du second token
            t_token *to_free = cur->next;
            cur->next = to_free->next;
            free(to_free->content);
            free(to_free);
            continue;
        }
        else
            printf("Skipping merge - incompatible types or space detected\n");
        cur = cur->next;
    }
}

// Cette fonction est maintenant obsolète, on utilise merge_tokens_without_space à la place
void merge_adjacent_tokens(t_token **tokens)
{
    printf("\n=== merge_adjacent_tokens (DEPRECATED) ===\n");
    printf("This function is deprecated, use merge_tokens_without_space instead\n");
    merge_tokens_without_space(tokens);
}

int has_output_redirection(t_token *tokens)
{
    while (tokens)
    {
        if (tokens->type == T_REDIR_OUT || tokens->type == T_APPEND)
            return (1);
        tokens = tokens->next;
    }
    return (0);
}

int is_append(t_token *tokens)
{
    while (tokens)
    {
        if (tokens->type == T_APPEND)
            return (1);
        tokens = tokens->next;
    }
    return (0);
}

char *get_output_filename(t_token *tokens)
{
    while (tokens)
    {
        if ((tokens->type == T_REDIR_OUT || tokens->type == T_APPEND)
            && tokens->next && tokens->next->type == T_WORD)
            return (tokens->next->content);
        tokens = tokens->next;
    }
    return (NULL); // Sécurité
}

t_token **split_tokens_by_pipe(t_token *tokens, int *count)
{
    int n = 1;
    t_token *tmp = tokens;
    while (tmp) {
        if (tmp->type == T_PIPE)
            n++;
        tmp = tmp->next;
    }
    t_token **cmds = malloc((n + 1) * sizeof(t_token *));
    tmp = tokens;
    int i = 0;
    cmds[i++] = tmp;
    while (tmp) {
        if (tmp->type == T_PIPE && tmp->next) {
            cmds[i++] = tmp->next;
        }
        tmp = tmp->next;
    }
    cmds[i] = NULL;
    if (count)
        *count = n;
    return cmds;
}

char **build_argv_from_tokens(t_token *cmd)
{
    int argc = 0;
    t_token *tmp = cmd;
    // Compte le nombre d'arguments utiles jusqu'au prochain pipe
    while (tmp && tmp->type != T_PIPE) {
        if (tmp->type == T_WORD || tmp->type == T_ENVVAR)
            argc++;
        // saute l'argument du >, >>, <, <<, etc.
        if ((tmp->type == T_REDIR_OUT || tmp->type == T_APPEND || tmp->type == T_REDIR_IN || tmp->type == T_HEREDOC) && tmp->next)
            tmp = tmp->next;
        tmp = tmp->next;
    }
    char **argv = malloc((argc + 1) * sizeof(char *));
    tmp = cmd;
    int i = 0;
    while (tmp && tmp->type != T_PIPE) {
        if (tmp->type == T_WORD || tmp->type == T_ENVVAR)
            argv[i++] = tmp->content;
        if ((tmp->type == T_REDIR_OUT || tmp->type == T_APPEND || tmp->type == T_REDIR_IN || tmp->type == T_HEREDOC) && tmp->next)
            tmp = tmp->next;
        tmp = tmp->next;
    }
    argv[i] = NULL;
    return argv;
}

int get_heredoc_fd_from_segment(t_token *seg)
{
    while (seg)
    {
        if (seg->type == T_HEREDOC && seg->heredoc_pipe_read_fd != -1)
            return seg->heredoc_pipe_read_fd;
        if (seg->type == T_PIPE)
            break; // ne dépasse pas ce segment
        seg = seg->next;
    }
    return -1;
}
