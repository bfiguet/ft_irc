/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmdPrivmsg.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bfiguet <bfiguet@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/16 15:55:30 by bfiguet           #+#    #+#             */
/*   Updated: 2024/02/27 15:41:05 by bfiguet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Irc.hpp"

//Command: PRIVMSG <target> <text to be sent>
int	cmdPrivmsg(Server *server, std::vector<std::string> str, User *user){
	std::string	msg;
	User* dest = server->findUser(str[1]);
	Channel	*cha = server->findChannel(str[1]);

	if (str.size() < 3)
	{
		user->addMsgToSend(ERR_NEEDMOREPARAMS(str[0]));
		return 1;
	}
	if (str[2][0] == ':')
		msg = str[2].substr(1);
	for (size_t i = 3; i < str.size(); i++)
	{
		msg += " ";
		msg += str[i];
	}
	if (str[1][0] != '#' && str[1][0] != '&') //tagret == user
	{
		if (dest)
			dest->addMsgToSend(MSG(user->getNick(), user->getUser(), user->getHost(), str[0], dest->getNick(), msg));
		else
		{
			user->addMsgToSend(ERR_NOSUCHNICK(str[1]));
			return 1;
		}
	}
	else // target == channel
	{
		if (cha)
		{
			if (cha->isInChannel(user) == false)
			{
				user->addMsgToSend(ERR_CANNOTSENDTOCHAN(user->getNick(), cha->getName()));
				return 1;
			}
			std::vector<User *> listUser = cha->getUsers();
			for (std::vector<User*>::iterator it = listUser.begin(); it != listUser.end(); it++)
			{
				if ((*it)->getNick() == user->getNick())
					continue;
				(*it)->addMsgToSend(MSG(user->getNick(), user->getUser(), user->getHost(), str[0], cha->getName(), msg));
			}
		}
		else
		{
			user->addMsgToSend(ERR_NOSUCHCHANNEL(str[1]));
			return 1;
		}
	}
	return 0;
}

//optimization fun
