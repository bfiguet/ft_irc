/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmdJoin.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aalkhiro <aalkhiro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/16 15:55:03 by bfiguet           #+#    #+#             */
/*   Updated: 2024/02/23 10:08:05 by aalkhiro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Irc.hpp"

//Command: JOIN <channel>,<channels> <key>,<key>
int	cmdJoin(Server *server, std::vector<std::string> str, User *user)
{
	std::cout << "--cmdJoin--" << std::endl;
	size_t	i_pw = 0;
	size_t	end_cha = 0;
	size_t	end_pw = 0;

	if (str.size() < 2)
	{
		user->addMsgToSend(ERR_NEEDMOREPARAMS(str[0]));
		return 1;
	}
	if (user->canAddNewChannel() == false)
	{
		user->addMsgToSend(ERR_TOOMANYCHANNELS(user->getNick(), str[1]));
		return 1;
	}
	for (size_t i = 0; i < str[1].size(); i++)
	{
		if (str[1][i] == '#')
		{
			std::string	channel = "";
			while (str[1][end_cha] && str[1][end_cha] != ',')
				end_cha++;
			channel = str[1].substr(i, end_cha);
			Channel	*cha = server->findChannel(channel);
			if (cha == NULL)
			{
				if (cha->isValidName(channel) == false)
				{
					user->addMsgToSend(ERR_BADCHANMASK(channel));
					return 1;
				}
				server->addChannel(channel);
				cha = server->findChannel(channel);
				std::cout << "--creation of the channel-- " << cha->getName() << std::endl;
				cha->setOperators(user, true);
				std::cout << user->getNick() << " is IRC operator in this channel" << std::endl;
			}
			if (cha->getPw() != "" && str.size() < 3)
			{
				user->addMsgToSend(ERR_BADCHANNELKEY(channel));
				return 1;
			}
			if (cha->getLimit() == cha->getUserCount())
			{
				user->addMsgToSend(ERR_CHANNELISFULL(user->getNick(), channel));
				return 1;
			}
			if (cha->isInvitOnly() == true && cha->isInvited(user) == false)
			{
				user->addMsgToSend(ERR_INVITEONLYCHAN(user->getNick(), channel));
				return 1;
			}
			else if (cha->getPw() == "")
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
			else if (str.size() > 2)
			{
				std::string pw = "";
				while (str[2][end_pw] && str[2][end_pw] != ',')
					end_pw++;
				pw = str[2].substr(i_pw, end_pw);
				if (cha->getPw().compare(pw) == 0)
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
				if (str[2][end_pw] == ',')
					end_pw++;
				i_pw = end_pw;
			}
			user->addNewChannel();
			i = end_cha;
			if (str[1][end_cha] == ',')
				end_cha++;
		}
	}
	return 0;
}
