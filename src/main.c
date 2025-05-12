/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: psirault <psirault@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/07 09:34:28 by psirault          #+#    #+#             */
/*   Updated: 2025/05/07 13:54:55 by psirault         ###   ########.fr       */
/*                                                                            */
/* *************************************************************************

//double leaks

#include "../includes/minishell.h"
#include <signal.h>

// Nouvelle fonction helper
static int has_pipe(t_token *tokens)
{
	t_token *current = tokens;
	while (current != NULL)
	{
		if (current->type == T_PIPE)
			return (1);
		current = current->next;
	}
	return (0);
}

void	disable_ctrl_backslash(void)
{
	struct termios	term;

	if (tcgetattr(STDIN_FILENO, &term) == -1)
	{
		perror("tcgetattr");
		exit(EXIT_FAILURE);
	}
	term.c_cc[VQUIT] = 0;
	if (tcsetattr(STDIN_FILENO, TCSANOW, &term) == -1)
	{
		perror("tcsetattr");
		exit(EXIT_FAILURE);
	}
}

void	readline_loop(char *str, char **envp, t_token *tokens)
{
	replace_env_vars(tokens, envp);
	if (handle_heredocs(tokens, envp) == -1)
	{
		ft_putstr_fd("minishell: error handling heredocs\n", 2);
		free(str);
		free_tokens(tokens->first);
		return;
	}

	if (has_pipe(tokens->first)) // Vérifier sur tokens->first si la liste est chaînée via first
	{
		exec_cmd_tokens(tokens->first, envp);
	}
	else
	{
		if (!handle_builtins(envp, tokens->first))
		{		
			exec_cmd_tokens(tokens->first, envp);
		}
	}
	free(str);
	free_tokens(tokens->first);
}

void	mainloop(char *str, char **envp, t_token *tokens)
{
	while (1)
	{
		str = readline("minishell$> ");
		if (str == NULL)
		{
			free(str);
			break ;
		}
		add_history(str);
		if (str[0] == '\n' || str[0] == '\0')
		{
			free(str);
			continue ;
		}
		quote_and_token_handling(str, find_first_quote(str), &tokens);
		if (syntax_checker(tokens))
		{
			free(str);
			free_tokens(tokens->first);
			continue ;
		}
		readline_loop(str, envp, tokens);
	}
	free(str);
}

int	main(int ac, char **av, char **envp)
{
	char		*str;
	char		**env_cpy;
	t_token		*tokens;

	tokens = NULL;
	env_cpy = env_dup(envp);
	(void)ac;
	(void)av;
	disable_ctrl_backslash();
	signal_handler();
	str = NULL;
	mainloop(str, env_cpy, tokens);
	ft_free(env_cpy);
	free(str);
	//free_tokens(tokens);
#ifdef __APPLE__
	clear_history();
#else
	rl_clear_history();
#endif
	return (0);
}
