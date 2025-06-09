/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbariol- <nassimbariol@student.42.fr>>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/07 09:36:04 by psirault          #+#    #+#             */
/*   Updated: 2025/06/09 17:10:38 by nbariol-         ###   ########.fr       */
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
# include <fcntl.h>
# include <signal.h>

typedef struct s_token	t_token;
typedef struct s_data	t_data;
typedef struct s_vars {
	char	*result;
	char	*var_name;
	char	*value;
	char	*tmp;
	int		in_quote;
	char	quote_char;
	int		is_heredoc;
}					t_vars;
typedef struct s_append {
	int	fd;
	int	last_redir;
	int	redir_applied;
}					t_append;

// Function declarations
void	disable_ctrl_backslash(void);
void	mainloop(char *str, char **envp, t_data *data);
int		handle_heredocs(t_token *tokens, char **env, t_data *data);
void	ft_pwd(t_data *data);
int		is_builtin(char *cmd);
void	close_all_except_std(void);
void	ft_cd(char *str, char **env, t_data *data);
void	ft_env(char **strs, t_data *data);
void	ft_exit(char **envp, t_token *tokens, t_data *data);
void	ft_echo(t_token *token, t_data *data);
int		handle_builtins(char **envp, t_token *token, t_data *data);
char	**env_dup(char **envp);
void	handle_shlvl(char **envp);
void	oldpwd_update(char **env, char *new_oldpwd);
void	pwd_update(char **env, char *new_pwd);
void	ft_export(char **env, t_token *token, t_data *data);
void	ft_unset(t_token *token, char **env, t_data *data);
char	*ft_getenv(char **env, char *name);
int		ft_setenv(char **env, const char *name,
			const char *value, int overwrite);
int		export_len(char *arg);
void	*ft_realloc(void *dst, size_t old_size, size_t new_size);
void	sigint_prompt(int sig);
void	signal_handler(void);
char	*parse_export1(char *arg);
char	*parse_export2(char *arg);
void	replace_by_value(char **arg, char **envp);
void	replace_env_vars(t_token *tokens, char **envp, t_data *data);
int		handle_append_redirection(t_token *tokens);
int		handle_redirections(t_token *tokens);
char	*remove_quotes(char *str);
int		handle_input_redirection(t_token *tokens);
char	*replace_vars_in_str(t_token *token, char *str, char **envp,
			t_data *data);
void	cleanup(char **cmdtab, char **env, t_token *tokens, t_data *data);
size_t	get_var_name_len(char *str);
char	*strjoin_and_free_s1(char *s1, const char *s2);
int		is_valid_var_char(char c);
char	*get_env_value(char *var_name, char **envp, t_data *data);
void	handle_failed_redir(char *str, t_data *data, t_append *append);
void	restore_stdout(t_data *data, t_append *append, int is_builtin);
void	handle_out_redirections(t_data *data, t_append *append, t_token *redir);
void	readline_loop(char *str, char **envp, t_data *data);
void	redirection_exec(t_data *data, t_append *append, t_token *redir);
int		gestion_heredocs(t_data *data, char **envp, char *str);
int		check_pipe_syntax_error(t_token *current, t_data *data, char *str);
void	exec_cmd_tokens(t_data *data, char **envp);
int		is_stdout_redirected(t_token *tokens);
int		handle_pipe_redirections(t_token *tokens,
			int *prev_pipe_read, char **env, t_data *data);
t_token	*find_command_start_from_segment(t_token *current_segment_token);
void	exec_cmd_common(char **cmdtab, char **env, t_data *data);
void	dup2_and_close(int oldfd, int newfd);
void	exec_cmd_tokens(t_data *data, char **envp);
void	perror_exit(const char *msg);
void	execute_simple_redirection(char *str, char **envp, t_data *data,
		t_append *append);

#endif