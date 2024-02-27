/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmdKill.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aalkhiro <aalkhiro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/16 15:52:54 by bfiguet           #+#    #+#             */
/*   Updated: 2024/02/27 10:51:52 by aalkhiro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Irc.hpp"

//Command: KILL <nickname> <comment>
int	cmdKill(Server *server, std::vector<std::string> str, User *user){
	std::string	reason = "";

	if (str.size() < 2)
	{
		user->addMsgToSend(ERR_NEEDMOREPARAMS(str[0]));
		return 1;
	}
	if (server->findUser(user->getFd()) == NULL)
	{
		user->addMsgToSend(ERR_NOPRIVILEGES);
		return 1;
	}
	else if (server->findUser(str[1]) != NULL)
	{
		if (str[2].size() > 0)
			reason = str[2];
		// server->delUser(server->findUser(str[1]));
		server->findUser(str[1])->setDisconnect(true);
		user->addMsgToSend(KILL(user->getNick(), user->getUser(), user->getHost(), str[1], reason));
		return 0;
	}
	user->addMsgToSend(ERR_NOSUCHSERVER);
	return 1;
}
