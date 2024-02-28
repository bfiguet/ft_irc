/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmdQuit.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bfiguet <bfiguet@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/16 15:54:24 by bfiguet           #+#    #+#             */
/*   Updated: 2024/02/28 13:42:46 by bfiguet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Irc.hpp"

//Command: QUIT
int	cmdQuit(Server *server, std::vector<std::string> args, User *user){
	(void) args;
	(void) server;
	std::cout << user->getNick() << " on fd " << user->getFd() << " has left" << std::endl;
	user->setDisconnect(true);
	return 0;
}
