/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmds.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bfiguet <bfiguet@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/25 18:17:57 by bfiguet           #+#    #+#             */
/*   Updated: 2024/02/06 17:11:44 by bfiguet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Irc.hpp"

bool	checkNick(std::string str){
	//alphanimeric {} [] \ | ok
	// no space 
	// no : and no num at first
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

//The NICK command is used to give the client a nickname or change the previous one.
int	cmdNick(Server *server, std::vector<std::string> str, User *user){
	//std::cout << "--cmdNick--" << std::endl;
	if (str.size() < 2)
	{
		user->sendMsg(ERR_NONICKNAMEGIVEN);
		return 1;
	}
	std::vector<User*>	listUser = server->getUsers();
	for (int i = 0; listUser[i]; i++)
	{
		if (listUser[i]->getNick() == str[1] && listUser[i]->getFd() != user->getFd())
		{
			user->sendMsg(ERR_NICKNAMEINUSE(user->getNick()));
			return 1;
		}
	}
	if (checkNick(str[1]) == false)
	{
		user->sendMsg(ERR_ERRONEUSNICKNAME(user->getNick()));
			return 1;
	}
	user->setNick(str[1]);
	return 0;
}

//The PASS command is used to set a ‘connection password’.
int	cmdPass(Server *server, std::vector<std::string> str, User *user){
	//std::cout << "--cmdPw--" << std::endl;
	if (str.size() < 2)
	{
		user->sendMsg(ERR_NEEDMOREPARAMS(str[0]));
		return 1;
	}
	if (str[1].size() - 1 == '\r')
	{
		if (str[1].substr(0, (str.size() - 1)) != server->getPw())
		{
			user->sendMsg(ERR_PASSWDMISMATCH);
			return 1;
		}
	}
	return 0;
}

//The USER command is used at the beginning of a connection to specify the username and realname of a new user.
int	cmdUser(Server *server, std::vector<std::string> str, User *user){
	//std::cout << "--cmdUser--" << std::endl;
	(void)server;
	std::string	tmp;

	if (user->getUser() == str.at(1))
	{
		user->sendMsg(ERR_ALREADYREGISTERED);
		return 1;
	}
	else if (str.size() < 4)
	{
		user->sendMsg(ERR_NEEDMOREPARAMS(str[0]));
		return 1;
	}
	else if (str.size() >= 4)
	{
		user->setUser(str.at(1));
		if (str.at(4)[0] == ':')
			tmp = str.at(4).substr(1);
		if (str.size() == 4)
		{
			user->setRealname(tmp);
			return 0;
		}
		tmp += " ";
		tmp += str.at(5);
		user->setRealname(tmp);
	}
	return 0;
}

//The PING command is sent by either clients or servers to check the other side of the connection 
// is still connected and/or to check for connection latency, at the application layer.
int	cmdPing(Server *server, std::vector<std::string> str, User *user){
	(void)server;
	if (str.size() < 2)
	{
		user->sendMsg(ERR_NEEDMOREPARAMS(str[0]));
		return 1;
	}
	if (str[1].size() <= 0)
	{
		user->sendMsg(ERR_NOORIGIN(user->getNick()));
		return 1;
	}
	user->sendMsg("Pong " + user->getNick() + " :" + str[1]);
	return 0;
}

//The JOIN command indicates that the client wants to join the given channel(s)
// each channel using the given key for it.
int	cmdJoin(Server *server, std::vector<std::string> str, User *user){
	std::cout << "--cmdJoin--" << std::endl;
	(void)server;
	(void)user;
	(void)str;
	return 0;
}

//The PART command removes the client from the given channel(s). 
int	cmdPart(Server *server, std::vector<std::string> str, User *user){
	//std::cout << "--cmdPart--" << std::endl;
	if (str.size() < 2)
	{
		user->sendMsg(ERR_NEEDMOREPARAMS(str[0]));
		return 1;
	}
	for (size_t	i = 1; i < str.size(); i++)
	{
		if (server->isChannel(str[i]) == false)
		{
			user->sendMsg(ERR_NOSUCHCHANNEL(str[i]));
			return 1;
		}
		if (server->findChannel(str[i])->isInChannel(user) == false)
		{
			user->sendMsg(ERR_NOTONCHANNEL(str[i]));
			return 1;
		}
		//send msg all channel's user
		server->findChannel(str[i])->delUser(user);
	}
	return 0;
}

int	cmdMode(Server *server, std::vector<std::string> str, User *user){
	std::cout << "--cmdMode--" << std::endl;
	(void)server;
	(void)user;
	(void)str;
	return 0;
}

//The KICK command can be used to request the forced removal of a user from a channel.
//It causes the <user> to be removed from the <channel> by force.
int	cmdKick(Server *server, std::vector<std::string> str, User *user){
	std::cout << "--cmdKick--" << std::endl;
	if (str.size() < 3)
	{
		user->sendMsg(ERR_NEEDMOREPARAMS(str[0]));
		return 1;
	}
	Channel *cha = server->findChannel(str[1]);
	User	*userToDel = server->findUser(str[2]);
	if (server->findUser(str[2]) != userToDel)
	{
		user->sendMsg(ERR_NOSUCHNICK(userToDel->getNick()));
		return 1;
	}
	else if (cha->isInChannel(userToDel) == false)
	{
		user->sendMsg(ERR_USERNOTINCHANNEL(userToDel->getNick(), str[1]));
		return 1;
	}
	else if (cha->isInvited(user) == true)
	{
		user->sendMsg(ERR_CHANOPRIVSNEEDED(cha->getName(), user->getNick()));
		return 1;
	}
	cha->delUser(userToDel);
	return 0;
}

//The TOPIC command is used to change or view the topic of the given channel. 
int	cmdTopic(Server *server, std::vector<std::string> str, User *user){
	//std::cout << "--cmdTopic--" << std::endl;
	if (str.size() < 2)
	{
		user->sendMsg(ERR_NEEDMOREPARAMS(str[0]));
		return 1;
	}
	//check if the channel exist
	Channel	*cha = server->findChannel(str[1]);
	if (cha == NULL)
	{
		user->sendMsg(ERR_NOSUCHCHANNEL(str[1]));
		return 1;
	}
	if (str.size() == 2)
	{
		if (cha->getTopic() == "")
			user->sendMsg(RPL_NOTOPIC(user->getNick(), user->getUser(), server->getHost(), str[1]));
		else
			user->sendMsg(RPL_TOPIC(user->getNick(), user->getUser(), server->getHost(), str[1], cha->getTopic()));
	}
	else
	{
		if (cha->isInvitOnly())
		{
			if (cha->isInvited(user))
			{
				user->sendMsg(ERR_CHANOPRIVSNEEDED(str[1], user->getNick()));
				return 1;
			}
		}
	}
	cha->setTopic(str[2]);
	std::vector<User*>	listUser = cha->getUsers();
	for (int i = 0; listUser[i]; i++)
	{
		listUser[i]->sendMsg(TOPIC(user->getNick(), user->getUser(), server->getHost(), str[1], str[2]));
		return 1;
	}
	return 0;
}
//user->sendMsgToClient(RPL_TOPICWHOTIME((*i)->getNick(),str[1], user->getNick(), str[2], ??));

//The KILL command is used to close the connection between a given client and the server they are connected to.
//KILL is a privileged command and is available only to IRC Operators. 
int	cmdKill(Server *server, std::vector<std::string> str, User *user){
	//std::cout << "--cmdKill--" << std::endl;
	if (str.size() < 2)
	{
		user->sendMsg(ERR_NEEDMOREPARAMS(str[0]));
		return 1;
	}
	if (server->findUser(user->getFd()) == NULL)
	{
		user->sendMsg(ERR_NOPRIVILEGES);
		return 1;
	}
	else
	{
		server->deleteUserFromChannels(user);
		server->delUser(user);
		return 0;
	}
	user->sendMsg(ERR_NOSUCHSERVER);
	return 1;
}

//The QUIT command is used to terminate a client’s connection to the server. 
int	cmdQuit(Server *server, std::vector<std::string> str, User *user){
	//std::cout << "--cmdQuit--" << std::endl;
	(void) str;
	server->delUser(user);
	return 0;
}
