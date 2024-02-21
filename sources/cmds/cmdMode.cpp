/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmdMode.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aalkhiro <aalkhiro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/16 15:51:16 by bfiguet           #+#    #+#             */
/*   Updated: 2024/02/21 15:12:01 by aalkhiro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Irc.hpp"

//Command: MODE <channel> [<modestring> [<mode arguments>...]]
int	cmdMode(Server *server, std::vector<std::string> str, User *user){
	std::cout << "--cmdMode--" << std::endl;
	std::string			comment = "";

	if (str.size() < 2)
	{
		user->addMsgToSend(ERR_NEEDMOREPARAMS(str[0]));
		return 1;
	}
	if (str[1][0] == '#' && str.size() >= 3)
	{
		Channel				*cha = server->findChannel(str[1]);
		std::vector<User*>	listUser = cha->getUsers();
		if (cha == NULL)
		{
			user->addMsgToSend(ERR_NOSUCHCHANNEL(cha->getName()));
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
		if (str[2].empty() == false && str[2][0] == '+')
		{
			// +i : invite only, a user must be invited before they can join the channel
			if (str[2][1] == 'i')
			{
				cha->setInvitOnly(true);
				comment = "is now invite-only.";
			}
			// +t : topic protection, Only ChannelOperator can change topic
			else if (str[2][1] == 't')
			{
				
				cha->setTopicChange(true);
				comment = "topic is now protected.";
			}
			// +k : key protect, set password on channel
			else if (str[2][1] == 'k')
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
				else
				{
					cha->setPw(str[3]);
					comment = "is now locked.";
				}
			}
			// +l : limits max number of users in a channel
			else if (str[2][1] == 'l')
			{
				int nb = 0;
				if (str.size() < 4)
				{
					user->addMsgToSend(ERR_NEEDMOREPARAMS(str[0]));
					return 1;
				}
				nb = atoi(str[3].c_str());
				if (nb < 1 && cha->getUserCount() >= nb)
					return 1;
				cha->setLimit(nb);
				comment = "is now limited to " + str[3] + " users.";
			}
			else
			{
				user->addMsgToSend(ERR_UNKNOWNMODE(str[2]));
				return 1;
			}
		}
		// +o : gives operator status to a user (ChannelOperator)
		else if (str[2][1] == 'o')
		{
			if (str.size() < 3)
			{
				user->addMsgToSend(ERR_NEEDMOREPARAMS(str[0]));
				return 1;
			}
			User *userOp = server->findUser(str[3]);
			if (userOp == NULL)
			{
				user->addMsgToSend(ERR_NOSUCHNICK(str[3]));
				return 1;
			}
			else if (cha->isInChannel(userOp) == false)
			{
				user->addMsgToSend(ERR_NOTONCHANNEL(cha->getName()));
				return 1;
			}
			if (str[2][0] == '+')
			{
				cha->setOperators(userOp, true);
				comment = "is now channel operator.";
			}
			else if (str[2][0] == '-')
			{
				cha->setOperators(userOp, false);
				comment = "is no longer operator.";
			}
		}
		else if (str[2][0] == '-')
		{
			if (str[2][1] == 'i')
			{
				cha->setInvitOnly(false);
				comment = "is no longer invite-only.";
			}
			else if (str[2][1] == 't')
			{
				cha->setTopicChange(false);
				comment = "topic is no longer protected.";
			}
			else if (str[2][1] == 'k')
			{
				cha->setPw("");
				comment = "is no longer locked.";
			}
			else if (str[2][1] == 'l')
			{
				cha->setLimit(100);
				comment = "is no longer limited in members.";
			}
			else
			{
				user->addMsgToSend(ERR_UNKNOWNMODE(str[2]));
				return 1;
			}
		}
		else
		{
			user->addMsgToSend(ERR_UNKNOWNMODE(str[2]));
			return 1;
		}
		for (std::vector<User*>::iterator it = listUser.begin(); it != listUser.end(); it++)
			(*it)->addMsgToSend(MODE(cha->getName(), str[2], comment));
	}
	return 0;
}
