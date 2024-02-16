# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: bfiguet <bfiguet@student.42.fr>            +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/01/09 13:35:00 by bfiguet           #+#    #+#              #
#    Updated: 2024/02/16 17:33:29 by bfiguet          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME		= ircserv
CC			= c++
CFLAGS		= -Wall -Wextra -Werror -g3 -std=c++98

I			= includes
INCS		= $(wildcard $I/*.hpp)

S			= sources
SRCS		=	$(wildcard $S/*.cpp) \
				$(wildcard $S/cmds/*.cpp)

O			= obj
OBJS		= $(SRCS:$S/%.cpp=$O/%.o)

# valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --track-fds=yes
# lsof -i -a -c ircserv  // leaks fd (sockets).

#irssi -c localhost -p port -w password -n nick1

all:	$(NAME)

$O:
	@mkdir -p $O

$O/%.o: $S/%.cpp Makefile $(INCS) | $O
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -I$I -c $< -o $@

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) -I$I $(OBJS) -o $@

clean:
	rm -rdf $O

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all re clean fclean