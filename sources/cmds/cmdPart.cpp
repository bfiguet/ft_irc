/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmdPart.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aalkhiro <aalkhiro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/16 15:50:31 by bfiguet           #+#    #+#             */
/*   Updated: 2024/02/29 12:33:01 by aalkhiro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Irc.hpp"

//Command: PART <channel> <reason>
int	cmdPart(ServerData *serverData, std::vector<std::string> args, User *user)
{
	std::string	reason = "";
	Channel* chan = serverData->findChannel(args[1]);
	if (args.size() < 2)
	{
		user->addMsgToSend(ERR_NEEDMOREPARAMS(args[0]));
		return 1;
	}
	if (chan == NULL)
	{
		user->addMsgToSend(ERR_NOSUCHCHANNEL(args[1]));
		return 1;
	}
	if (chan->isInChannel(user) == false)
	{
		user->addMsgToSend(ERR_NOTONCHANNEL(args[1]));
		return 1;
	}
	if (args.size() > 1)
		reason = joinArgs(2, args, ' ');
	user->addMsgToSend(PART(user->getNick(), user->getUser(), user->getHost(), args[1], reason));
	chan->delUser(user);
	return 0;
}
