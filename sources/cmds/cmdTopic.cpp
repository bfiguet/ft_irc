/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmdTopic.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bfiguet <bfiguet@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/16 15:52:28 by bfiguet           #+#    #+#             */
/*   Updated: 2024/02/16 15:52:44 by bfiguet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Irc.hpp"

//Command: TOPIC <channel> [<topic>]
int	cmdTopic(Server *server, std::vector<std::string> str, User *user){
	std::cout << "--cmdTopic--" << std::endl;
	std::string	topic;

	if (str.size() < 2)
	{
		user->addMsgToSend(ERR_NEEDMOREPARAMS(str[0]));
		return 1;
	}
	Channel	*cha = server->findChannel(str[1]);
	if (cha == NULL)
	{
		user->addMsgToSend(ERR_NOSUCHCHANNEL(cha->getName()));
		return 1;
	}
	if (cha->isInChannel(user) == false)
	{
		user->addMsgToSend(ERR_NOTONCHANNEL(cha->getName()));
		return 1;
	}
	if (str.size() == 2)
	{
		if (cha->getTopic() == "")
			user->addMsgToSend(RPL_NOTOPIC(user->getNick(), user->getUser(), server->getHost(), cha->getName()));
		else
			user->addMsgToSend(RPL_TOPIC(user->getNick(), user->getUser(), server->getHost(), cha->getName(), cha->getTopic()));
	}
	else
	{
		if (cha->isTopicChange() == false && cha->isOperator(user) == false)
		{
			user->addMsgToSend(ERR_CHANOPRIVSNEEDED(cha->getName(), user->getNick()));
			return 1;
		}
		topic = str[2].substr(1);
		if (topic.size() < 1)
			cha->setTopic("");
		else if ((cha->isTopicChange() == true) || (cha->isTopicChange() == false && cha->isOperator(user) == true))
		{
			cha->setTopic(topic);
			std::vector<User*>	listUser = cha->getUsers();
			for (std::vector<User*>::iterator it = listUser.begin(); it != listUser.end(); it++)
				(*it)->addMsgToSend(TOPIC(user->getNick(), user->getUser(), server->getHost(), cha->getName(), topic));
		}
	}
	return 0;
}
