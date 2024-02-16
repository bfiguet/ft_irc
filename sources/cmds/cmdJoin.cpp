/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmdJoin.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bfiguet <bfiguet@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/16 15:55:03 by bfiguet           #+#    #+#             */
/*   Updated: 2024/02/16 15:55:10 by bfiguet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Irc.hpp"

//Command: JOIN <channel> <key>
int	cmdJoin(Server *server, std::vector<std::string> str, User *user){
	std::cout << "--cmdJoin--" << std::endl;

	if (str.size() < 2)
	{
		user->addMsgToSend(ERR_NEEDMOREPARAMS(str[0]));
		return 1;
	}
	if (user->addNewChannel() == false)
	{
		user->addMsgToSend(ERR_TOOMANYCHANNELS(user->getNick(), str[1]));
		return 1;
	}
	Channel	*cha = server->findChannel(str[1]);
	if (cha == NULL)
	{
		if (cha->isValidName(str[1]) == false)
		{
			user->addMsgToSend(ERR_BADCHANMASK(str[1]));
			return 1;
		}
		server->addChannel(str[1]);
		cha = server->findChannel(str[1]);
		std::cout << "--creation of the channel-- " << cha->getName() << std::endl;
		cha->setOperators(user, true);
		std::cout << user->getNick() << " is IRC operator in this channel" << std::endl;
	}
	if (cha->getPw() != "" && str.size() < 3)
	{
		user->addMsgToSend(ERR_BADCHANNELKEY(str[1]));
		return 1;
	}
	if (cha->getLimit() == cha->getUserCount())
	{
		user->addMsgToSend(ERR_CHANNELISFULL(user->getNick(), str[1]));
		return 1;
	}
	if (cha->isInvitOnly() == true && cha->isInvited(user) == false)
	{
		user->addMsgToSend(ERR_INVITEONLYCHAN(user->getNick(), str[1]));
		return 1;
	}
	else if (cha->getPw() == "" || (cha->getPw().compare(str[2]) == 0))
	{
		if (cha->isInChannel(user) == false)
		{
			std::cout << "add " << user->getNick() << " in this channel" << std::endl;
			cha->addUser(user);
		}
		std::vector<User *> listUser = cha->getUsers();
		for (std::vector<User*>::iterator it = listUser.begin(); it != listUser.end(); it++)
			(*it)->addMsgToSend(JOIN(user->getNick(), user->getUser(), user->getHost(), cha->getName()));
	}
	return 0;
}
