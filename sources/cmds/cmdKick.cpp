/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmdKick.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bfiguet <bfiguet@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/16 15:51:50 by bfiguet           #+#    #+#             */
/*   Updated: 2024/02/22 17:51:47 by bfiguet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Irc.hpp"

//Command: KICK <channel> <user> <comment>
int	cmdKick(Server *server, std::vector<std::string> str, User *user){
	std::cout << "--cmdKick--" << std::endl;
	std::string	reason = "";

	if (str.size() < 3)
	{
		user->addMsgToSend(ERR_NEEDMOREPARAMS(str[0]));
		return 1;
	}
	Channel *cha = server->findChannel(str[1]);
	User	*userToDel = server->findUser(str[2]);
	if (server->findUser(str[2]) != userToDel)
	{
		user->addMsgToSend(ERR_NOSUCHNICK(str[2]));
		return 1;
	}
	else if (cha == NULL)
	{
		user->addMsgToSend(ERR_NOSUCHCHANNEL(str[1]));
		return 1;
	}
	else if (cha->isInChannel(user) == false)
	{
		user->addMsgToSend(ERR_NOTONCHANNEL(str[1]));
		return 1;
	}
	else if (cha->isInChannel(userToDel) == false)
	{
		user->addMsgToSend(ERR_USERNOTINCHANNEL(userToDel->getNick(), str[1]));
		return 1;
	}
	else if (cha->isInvited(user) == true)
	{
		user->addMsgToSend(ERR_CHANOPRIVSNEEDED(cha->getName(), user->getNick()));
		return 1;
	}
	std::vector<User*>	listUser = cha->getUsers();
	if (str[3].size() > 0)
	{
		reason = str[3];
		for (size_t i = 4; i < str.size(); i++)
		{
			reason += " ";
			reason += str[i];
		}
	}
	for (std::vector<User*>::iterator it = listUser.begin(); it != listUser.end(); it++)
		(*it)->addMsgToSend(KICK(user->getNick(), user->getUser(), user->getHost(), cha->getName(), userToDel->getNick(), reason));
	cha->delUser(userToDel);
	cha->setInviteUser(userToDel, false);
	return 0;
}
