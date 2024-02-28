/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmdJoin.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aalkhiro <aalkhiro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/16 15:55:03 by bfiguet           #+#    #+#             */
/*   Updated: 2024/02/28 11:37:57 by aalkhiro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Irc.hpp"

std::vector<std::string>	ft_split(std::string str, char delimiter)
{
	std::string					temp;
	std::vector<std::string>	strs;

	while (!str.empty())
	{
		temp = str.substr(0, str.find(delimiter));
		strs.push_back(temp);
		if (temp.size() == str.size())
		    str = "";
		else
    		str = str.substr(temp.size() + 1, str.size() - temp.size());
	}
	return (strs);
}

void	cmdJoinPars(std::vector<std::string>& chanNames, std::vector<std::string>& passWords, std::string names, std::string pass)
{
	chanNames = ft_split(names, ',');
	if (!pass.empty())
	{
		passWords = ft_split(pass, ',');
		if (!pass.empty() && pass[pass.size() - 1] == ',')
			passWords.push_back("");
	}
}

Channel*	createChannel(Server* server, User* user, std::string name)
{
	Channel* cha = NULL;
	if (cha->isValidName(name) == false)
	{
		user->addMsgToSend(ERR_BADCHANMASK(name));
		return NULL;
	}
	server->addChannel(name);
	cha = server->findChannel(name);
	std::cout << "--creation of the channel-- " << cha->getName() << std::endl;
	cha->setOperators(user, true);
	std::cout << user->getNick() << " is IRC operator in this channel" << std::endl;
	return (cha);
}

int	joinChannel(Server* server, std::string chaName, User* user, std::string passWord)
{
	Channel	*cha = server->findChannel(chaName);
	if (cha == NULL)
	{
		cha = createChannel(server, user, chaName);
		if (cha == NULL)
			return (1);
	}
	if (cha->isInChannel(user))
	{
		return (0);
	}
	if (cha->getLimit() == cha->getUserCount())
	{
		user->addMsgToSend(ERR_CHANNELISFULL(user->getNick(), chaName));
		return 1;
	}
	if (cha->isInvitOnly() == true && cha->isInvited(user) == false)
	{
		user->addMsgToSend(ERR_INVITEONLYCHAN(user->getNick(), chaName));
		return 1;
	}
	if (cha->getPw().compare(passWord) == 0)
	{
		std::cout << "add " << user->getNick() << " in this channel" << std::endl;
		cha->addUser(user);
		cha->proadcast(JOIN(user->getNick(), user->getUser(), user->getHost(), cha->getName()));
		user->addMsgToSend(RPL_NAMREPLY(user->getNick(), user->getUser(), user->getHost(), cha->getName(), cha->listNames()));
		user->addMsgToSend(RPL_ENDOFNAMES(user->getNick(), user->getUser(), user->getHost(), cha->getName()));
		if (!cha->getTopic().empty())
						user->addMsgToSend(RPL_TOPIC(user->getNick(), user->getUser(), server->getHost(), cha->getName(), cha->getTopic()));
		cha->setInviteUser(user, false);
	}
	else
	{
		user->addMsgToSend(ERR_BADCHANNELKEY(user->getNick(), chaName));
		return 1;
	}
	return (0);
}

//Command: JOIN <channel>,<channels> <key>,<key>
int	cmdJoin(Server *server, std::vector<std::string> args, User *user)
{
	std::vector<std::string> chanNames;
	std::vector<std::string> passWords;
	std::string pass;

	if (args.size() < 2)
	{
		user->addMsgToSend(ERR_NEEDMOREPARAMS(args[0]));
		return 1;
	}
	if (user->canAddNewChannel() == false)
	{
		user->addMsgToSend(ERR_TOOMANYCHANNELS(user->getNick(), args[1]));
		return 1;
	}
	if (args.size() > 2)
		cmdJoinPars(chanNames, passWords, args[1], args[2]);
	else
		cmdJoinPars(chanNames, passWords, args[1], "");
	std::vector<std::string>::iterator ipass = passWords.begin();
	for (std::vector<std::string>::iterator iname = chanNames.begin(); iname != chanNames.end(); iname++)
	{
		if (ipass == passWords.end())
			pass = "";
		else
			pass = *ipass;
		if (joinChannel(server, *iname, user, pass))
			return (1);
		if (ipass != passWords.end())
			ipass++;
	}
	return 0;
}
