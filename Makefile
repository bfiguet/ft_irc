# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: bfiguet <bfiguet@student.42.fr>            +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/01/09 13:35:00 by bfiguet           #+#    #+#              #
#    Updated: 2024/01/25 18:43:53 by bfiguet          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME		= ircserv
CC			= c++
CFLAGS		= -Wall -Wextra -Werror -g3 -std=c++98

INC_PATH	= ./includes/
INC			= -I $(INC_PATH)

SRC_PATH	= ./sources/
SRC			= $(wildcard $(SRC_PATH)*.cpp)

OBJ_PATH	= ./obj/
OBJ			= $(SRC:$(SRC_PATH)%.cpp=$(OBJ_PATH)%.o)

# valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --track-fds=yes
# lsof -i -a -c ircserv  // leaks fd (sockets).

all:	$(OBJ_PATH) $(NAME)

$(OBJ_PATH):
	mkdir -p $(OBJ_PATH)

$(OBJ_PATH)%.o: $(SRC_PATH)%.cpp
	$(CC) $(CFLAGS) -c $< -o $@ $(INC)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $@ $(INC)

clean:
	rm -rf $(OBJ_PATH)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all re clean fclean