/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmdPrivmsg.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aalkhiro <aalkhiro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/16 15:55:30 by bfiguet           #+#    #+#             */
/*   Updated: 2024/02/28 13:30:42 by aalkhiro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Irc.hpp"

//Command: PRIVMSG <target> <text to be sent>
int	cmdPrivmsg(Server *server, std::vector<std::string> args, User *user){
	std::string	msg;
	User* dest = server->findUser(args[1]);
	Channel	*cha = server->findChannel(args[1]);

	if (args.size() < 3)
	{
		user->addMsgToSend(ERR_NEEDMOREPARAMS(args[0]));
		return 1;
	}
	if (args[2][0] == ':')
		msg = args[2].substr(1);
	for (size_t i = 3; i < args.size(); i++)
	{
		msg += " ";
		msg += args[i];
	}
	if (args[1][0] != '#' && args[1][0] != '&') //tagret == user
	{
		if (dest)
			dest->addMsgToSend(MSG(user->getNick(), user->getUser(), user->getHost(), args[0], dest->getNick(), msg));
		else
		{
			user->addMsgToSend(ERR_NOSUCHNICK(args[1]));
			return 1;
		}
	}
	else // target == channel
	{
		if (cha)
			cha->broadcast(MSG(user->getNick(), user->getUser(), user->getHost(), args[0], cha->getName(), msg));
		else
		{
			user->addMsgToSend(ERR_NOSUCHCHANNEL(args[1]));
			return 1;
		}
	}
	return 0;
}
