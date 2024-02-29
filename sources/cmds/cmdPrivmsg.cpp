/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmdPrivmsg.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aalkhiro <aalkhiro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/16 15:55:30 by bfiguet           #+#    #+#             */
/*   Updated: 2024/02/29 14:48:15 by aalkhiro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Irc.hpp"

//Command: PRIVMSG <target> <text to be sent>
int	cmdPrivmsg(ServerData *serverData, std::vector<std::string> args, User *user){
	std::string	msg;
	User* dest;
	Channel	*cha;

	if (args.size() < 3)
	{
		user->addMsgToSend(ERR_NEEDMOREPARAMS(args[0]));
		return 1;
	}
	dest = serverData->findUser(args[1]);
	cha = serverData->findChannel(args[1]);
	msg = args[2].substr(1);
	msg += joinArgs(3, args, ' ');
	if (args[1][0] != '#' && args[1][0] != '&') //tagret == user
	{
		if (!dest)
		{
			user->addMsgToSend(ERR_NOSUCHNICK(args[1]));
			return 1;
		}
		dest->addMsgToSend(MSG(user->getNick(), user->getUser(), user->getHost(), args[0], dest->getNick(), msg));
	}
	else // target == channel
	{
		if (cha)
		{
			if (cha->isInChannel(user))
				cha->broadcast(MSG(user->getNick(), user->getUser(), user->getHost(), args[0], cha->getName(), msg), user);
			else
			{
				user->addMsgToSend(ERR_CANNOTSENDTOCHAN(user->getNick(), args[1]));
				return 1;
			}
		}
		else
		{
			user->addMsgToSend(ERR_NOSUCHCHANNEL(args[1]));
			return 1;
		}
	}
	return 0;
}
