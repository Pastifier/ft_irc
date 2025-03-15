NAME = ircserv

SRC = src/main.cpp

INCLUDE_DIR := include/

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
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

$(NAME): $(OBJS)
	$(CC) $(OBJS) -I$(INCLUDE_DIR) $(CFLAGS) -o $(NAME)

clean:
	@$(RM) $(OBJS)

fclean: clean
	@$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re

