/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmdQuit.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bfiguet <bfiguet@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/16 15:54:24 by bfiguet           #+#    #+#             */
/*   Updated: 2024/02/16 15:54:42 by bfiguet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Irc.hpp"

//Command: QUIT
int	cmdQuit(Server *server, std::vector<std::string> str, User *user){
	//std::cout << "--cmdQuit--" << std::endl;
	(void) str;
	(void) server;
	std::cout << user->getNick() << " on fd " << user->getFd() << " has leaving" << std::endl;
	user->setDisconnect(true);
	return 0;
}
