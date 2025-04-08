CC := cc
RM := rm -rf
CFLAGS := -Wall -Werror -Wextra -g3
NAME := minishell
SRC_DIR := src/
SRCS := main.c \
		src/pipex/pipex.c \
		src/pipex/pipex_utils.c \
		src/builtins/ft_pwd.c \
		src/builtins/ft_cd.c \
		src/builtins/ft_env.c \
		src/builtins/ft_exit.c \
		src/builtins/ft_echo.c \
		src/builtins/handle_builtins.c

SRC := $(addprefix $(SRC_DIR), $(SRCS))
INCLUDES := includes/minishell.h
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