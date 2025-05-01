/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: psirault <psirault@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/27 08:16:11 by psirault          #+#    #+#             */
/*   Updated: 2025/05/01 15:53:31 by psirault         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PIPEX_H
# define PIPEX_H
# include <unistd.h>
# include <stdlib.h>
# include <stdio.h>
# include <sys/types.h>
# include <fcntl.h>
# include "minishell.h"
# include "lexer.h"
# include "../libft/libft.h"

typedef struct s_token t_token;

void	wrong_args(void);
char	**ft_get_paths(char *var, char **env);
char	*path_of_cmd(char *cmd, char **paths);
void	exec_child(int *fd, char **argv, char **env);
void	exec_parent(int *fd, char **argv, char **env);
void	exec_cmd(char *cmd, char **env);
void	exec_cmd_tokens(t_token *tokens, char *cmd, char **env);
void	pipex(int argc, char **argv, char **env);
#endif