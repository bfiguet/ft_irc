/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmdPart.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bfiguet <bfiguet@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/16 15:50:31 by bfiguet           #+#    #+#             */
/*   Updated: 2024/02/27 15:35:39 by bfiguet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Irc.hpp"

//Command: PART <channel> <reason>
int	cmdPart(Server *server, std::vector<std::string> str, User *user)
{
	std::string	reason = "";
	Channel* chan = server->findChannel(str[1]);
	if (str.size() < 2)
	{
		user->addMsgToSend(ERR_NEEDMOREPARAMS(str[0]));
		return 1;
	}
	if (chan == NULL)
	{
		user->addMsgToSend(ERR_NOSUCHCHANNEL(str[1]));
		return 1;
	}
	if (chan->isInChannel(user) == false)
	{
		user->addMsgToSend(ERR_NOTONCHANNEL(str[1]));
		return 1;
	}
	if (str.size() > 1)
		for (size_t i = 2; i < str.size(); i++)
		{
			reason += " ";
			reason += str[i];
		}
	user->addMsgToSend(PART(user->getNick(), user->getUser(), user->getHost(), str[1], reason));
	chan->delUser(user);
	return 0;
}
