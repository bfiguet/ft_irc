/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmdKick.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bfiguet <bfiguet@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/16 15:51:50 by bfiguet           #+#    #+#             */
/*   Updated: 2024/02/28 13:32:43 by bfiguet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Irc.hpp"

int	errKick(Channel* cha, User* user, std::vector<std::string> str, User* userToDel)
{
	if (userToDel == NULL)
	{
		user->addMsgToSend(ERR_NOSUCHNICK(str[2]));
		return 1;
	}
	if (cha == NULL)
	{
		user->addMsgToSend(ERR_NOSUCHCHANNEL(str[1]));
		return 1;
	}
	if (cha->isInChannel(user) == false)
	{
		user->addMsgToSend(ERR_NOTONCHANNEL(str[1]));
		return 1;
	}
	if (cha->isInChannel(userToDel) == false)
	{
		user->addMsgToSend(ERR_USERNOTINCHANNEL(userToDel->getNick(), str[1]));
		return 1;
	}
	if (cha->isOperator(user) == false)
	{
		user->addMsgToSend(ERR_CHANOPRIVSNEEDED(cha->getName(), user->getNick()));
		return 1;
	}
	return 0;
}
//Command: KICK <channel> <user> *( "," <user> ) [<comment>]
int	cmdKick(Server *server, std::vector<std::string> str, User *user){
	std::string	reason = "";
	if (str.size() < 3)
	{
		user->addMsgToSend(ERR_NEEDMOREPARAMS(str[0]));
		return 1;
	}
	Channel *cha = server->findChannel(str[1]);
	User	*userToDel = server->findUser(str[2]);
	if (errKick(cha, user, str, userToDel) == 1)
		return 1;
	if (str.size() > 3)
	{
		reason = str[3].substr(1);
		for (size_t i = 4; i < str.size(); i++)
		{
			reason += " ";
			reason += str[i];
		}
	}
	cha->broadcast(KICK(user->getNick(), user->getUser(), user->getHost(), cha->getName(), userToDel->getNick(), reason));
	cha->delUser(userToDel);
	return 0;
}
