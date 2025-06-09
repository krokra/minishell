/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: psirault <psirault@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/27 08:16:11 by psirault          #+#    #+#             */
/*   Updated: 2025/06/09 17:50:55 by psirault         ###   ########.fr       */
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

typedef struct s_token	t_token;
typedef struct s_data	t_data;
typedef struct s_execmeta
{
	t_token		**cmds;
	int			i;
	int			prev_pipe_read;
	int			pipefd[2];
	int			has_next;
	int			status;
	int			n_cmds;
	pid_t		pid;
	pid_t		pids[256];
}				t_execmeta;
void	wrong_args(void);
char	**ft_get_paths(char *var, char **env);
char	*path_of_cmd(char *cmd, char **paths);
void	exec_child(int *fd, char **argv, char **env);
void	exec_parent(int *fd, char **argv, char **env);
void	exec_cmd(char *cmd, char **env);
void	exec_cmd_tokens(t_data *data, char **env);
void	pipex(int argc, char **argv, char **env);
void	while_exec(t_execmeta *meta, t_data *data, char **envp);
void	wait_for_children(t_execmeta *meta, t_data *data);
#endif