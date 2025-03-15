NAME = ircserv

SRC = src/main.cpp

OBJS = $(SRC:.cpp=.o)

CC = c++

CFLAGS = -Wall -Wextra -Werror -std=c++98

all: $(NAME)

irssi:
	docker run -it --name irssi -e TERM -u $(id -u):$(id -g) \
	--log-driver=none \
	   -v ${HOME}/.irssi:/home/user/.irssi:ro \
	irssi
	docker rm -f irssi 2>/dev/null

.cpp.o:
	$(CC) $(CFLAGS) -c $< -o $@

$(NAME): $(OBJS)
	$(CC) $(OBJS) $(CFLAGS) -o $(NAME)

clean:
	@$(RM) $(OBJS)

fclean: clean
	@$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re

