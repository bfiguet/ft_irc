/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmdInvite.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aalkhiro <aalkhiro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/16 15:50:06 by bfiguet           #+#    #+#             */
/*   Updated: 2024/02/23 15:46:14 by aalkhiro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Irc.hpp"

// Command: INVITE <nickname> <channel>
int	cmdInvite(Server *server, std::vector<std::string> str, User *user){
	if (str.size() < 3)
	{
		user->addMsgToSend(ERR_NEEDMOREPARAMS(str[0]));
		return 1;
	}
	Channel	*cha = server->findChannel(str[2]);
	User	*userNew = server->findUser(str[1]);
	if (cha == NULL)
	{
		user->addMsgToSend(ERR_NOSUCHCHANNEL(str[2]));
		return 1;
	}
	if (cha->isInChannel(user) == false)
	{
		user->addMsgToSend(ERR_NOTONCHANNEL(cha->getName()));
		return 1;
	}
	if (cha->isInvited(user) == true)
	{
		user->addMsgToSend(ERR_CHANOPRIVSNEEDED(cha->getName(), user->getNick()));
		return 1;
	}
	if (userNew == NULL)
	{
		user->addMsgToSend(ERR_NOSUCHNICK(str[1]));
		return 1;
	}
	if (cha->isInChannel(userNew) == true)
	{
		user->addMsgToSend(ERR_USERONCHANNEL(user->getNick(), userNew->getNick(), cha->getName()));
		return 1;
	}
	if (!cha->isInvited(userNew))
		cha->setInviteUser(userNew, true);
	user->addMsgToSend(RPL_INVITING(user->getNick(), user->getUser(), server->getHost(), userNew->getNick(), cha->getName()));
	userNew->addMsgToSend(INVITE(user->getNick(), user->getUser(), server->getHost(), userNew->getNick(), cha->getName()));
	return 0;
}
