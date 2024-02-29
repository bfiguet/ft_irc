/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmdQuit.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aalkhiro <aalkhiro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/16 15:54:24 by bfiguet           #+#    #+#             */
/*   Updated: 2024/02/29 12:33:40 by aalkhiro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Irc.hpp"

//Command: QUIT
int	cmdQuit(ServerData *serverData, std::vector<std::string> args, User *user){
	(void) args;
	(void) serverData;
	std::cout << user->getNick() << " on fd " << user->getFd() << " has left" << std::endl;
	user->setDisconnect(true);
	return 0;
}
