/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmdMode.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bfiguet <bfiguet@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/16 15:51:16 by bfiguet           #+#    #+#             */
/*   Updated: 2024/02/28 13:41:05 by bfiguet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Irc.hpp"

std::string	keyMode(Channel* cha, User* user, std::vector<std::string> args, bool oper)
{
	std::string comment = "";
	if (args.size() < 4)
	{
		user->addMsgToSend(ERR_NEEDMOREPARAMS(args[0]));
		return comment;
	}
	else if (!cha->getPw().empty())
	{
		user->addMsgToSend(ERR_KEYSET(cha->getName()));
		return comment;
	}
	else if (args[3].size() < 2 || args[3].size() > 8)
	{
		user->addMsgToSend(ERR_INVALIDKEY(args[1]));
		return comment;
	}
	if (oper == true)
	{
		cha->setPw(args[3]);
		comment = args[3];
	}
	else
		cha->setPw("");
	return comment;
}

std::string	operatorMode(Server* server, Channel* cha, User* user, std::vector<std::string> args, bool oper)
{
	User* userOp = NULL;
	std::string comment = "";
	if (args.size() < 4)
	{
		user->addMsgToSend(ERR_NEEDMOREPARAMS(args[0]));
		return comment;
	}
	userOp = server->findUser(args[3]);
	if (userOp == NULL)
	{
		user->addMsgToSend(ERR_NOSUCHNICK(args[3]));
		return comment;
	}
	if (cha->isInChannel(userOp) == false)
	{
		user->addMsgToSend(ERR_NOTONCHANNEL(cha->getName()));
		return comment;
	}
	userOp = server->findUser(args[3]);
	cha->setOperators(userOp, oper);
	comment = userOp->getNick();
	return comment;
}

int	errChannel(Channel* cha, User* user, std::vector<std::string> args)
{
	if (cha == NULL)
	{
		user->addMsgToSend(ERR_NOSUCHCHANNEL(args[1]));
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
	if (args[2].empty() || (args[2][0] != '+' && args[2][0] != '-'))
		return 1;
	return 0;
}

std::string	limitMode(Channel* cha, User* user, bool oper, std::vector<std::string> args)
{
	std::string comment = "";
	if (oper == true)
	{
		int nb = 0;
		if (args.size() < 4)
		{
			user->addMsgToSend(ERR_NEEDMOREPARAMS(args[0]));
			return comment;
		}
		nb = atoi(args[3].c_str());
		if (nb < 1 || cha->getUserCount() > nb)
			return comment;
		cha->setIsLimited(nb, true);
		comment = args[3];
	}
	else
		cha->setIsLimited(100, false);
	return comment;
}

//Command: MODE <channel> [<modestring> [<mode arguments>...]]
int	cmdMode(Server *server, std::vector<std::string> args, User *user){
	std::string			comment = "";
	//User*				userOp = NULL;

	if (args.size() < 2)
	{
		user->addMsgToSend(ERR_NEEDMOREPARAMS(args[0]));
		return 1;
	}
	if (args[1][0] == '#' && args.size() >= 3)
	{
		Channel				*cha = server->findChannel(args[1]);		
		if (errChannel(cha, user, args) == 1)
			return 1;
		else
		{
			bool	oper = (args[2][0] == '+');
			switch (args[2][1])
			{
				// +i : invite only, a user must be invited before they can join the channel
				case 'i':
						cha->setInvitOnly(oper);
					break;
				// +k : key protect, set password on channel
				case 'k':
					comment = keyMode(cha, user, args, oper);
					break;
				// +l : limits max number of users in a channel
				case 'l':
					comment = limitMode(cha, user, oper, args);
					break;
				// +o : gives operator status to a user (ChannelOperator)
				case 'o':
					comment = operatorMode(server, cha, user, args, oper);
					break;
				// +t : topic protection, Only ChannelOperator can change topic
				case 't':
					if (oper == true)
						cha->setTopicChange(oper);
					break;
				default:
				{
					user->addMsgToSend(ERR_UNKNOWNMODE(args[2]));
					return 0;
				}
			}
			cha->broadcast(MODE(user->getNick(), cha->getName(), args[2], comment));
		}
	}
	return 0;
}
