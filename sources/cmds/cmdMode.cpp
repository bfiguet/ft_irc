/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmdMode.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bfiguet <bfiguet@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/16 15:51:16 by bfiguet           #+#    #+#             */
/*   Updated: 2024/02/28 12:22:25 by bfiguet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Irc.hpp"

std::string	keyMode(Channel* cha, User* user, std::vector<std::string> str, bool oper)
{
	std::string comment = "";
	if (str.size() < 4)
	{
		user->addMsgToSend(ERR_NEEDMOREPARAMS(str[0]));
		return comment;
	}
	else if (!cha->getPw().empty())
	{
		user->addMsgToSend(ERR_KEYSET(cha->getName()));
		return comment;
	}
	else if (str[3].size() < 2 || str[3].size() > 8)
	{
		user->addMsgToSend(ERR_INVALIDKEY(str[1]));
		return comment;
	}
	if (oper == true)
	{
		cha->setPw(str[3]);
		comment = str[3];
	}
	else
		cha->setPw("");
	return comment;
}

std::string	operatorMode(Server* server, Channel* cha, User* user, std::vector<std::string> str, bool oper)
{
	User* userOp = NULL;
	std::string comment = "";
	if (str.size() < 4)
	{
		user->addMsgToSend(ERR_NEEDMOREPARAMS(str[0]));
		return comment;
	}
	userOp = server->findUser(str[3]);
	if (userOp == NULL)
	{
		user->addMsgToSend(ERR_NOSUCHNICK(str[3]));
		return comment;
	}
	if (cha->isInChannel(userOp) == false)
	{
		user->addMsgToSend(ERR_NOTONCHANNEL(cha->getName()));
		return comment;
	}
	userOp = server->findUser(str[3]);
	cha->setOperators(userOp, oper);
	comment = userOp->getNick();
	return comment;
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

std::string	limitMode(Channel* cha, User* user, bool oper, std::vector<std::string> str)
{
	std::string comment = "";
	if (oper == true)
	{
		int nb = 0;
		if (str.size() < 4)
		{
			user->addMsgToSend(ERR_NEEDMOREPARAMS(str[0]));
			return comment;
		}
		nb = atoi(str[3].c_str());
		if (nb < 1 || cha->getUserCount() > nb)
			return comment;
		cha->setIsLimited(nb, true);
		comment = str[3];
	}
	else
		cha->setIsLimited(100, false);
	return comment;
}

//Command: MODE <channel> [<modestring> [<mode arguments>...]]
int	cmdMode(Server *server, std::vector<std::string> str, User *user){
	std::string			comment = "";
	//User*				userOp = NULL;

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
						cha->setInvitOnly(oper);
					break;
				// +k : key protect, set password on channel
				case 'k':
					comment = keyMode(cha, user, str, oper);
					break;
				// +l : limits max number of users in a channel
				case 'l':
					comment = limitMode(cha, user, oper, str);
					break;
				// +o : gives operator status to a user (ChannelOperator)
				case 'o':
					comment = operatorMode(server, cha, user, str, oper);
					break;
				// +t : topic protection, Only ChannelOperator can change topic
				case 't':
					if (oper == true)
						cha->setTopicChange(oper);
					break;
				default:
				{
					user->addMsgToSend(ERR_UNKNOWNMODE(str[2]));
					return 0;
				}
			}
			cha->broadcast(MODE(user->getNick(), cha->getName(), str[2], comment));
		}
	}
	return 0;
}
