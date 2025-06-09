CC := cc
RM := rm -rf
CFLAGS := -Wall -Werror -Wextra -g3
NAME := minishell
SRC_DIR := src/
SRCS := src/Main/main.c \
		src/Main/main2.c \
		src/Main/main3.c \
		src/pipex/pipex.c \
		src/pipex/pipex_utils.c \
		src/builtins/ft_pwd.c \
		src/builtins/ft_cd.c \
		src/builtins/ft_env.c \
		src/builtins/ft_exit.c \
		src/builtins/ft_echo.c \
		src/builtins/ft_export.c \
		src/builtins/ft_unset.c \
		src/ft_getenv.c \
		src/ft_realloc.c \
		src/signal_handling.c \
		src/builtins/export_utils.c \
		src/builtins/handle_builtins.c \
		src/lexer/lexer.c \
		src/lexer/lexmain.c \
		src/lexer/token_utils.c \
		src/lexer/token_utils2.c \
		src/lexer/token_utils3.c \
		src/lexer/token_print.c \
		src/lexer/token_parser.c \
		src/token_syntax.c \
		src/env_vars.c \
		src/env_vars_utils.c \
		src/env_vars_utils2.c \
		src/env_vars_utils3.c \
		src/env_vars_utils4.c \
		src/heredoc.c \
		src/redirections.c \
		src/lexer/get_token.c

SRC := $(addprefix $(SRC_DIR), $(SRCS))
INCLUDES := includes/minishell.h \
			includes/lexer.h
OBJ := $(SRCS:.c=.o)

all: $(NAME)

$(NAME): $(OBJ)
	@make -C libft
	${CC} ${CFLAGS} $^ -lreadline -Llibft -lft -o ${NAME}

%.o:	$(SRC_DIR)%.c
	${CC} ${CFLAGS} -Ilibft -c $? -o $@

libft:
	make -C libft

clean:
	@make clean -C libft
	$(RM) $(OBJ)

fclean: clean
	@make fclean -C libft
	$(RM) $(NAME)

re: fclean all

.PHOMY: all clean fclean re libft