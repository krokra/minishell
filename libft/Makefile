CC := cc
RM := rm -rf
AR := ar rcs
CFLAGS := -Wall -Werror -Wextra -g3
NAME := libft.a
SRC := ft_isascii.c \
			ft_atoi.c \
			ft_memcmp.c \
			ft_putendl_fd.c \
			ft_strdup.c \
			ft_strlen.c \
			ft_strtrim.c \
			ft_bzero.c \
			ft_isdigit.c \
			ft_memcpy.c \
			ft_putnbr_fd.c \
			ft_striteri.c \
			ft_strmapi.c \
			ft_substr.c \
			ft_calloc.c \
			ft_isprint.c \
			ft_memmove.c \
			ft_putstr_fd.c \
			ft_strjoin.c \
			ft_strncmp.c \
			ft_tolower.c \
			ft_isalnum.c \
			ft_itoa.c \
			ft_memset.c \
			ft_split.c \
			ft_strlcat.c \
			ft_strnstr.c \
			ft_toupper.c \
			ft_isalpha.c \
			ft_memchr.c \
			ft_putchar_fd.c \
			ft_strchr.c \
			ft_strlcpy.c \
			ft_strrchr.c \
			ft_lstnew_bonus.c \
			ft_lstadd_front_bonus.c \
			ft_lstsize_bonus.c \
			ft_lstlast_bonus.c \
			ft_lstadd_back_bonus.c \
			ft_lstdelone_bonus.c \
			ft_lstclear_bonus.c \
			ft_lstiter_bonus.c \
			ft_lstmap_bonus.c \
			is_str_empty.c \
			ft_lstavantlast.c \
			ft_rev_int_tab.c \
			ft_is_int.c \
			ft_count_words.c

INCLUDES := libft.h

OBJ := $(SRC:.c=.o)

all: $(NAME)

$(NAME): $(OBJ)
	$(AR) $(NAME) $(OBJ)

.o:     %.c
	$(CC) $(CFLAGS) -c $< -o ${<:.c=.o}

clean:
	$(RM) $(OBJ)

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHOMY: all clean fclean re
