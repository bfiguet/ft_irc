/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmdKick.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bfiguet <bfiguet@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/16 15:51:50 by bfiguet           #+#    #+#             */
/*   Updated: 2024/02/28 14:36:21 by bfiguet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Irc.hpp"

std::string joinArgs(size_t i, std::vector<std::string> args, char add)
{
	std::string str = "";
	for (; i < args.size(); i++)
	{
		str += add;
		str += args[i];
	}
	return str;
}

int	errKick(Channel* cha, User* user, std::vector<std::string> args, User* userToDel)
{
	if (userToDel == NULL)
	{
		user->addMsgToSend(ERR_NOSUCHNICK(args[2]));
		return 1;
	}
	if (cha == NULL)
	{
		user->addMsgToSend(ERR_NOSUCHCHANNEL(args[1]));
		return 1;
	}
	if (cha->isInChannel(user) == false)
	{
		user->addMsgToSend(ERR_NOTONCHANNEL(args[1]));
		return 1;
	}
	if (cha->isInChannel(userToDel) == false)
	{
		user->addMsgToSend(ERR_USERNOTINCHANNEL(userToDel->getNick(), args[1]));
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
int	cmdKick(Server *server, std::vector<std::string> args, User *user){
	std::string	reason = "";
	if (args.size() < 3)
	{
		user->addMsgToSend(ERR_NEEDMOREPARAMS(args[0]));
		return 1;
	}
	Channel *cha = server->findChannel(args[1]);
	User	*userToDel = server->findUser(args[2]);
	if (errKick(cha, user, args, userToDel) == 1)
		return 1;
	if (args.size() > 3)
	{
		reason = args[3].substr(1);
		reason += joinArgs(4, args, ' ');
	}
	cha->broadcast(KICK(user->getNick(), user->getUser(), user->getHost(), cha->getName(), userToDel->getNick(), reason));
	cha->delUser(userToDel);
	return 0;
}
