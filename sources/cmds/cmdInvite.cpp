/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmdInvite.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aalkhiro <aalkhiro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/16 15:50:06 by bfiguet           #+#    #+#             */
/*   Updated: 2024/02/29 12:26:54 by aalkhiro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Irc.hpp"

// Command: INVITE <nickname> <channel>
int	cmdInvite(ServerData *serverData, std::vector<std::string> args, User *user){
	if (args.size() < 3)
	{
		user->addMsgToSend(ERR_NEEDMOREPARAMS(args[0]));
		return 1;
	}
	Channel	*cha = serverData->findChannel(args[2]);
	User	*userNew = serverData->findUser(args[1]);
	if (cha == NULL)
	{
		user->addMsgToSend(ERR_NOSUCHCHANNEL(args[2]));
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
		user->addMsgToSend(ERR_NOSUCHNICK(args[1]));
		return 1;
	}
	if (cha->isInChannel(userNew) == true)
	{
		user->addMsgToSend(ERR_USERONCHANNEL(user->getNick(), userNew->getNick(), cha->getName()));
		return 1;
	}
	if (!cha->isInvited(userNew))
		cha->setInviteUser(userNew, true);
	user->addMsgToSend(RPL_INVITING(user->getNick(), user->getUser(), serverData->getHost(), userNew->getNick(), cha->getName()));
	userNew->addMsgToSend(INVITE(user->getNick(), user->getUser(), serverData->getHost(), userNew->getNick(), cha->getName()));
	return 0;
}
