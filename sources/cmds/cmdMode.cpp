/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmdMode.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bfiguet <bfiguet@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/16 15:51:16 by bfiguet           #+#    #+#             */
/*   Updated: 2024/02/28 11:28:27 by bfiguet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Irc.hpp"

int	errKeyProtect(Channel* cha, User* user, std::vector<std::string> str)
{
	if (str.size() < 4)
	{
		user->addMsgToSend(ERR_NEEDMOREPARAMS(str[0]));
		return 1;
	}
	else if (!cha->getPw().empty())
	{
		user->addMsgToSend(ERR_KEYSET(cha->getName()));
		return 1;
	}
	else if (str[3].size() < 2 || str[3].size() > 8)
	{
		user->addMsgToSend(ERR_INVALIDKEY(str[1]));
		return 1;
	}
	return 0;
}

int	errOperator(Server* server, Channel* cha, User* user, std::vector<std::string> str)
{
	User* userOp = NULL;
	if (str.size() < 4)
	{
		user->addMsgToSend(ERR_NEEDMOREPARAMS(str[0]));
		return 1;
	}
	userOp = server->findUser(str[3]);
	if (userOp == NULL)
	{
		user->addMsgToSend(ERR_NOSUCHNICK(str[3]));
		return 1;
	}
	if (cha->isInChannel(userOp) == false)
	{
		user->addMsgToSend(ERR_NOTONCHANNEL(cha->getName()));
		return 1;
	}
	return 0;
}

int	errChannel(Channel* cha, User* user, std::vector<std::string> str)
{
	if (cha == NULL)
	{
		user->addMsgToSend(ERR_NOSUCHCHANNEL(str[1]));
		return 1;
	}
	else if (cha->isInChannel(user) == false)
	{
		user->addMsgToSend(ERR_NOTONCHANNEL(cha->getName()));
		return 1;
	}
	else if (cha->isOperator(user) == false)
	{
		user->addMsgToSend(ERR_NOPRIVILEGES);
		return 1;
	}
	if (str[2].empty() || (str[2][0] != '+' && str[2][0] != '-'))
		return 1;
	return 0;
}

//Command: MODE <channel> [<modestring> [<mode arguments>...]]
int	cmdMode(Server *server, std::vector<std::string> str, User *user){
	std::string			comment = "";
	User*				userOp = NULL;

	if (str.size() < 2)
	{
		user->addMsgToSend(ERR_NEEDMOREPARAMS(str[0]));
		return 1;
	}
	if (str[1][0] == '#' && str.size() >= 3)
	{
		Channel				*cha = server->findChannel(str[1]);		
		if (errChannel(cha, user, str) == 1)
			return 1;
		else
		{
			bool	oper = (str[2][0] == '+');
			switch (str[2][1])
			{
				// +i : invite only, a user must be invited before they can join the channel
				case 'i':
					if (oper == true)
						cha->setInvitOnly(true);
					else
						cha->setInvitOnly(false);
					break;
				// +k : key protect, set password on channel
				case 'k':
					if (oper == true)
					{
						if (errKeyProtect(cha, user, str) == 1)
							break;
						cha->setPw(str[3]);
						comment = str[3];
					}
					else
						cha->setPw("");
					break;
				// +l : limits max number of users in a channel
				case 'l':
					if (oper == true)
					{
						int nb = 0;
						if (str.size() < 4)
						{
							user->addMsgToSend(ERR_NEEDMOREPARAMS(str[0]));
							return 1;
						}
						nb = atoi(str[3].c_str());
						if (nb < 1 || cha->getUserCount() > nb)
							return 1;
						cha->setIsLimited(nb, true);
						comment = str[3];
					}
					else
						cha->setIsLimited(100, false);
					break;
				// +o : gives operator status to a user (ChannelOperator)
				case 'o':
					if (errOperator(server, cha, user, str) == 1)
						break;
					userOp = server->findUser(str[3]);
					if (oper == true)
						cha->setOperators(userOp, true);
					else
						cha->setOperators(userOp, false);
					comment = userOp->getNick();
					break;
				// +t : topic protection, Only ChannelOperator can change topic
				case 't':
					if (oper == true)
						cha->setTopicChange(true);
					else
						cha->setTopicChange(false);
					break;
				default:
				{
					user->addMsgToSend(ERR_UNKNOWNMODE(str[2]));
					break;
				}
			}
			std::vector<User *> listUser = cha->getUsers();
			for (std::vector<User*>::iterator it = listUser.begin(); it != listUser.end(); it++)
				(*it)->addMsgToSend(MODE(user->getNick(), cha->getName(), str[2], comment));
		}
	}
	return 0;
}
