/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmdMode.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bfiguet <bfiguet@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/16 15:51:16 by bfiguet           #+#    #+#             */
/*   Updated: 2024/02/23 10:36:58 by bfiguet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Irc.hpp"

//Command: MODE <channel> [<modestring> [<mode arguments>...]]
int	cmdMode(Server *server, std::vector<std::string> str, User *user){
	//std::cout << "--cmdMode--" << std::endl;
	//std::cout << "str[1]" << str[1] << std::endl;
	//std::cout << "str[2]" << str[2] << std::endl;
	std::string			comment = "";
	User				*userOp = NULL;

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
		if (str[2].empty() == false && (str[2][0] == '+' || str[2][0] == '-'))
		{
			switch (str[2][1])
			{
				// +i : invite only, a user must be invited before they can join the channel
				case 'i':
					if (str[2][0] == '+')
					{
						cha->setInvitOnly(true);
						//comment = "is now invite-only.";
					}
					else if (str[2][0] == '-')
					{
						cha->setInvitOnly(false);
						//comment = "is no longer invite-only.";
					}
					break;
				// +k : key protect, set password on channel
				case 'k':
					if (str[2][0] == '+')
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
							//comment = "is now locked.";
							comment = str[3];
						}
					}
					else if (str[2][0] == '-')
					{
						cha->setPw("");
						//comment = "is no longer locked.";
					}
					break;
				// +l : limits max number of users in a channel
				case 'l':
					if (str[2][0] == '+')
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
						cha->setLimit(nb);
						//comment = "is now limited to " + str[3] + " users.";
						comment = str[3];
					}
					else if (str[2][0] == '-')
					{
						cha->setLimit(100);
						//comment = "is no longer limited in members.";
					}
					break;
				// +o : gives operator status to a user (ChannelOperator)
				case 'o':
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
					else if (cha->isInChannel(userOp) == false)
					{
						user->addMsgToSend(ERR_NOTONCHANNEL(cha->getName()));
						return 1;
					}
					else if (str[2][0] == '+')
					{
						cha->setOperators(userOp, true);
						//comment = "is now channel operator.";
						comment = userOp->getNick();
					}
					else if (str[2][0] == '-')
					{
						cha->setOperators(userOp, false);
						//comment = "is no longer operator.";
						comment = userOp->getNick();
					}
					break;
				// +t : topic protection, Only ChannelOperator can change topic
				case 't':
					if (str[2][0] == '+')
					{
						cha->setTopicChange(true);
						//comment = "topic is now protected.";
					}
					else if (str[2][0] == '-')
					{
						cha->setTopicChange(false);
						//comment = "topic is no longer protected.";
					}
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
