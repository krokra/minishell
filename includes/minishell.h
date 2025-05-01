/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: psirault <psirault@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/07 09:36:04 by psirault          #+#    #+#             */
/*   Updated: 2025/05/01 15:35:30 by psirault         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H
# define MAX_ENV_VARS 1024

# include "../libft/libft.h"
# include "pipex.h"
# include "lexer.h"
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
# include <sys/param.h>
# include <limits.h>

typedef struct s_token t_token;

void	ft_pwd(void);
void	ft_cd(char *str, char **env);
void	ft_env(char **strs);
void	ft_exit(char **argv, char **envp, t_token *tokens);
int		ft_echo(int argc, char **argv);
int		handle_builtins(char **arg, char **envp, int i, t_token *tokens);
char	**env_dup(char **envp);
void	oldpwd_update(char **env, char *new_oldpwd);
void	pwd_update(char **env, char *new_pwd);
void	ft_export(char **env, char *arg);
void	ft_unset(char *name, char **env);
char	*ft_getenv(char **env, char *name);
int		export_len(char *arg);
void	*ft_realloc(void *dst, size_t old_size, size_t new_size);
void	sigint_prompt(int sig);
void	signal_handler(void);
int		export_len(char *arg);
char	*parse_export1(char *arg);
char	*parse_export2(char *arg);
void	replace_by_value(char **arg, char **envp);
void	replace_env_vars(t_token *tokens, char **args, char **envp);
#endif