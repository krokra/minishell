/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: psirault <psirault@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/07 09:34:28 by psirault          #+#    #+#             */
/*   Updated: 2025/05/21 11:30:37 by psirault         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

//double leaks

#include "../includes/minishell.h"
#include <signal.h>

// Nouvelle fonction helper
static int has_pipe(t_token *tokens)
{
	t_token *current;
	current = tokens;
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

void	readline_loop(char *str, char **envp, t_data *data)
{
	printf("[READLINE] Début du traitement de la commande\n");
	int saved_stdout;
	int redir_applied;
	int last_redir;
	t_token *redir;
	int	fd;

	printf("[READLINE] Remplacement des variables d'environnement\n");
	replace_env_vars(data->tokens, envp, data);
	
	printf("[READLINE] Gestion des heredocs\n");
	if (handle_heredocs(data->tokens, envp, data) == -1)
	{
		printf("[READLINE] ERREUR: Échec de la gestion des heredocs\n");
		ft_putstr_fd("minishell: error handling heredocs\n", 2);
		free(str);
		free_tokens(data->tokens->first);
		data->tokens = NULL;
		return;
	}

	if (has_pipe(data->tokens->first))
	{
		printf("[READLINE] Pipe détecté, exécution avec pipes\n");
		saved_stdout = dup(STDOUT_FILENO);
		exec_cmd_tokens(data, envp);
		dup2(saved_stdout, STDOUT_FILENO);
		close(saved_stdout);
		if (data->tokens->heredoc_pipe_read_fd != -1)
			close(data->tokens->heredoc_pipe_read_fd);
	}
	else
	{
		printf("[READLINE] Pas de pipe, exécution simple\n");
		saved_stdout = -1;
		redir_applied = 0;
		redir = data->tokens->first;
		while (redir)
		{
			last_redir = 0;
			if (redir->type == T_APPEND || redir->type == T_REDIR_OUT)
			{
				printf("[READLINE] Redirection détectée\n");
				if (saved_stdout == -1)
					saved_stdout = dup(STDOUT_FILENO);
				if (redir->type == T_APPEND)
				{
					printf("[READLINE] Redirection append\n");
					if (handle_append_redirection(redir) < 0)
						last_redir = 1;
					else
						last_redir = 0;
				}
				else
				{
					printf("[READLINE] Redirection simple\n");
					fd = handle_redirections(redir);
					if (fd < 0)
						last_redir = 1;
					else
						last_redir = 0;
					close(fd);
				}
				redir_applied = 1;
			}
			redir = redir->next;
		}
		if (last_redir < 0)
		{
			printf("[READLINE] ERREUR: Échec de la redirection\n");
			if (redir_applied && saved_stdout != -1)
				close(saved_stdout);
			free(str);
			free_tokens(data->tokens->first);
			data->tokens = NULL;
			return;
		}
		if (!handle_builtins(envp, data->tokens->first, data))
		{
			printf("[READLINE] Exécution de la commande\n");
			exec_cmd_tokens(data, envp);
		}
		if (redir_applied && saved_stdout != -1)
		{
			printf("[READLINE] Restauration de la sortie standard\n");
			dup2(saved_stdout, STDOUT_FILENO);
			close(saved_stdout);
		}
		if (data->tokens->heredoc_pipe_read_fd != -1)
			close(data->tokens->heredoc_pipe_read_fd);
	}
	printf("[READLINE] Nettoyage\n");
	write(STDOUT_FILENO, "\n", 1);
	free(str);
	free_tokens(data->tokens->first);
	data->tokens = NULL;
}

void	mainloop(char *str, char **envp, t_data *data)
{
	printf("[MAINLOOP] Démarrage de la boucle principale\n");
	while (1)
	{
		str = readline("minishell$> ");
		printf("[MAINLOOP] Ligne lue: '%s'\n", str ? str : "NULL");
		if (str == NULL)
		{
			printf("[MAINLOOP] EOF détecté\n");
			free(str);
			break;
		}
		add_history(str);
		if (str[0] == '\n' || str[0] == '\0')
		{
			printf("[MAINLOOP] Ligne vide, skip\n");
			free(str);
			continue;
		}
		printf("[MAINLOOP] Traitement de la ligne\n");
		data->tokens = NULL;  // Réinitialisation avant le traitement
		quote_and_token_handling(str, find_first_quote(str), &data);
		if (syntax_checker(data->tokens))
		{
			printf("[MAINLOOP] Erreur de syntaxe détectée\n");
			free(str);
			if (data->tokens != NULL)
			{
				free_tokens(data->tokens->first);
				data->tokens = NULL;  // Réinitialisation après l'erreur de syntaxe
			}
			continue;
		}
		printf("[MAINLOOP] Exécution de la commande\n");
		readline_loop(str, envp, data);
	}
	printf("[MAINLOOP] Fin de la boucle principale\n");
	free(str);
}

int	main(int argc, char **argv, char **envp)
{
	printf("\n[MAIN] Démarrage du programme\n");
	printf("[MAIN] argc: %d\n", argc);
	printf("[MAIN] envp: %p\n", (void*)envp);
	
	char *str;
	char **env_cpy;
	t_token *tokens;
	t_data *data;

	data = malloc(sizeof(t_data));
	if (!data)
	{
		printf("[MAIN] ERREUR: Échec d'allocation de data\n");
		perror("malloc");
		return (1);
	}
	printf("[MAIN] Data allouée: %p\n", (void*)data);
	
	(void)argc;
	(void)argv;
	tokens = NULL;
	data->tokens = tokens;
	data->exit_status = 0;
	data->status_getter = 0;
	
	printf("[MAIN] Copie de l'environnement\n");
	env_cpy = env_dup(envp);
	printf("[MAIN] Environnement copié: %p\n", (void*)env_cpy);
	
	printf("[MAIN] Configuration des signaux\n");
	disable_ctrl_backslash();
	signal_handler();
	
	str = NULL;
	printf("[MAIN] Démarrage de la boucle principale\n");
	mainloop(str, env_cpy, data);
	
	printf("[MAIN] Nettoyage final\n");
	ft_free(env_cpy);
	free(str);
	free(data);
#ifdef __APPLE__
	clear_history();
#else
	rl_clear_history();
#endif
	printf("[MAIN] Fin du programme\n");
	return (0);
}
