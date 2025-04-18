/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: psirault <psirault@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/07 09:36:04 by psirault          #+#    #+#             */
/*   Updated: 2025/04/18 18:38:41 by psirault         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# ifndef MINISHELL_H
# define MINISHELL_H

# include "../libft/libft.h"
# include "pipex.h"
# include <stdlib.h>
# include <stdio.h>
# include <unistd.h>
# include <sys/stat.h>
# include <sys/types.h>
# include <signal.h>
# include <readline/history.h>
# include <readline/readline.h>
# include <curses.h>
# include <term.h>
# include <sys/ioctl.h>
# include <sys/wait.h>
# include <limits.h>

void	ft_pwd(void);
void	ft_cd(char *str, char **env);
void	ft_env(char **strs);
void	ft_exit(char **argv);
int		ft_echo(int argc, char **argv);
int		handle_builtins(char **arg, char **envp, int i);
char	**env_dup(char **envp);
void	oldpwd_update(char **env, char *new_oldpwd);
void	pwd_update(char **env, char *new_pwd);
void    ft_export(char **env, char *arg);
void    ft_unset(char *name, char **env);
char    *ft_getenv(char **env, char *name);
int     export_len(char *arg);
void    *ft_realloc(void *dst, size_t old_size, size_t new_size);

#endif