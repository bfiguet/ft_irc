/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmds.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bfiguet <bfiguet@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/25 18:17:57 by bfiguet           #+#    #+#             */
/*   Updated: 2024/02/14 14:27:05 by bfiguet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Irc.hpp"

//alphanimeric {} [] \ | ok
// no space 
// no : and no num at first
bool	checkNick(std::string str){
	
	int	i = 0;
	if (str[i] == ':' || str[i] == '#' || isdigit(str[i]))
		return false;
	while (str[i])
	{
		if (isalnum(str[i]) || str[i] == '{' || str[i] == '}' || str[i] == '['
			|| str[i] == ']' || str[i] == '|' || str[i] == '\\')
			i++;
		else
			return false;
	}
	return true;
}

//Command: NICK <nickname>
int	cmdNick(Server *server, std::vector<std::string> str, User *user){
	std::vector<User*>	listUser = server->getUsers();

	if (str.size() < 2)
	{
		user->addMsgToSend(ERR_NONICKNAMEGIVEN);
		return 1;
	}
	if (std::find(listUser.begin(), listUser.end(), user) != listUser.end() == false)
	{
		user->addMsgToSend(ERR_NICKNAMEINUSE(user->getNick()));
		return 1;
	}
	if (checkNick(str[1]) == false)
	{
		user->addMsgToSend(ERR_ERRONEUSNICKNAME(user->getNick()));
			return 1;
	}
	if (user->getNick() == "")
	{
		user->sendMsg(NICK(str[1]));
	}
	else
		user->addMsgToSend(NICK_CHANGE(user->getNick(), str[1]));
	user->setNick(str[1]);
	return 0;
}

//Command: PASS <password>
int	cmdPass(Server *server, std::vector<std::string> str, User *user){
	
	if (str.size() < 2)
	{
		user->addMsgToSend(ERR_NEEDMOREPARAMS(str[0]));
		return 1;
	}
	if (str[1] != server->getPw())
	{
		user->addMsgToSend(ERR_PASSWDMISMATCH);
		return 1;
	}
	user->setPass(str[1]);
	//std::cout << "Password is good" << std::endl;
	return 0;
}

//Command: USER <username> 0 * <realname>
int	cmdUser(Server *server, std::vector<std::string> str, User *user){
	(void)server;
	std::string	tmp;

	if (user->getUser() == str.at(1))
	{
		user->addMsgToSend(ERR_ALREADYREGISTERED);
		return 1;
	}
	else if (str.size() < 4)
	{
		user->addMsgToSend(ERR_NEEDMOREPARAMS(str[0]));
		return 1;
	}
	else if (str.size() >= 4)
	{
		user->setUser(str[1]);
		user->setHost(str[3]);
		if (str.at(4)[0] == ':')
		{
			tmp = str.at(4).substr(1);
			user->setRealname(tmp);
		}
		 tmp += " ";
		 tmp += str.at(5);
		 user->setRealname(tmp);
	}
	return 0;
}

//Command: PING <token>
int	cmdPing(Server *server, std::vector<std::string> str, User *user){
	(void)server;

	if (str[1].size() == 0)
	{
		user->addMsgToSend(ERR_NOORIGIN(user->getNick()));
		return 1;
	}
	user->addMsgToSend(PONG(user->getHost(), str[1]));
	return 0;
}

// Command: INVITE <nickname> <channel>
int	cmdInvite(Server *server, std::vector<std::string> str, User *user){
	std::cout << "--cmdInvite--" << std::endl;
	Channel	*cha = server->findChannel(str[2]);
	User	*userNew = server->findUser(str[1]);

	if (str.size() < 3)
	{
		user->addMsgToSend(ERR_NEEDMOREPARAMS(str[0]));
		return 1;
	}
	if (cha == NULL)
	{
		user->addMsgToSend(ERR_NOSUCHCHANNEL(str[2]));
		return 1;
	}
	if (cha->isInChannel(user) == false)
	{
		user->addMsgToSend(ERR_NOTONCHANNEL(cha->getName()));
		return 1;
	}
	if (cha->isInvited(user) == true)
	{
		user->addMsgToSend(ERR_CHANOPRIVSNEEDED(cha->getName(), user->getNick()));
		return 1;
	}
	if (server->findUser(userNew->getNick()) == NULL)
	{
		user->addMsgToSend(ERR_NOSUCHNICK(userNew->getNick()));
		return 1;
	}
	if (cha->isInChannel(userNew) == true)
	{
		user->addMsgToSend(ERR_USERONCHANNEL(user->getNick(), userNew->getNick(), cha->getName()));
		return 1;
	}
	cha->addUser(userNew);
	cha->inviteUser(userNew);
	user->addMsgToSend(RPL_INVITING(user->getNick(), user->getUser(), server->getHost(), userNew->getNick(), cha->getName()));
	userNew->addMsgToSend(INVITE(user->getNick(), user->getUser(), server->getHost(), userNew->getNick(), cha->getName()));
	return 0;
}

//Command: PART <channel> <reason>
int	cmdPart(Server *server, std::vector<std::string> str, User *user){
	std::cout << "--cmdPart--" << std::endl;
	std::string	reason = "";

	if (str.size() < 2)
	{
		user->addMsgToSend(ERR_NEEDMOREPARAMS(str[0]));
		return 1;
	}
	for (size_t	i = 0; i < str.size(); ++i)
	{
		if (server->findChannel(str[i]) == NULL)
		{
			user->addMsgToSend(ERR_NOSUCHCHANNEL(str[i]));
			return 1;
		}
		if (server->findChannel(str[i])->isInChannel(user) == false)
		{
			user->addMsgToSend(ERR_NOTONCHANNEL(str[i]));
			return 1;
		}
		if (str[2].size() > 0)
			reason = str[2];
		user->addMsgToSend(PART(user->getNick(), user->getUser(), user->getHost(), str[1], reason));
		server->findChannel(str[i])->delUser(user);
	}
	return 0;
}

//Command: MODE <channel> [<modestring> [<mode arguments>...]]
//modesting (+ || -) && a-zA-Z
int	cmdMode(Server *server, std::vector<std::string> str, User *user){
	//std::cout << "--cmdMode--" << std::endl;
	//std::cout << "str[1]=" << str[1]<< " str[2]=" << str[2] << std::endl;
	std::string			comment = "";

	if (str.size() < 2)
	{
		user->addMsgToSend(ERR_NEEDMOREPARAMS(str[0]));
		return 1;
	}
	if (str[1][0] == '#' && str[2].empty() == false)
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
		if (str[2][0] == '+')
		{
			// +i : invite only, un utilisateur doit être invité avant de pouvoir rejoindre le channel.
			if (str[2][1] == 'i' || str[2][1] == 'I')
			{
				cha->setInvitOnly(true);
				comment = "is now invite-only.";
			}
			// +t : topic protection, seuls les ChannelOperator peuvent changer le topic.
			else if (str[2][1] == 't' || str[2][1] == 'T')
			{
				
				cha->setTopicChange(false);
				comment = "topic is now protected.";
			}
			// +k : key protect, place un mot de passe sur le channel. Les utilisateurs doivent indiquer ce mot de passe avec /JOIN #CHANNEL PASSWORD
			else if (str[2][1] == 'k' || str[2][1] == 'K')
			{
				if (str[3] == "")
				{
					user->addMsgToSend(ERR_NEEDMOREPARAMS(str[3]));
					return 1;
				}
				else if (!cha->getPw().empty())
				{
					user->addMsgToSend(ERR_KEYSET(cha->getName()));
					return 1;
				}
				else
				{
					cha->setPw(str[3]);
					comment = "is now locked.";
				}
			}
			// +l : limite le nombre maximal d'utilisateurs dans un channel
			else if (str[2][1] == 'l' || str[2][1] == 'L')
			{
				int nb = 0;
				if (str[3] == "")
				{
					user->addMsgToSend(ERR_NEEDMOREPARAMS(str[3]));
					return 1;
				}
				nb = atoi(str[3].c_str());
				if (nb < 1 && cha->getUserCount() >= nb)
				{
					//msg?
					return 1;
				}
				cha->setLimit(nb);
				comment = "is now limited to " + str[3] + " users.";
			}
			else
			{
				user->addMsgToSend(ERR_UNKNOWNMODE(str[2]));
				return 1;
			}
		}
		// +o : donne le status opérateur à un utilisateur (ChannelOperator)
		else if (str[2][1] == 'o' || str[2][1] == 'O')
		{
			User *userOp = server->findUser(str[3]);
			if (str[3] == "")
			{
				user->addMsgToSend(ERR_NEEDMOREPARAMS(str[3]));
				return 1;
			}
			else if (userOp == NULL)
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
			if (str[2][1] == 'i' || str[2][1] == 'I')
			{
				cha->setInvitOnly(false);
				comment = "is no longer invite-only.";
			}
			else if (str[2][1] == 't' || str[2][1] == 'T')
			{
				cha->setTopicChange(true);
				comment = "topic is no longer protected.";
			}
			else if (str[2][1] == 'k' || str[2][1] == 'K')
			{
				cha->setPw("");
				comment = "is no longer locked.";
			}
			else if (str[2][1] == 'l' || str[2][1] == 'L')
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
		user->addMsgToSend(ERR_NOSUCHNICK(userToDel->getNick()));
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
		reason = str[3];
	for (std::vector<User*>::iterator it = listUser.begin(); it != listUser.end(); it++)
		(*it)->addMsgToSend(KICK(user->getNick(), user->getUser(), user->getHost(), cha->getName(), userToDel->getNick(), reason));
	cha->delUser(userToDel);
	return 0;
}

//Command: TOPIC <channel> [<topic>]
int	cmdTopic(Server *server, std::vector<std::string> str, User *user){
	std::cout << "--cmdTopic--" << std::endl;

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
		if (str[2].size() < 1)
		{
			cha->setTopic("");
		}
		else if (cha->isTopicChange() == true && cha->isOperator(user) == true)
		{
			cha->setTopic(str[2]);
			std::vector<User*>	listUser = cha->getUsers();
			for (std::vector<User*>::iterator it = listUser.begin(); it != listUser.end(); it++)
				(*it)->addMsgToSend(TOPIC(user->getNick(), user->getUser(), server->getHost(), cha->getName(), str[2]));
		}
	}
	return 0;
}
//user->addMsgToSendToClient(RPL_TOPICWHOTIME((*i)->getNick(),str[1], user->getNick(), str[2], ??));

//Command: KILL <nickname> <comment>
int	cmdKill(Server *server, std::vector<std::string> str, User *user){
	std::cout << "--cmdKill--" << std::endl;
	std::string	reason = "";

	if (str.size() < 2)
	{
		user->addMsgToSend(ERR_NEEDMOREPARAMS(str[0]));
		return 1;
	}
	if (server->findUser(user->getFd()) == NULL)
	{
		user->addMsgToSend(ERR_NOPRIVILEGES);
		return 1;
	}
	else if (server->findUser(str[1]) != NULL)
	{
		if (str[2].size() > 0)
			reason = str[2];
		server->delUser(server->findUser(str[1]));
		user->addMsgToSend(KILL(user->getNick(), user->getUser(), user->getHost(), str[1], reason));
		return 0;
	}
	user->addMsgToSend(ERR_NOSUCHSERVER);
	return 1;
}

//Command: QUIT
int	cmdQuit(Server *server, std::vector<std::string> str, User *user){
	//std::cout << "--cmdQuit--" << std::endl;
	(void) str;
	std::cout << user->getNick() << " on fd " << user->getFd() << " has leaving" << std::endl;
	server->delUser(user);
	return 0;
}

//Command: JOIN <channel> <key>
int	cmdJoin(Server *server, std::vector<std::string> str, User *user){
	std::cout << "--cmdJoin--" << std::endl;

	if (str.size() < 2)
	{
		user->addMsgToSend(ERR_NEEDMOREPARAMS(str[0]));
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
		cha->isOperator(user);
		std::cout << user->getNick() << " is IRC operator in this channel" << std::endl;
	}
	if (cha->isInChannel(user) == false)
	{
		std::cout << "add " << user->getNick() << " in this channel" << std::endl;
		cha->addUser(user);
	}
	std::vector<User *> listUser = cha->getUsers();
	for (std::vector<User*>::iterator it = listUser.begin(); it != listUser.end(); it++)
		(*it)->addMsgToSend(JOIN(user->getNick(), user->getUser(), user->getHost(), cha->getName()));
	return 0;
}

//Command: PRIVMSG <target> <text to be sent>
int	cmdPrivmsg(Server *server, std::vector<std::string> str, User *user){
	std::cout << "--cmdPrivmsg--" << std::endl;
	std::cout << "nick/channel= " << str[1] << std::endl;
	std::cout << "msg= " << str[2] << std::endl;
	std::string	msg;
	std::vector<std::string>::iterator cpy;
	User* dest = server->findUser(str[1]);
	Channel	*cha = server->findChannel(str[1]);

	if (str.size() < 3)
	{
		user->addMsgToSend(ERR_NEEDMOREPARAMS(str[0]));
		return 1;
	}
	for (std::vector<std::string>::iterator it = str.begin() + 2; it != str.end(); ++it)
	{
		msg += *it;
		cpy = it;
		if (++cpy != str.end())
			msg += " ";
	}
	if (str[1][0] != '#' && str[1][0] != '&') //tagret == user
	{
		if (dest)
			dest->sendMsg(PRIVMSG(user->getNick(), user->getUser(), user->getHost(), dest->getUser(), msg));
		else
		{
			user->addMsgToSend(ERR_NOSUCHNICK(dest->getNick()));
			return 1;
		}
	}
	else // target == channel
	{
		if (cha)
		{
			if (cha->isInChannel(user) == false)
			{
				user->addMsgToSend(ERR_CANNOTSENDTOCHAN(user->getNick(), cha->getName()));
				return 1;
			}

			std::vector<User *> listUser = cha->getUsers();
			for (std::vector<User*>::iterator it = listUser.begin(); it != listUser.end(); it++)
			{
				if ((*it)->getNick() == user->getNick())
					continue;
				(*it)->addMsgToSend(RPL_PRIVMSG_CHANEL(user->getNick(), user->getUser(), str[0], cha->getName(), str[2]));
			}
		}
		else
		{
			user->addMsgToSend(ERR_NOSUCHCHANNEL(str[1]));
			return 1;
		}
	}
	return 0;
}
