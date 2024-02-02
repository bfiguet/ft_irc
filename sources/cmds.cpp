/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmds.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bfiguet <bfiguet@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/25 18:17:57 by bfiguet           #+#    #+#             */
/*   Updated: 2024/02/02 19:36:21 by bfiguet          ###   ########.fr       */
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

int	Server::cmdNick(std::vector<std::string> str, User *user){
	//std::cout << "--cmdNick--" << std::endl;
	if (str.size() < 2)
	{
		user->sendMsg(ERR_NONICKNAMEGIVEN);
		return 1;
	}
	for (std::vector<User*>::iterator it = _users.begin(); it != _users.end(); ++it)
	{
		if ((*it)->getNickname() == str[1] && (*it)->getFd() != user->getFd())
		{
			user->sendMsg(ERR_NICKNAMEINUSE(user->getNickname()));
			return 1;
		}
	}
	if (checkNick(str[1]) == false)
	{
		user->sendMsg(ERR_ERRONEUSNICKNAME(user->getNickname()));
			return 1;
	}
	user->setNickname(str[1]);
	return 0;
}

int	Server::cmdPass(std::vector<std::string> str, User *user){
	//std::cout << "--cmdPw--" << std::endl;
	if (str.size() < 2)
	{
		user->sendMsg(ERR_NEEDMOREPARAMS(str[0]));
		return 1;
	}
	if (str[1].size() - 1 == '\r')
	{
		if (str[1].substr(0, (str.size() - 1)) != _pw)
		{
			user->sendMsg(ERR_PASSWDMISMATCH);
			return 1;
		}
	}
	return 0;
}

int	Server::cmdUser(std::vector<std::string> str, User *user){
	//std::cout << "--cmdUser--" << std::endl;
	std::string	tmp;

	if (user->getUsername() == str.at(1))
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
		user->setUsername(str.at(1));
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

int	Server::cmdPing(std::vector<std::string> str, User *user){
	if (str.size() < 2)
	{
		user->sendMsg(ERR_NEEDMOREPARAMS(str[0]));
		return 1;
	}
	if (str[1].size() <= 0)
	{
		user->sendMsg(ERR_NOORIGIN(user->getNickname()));
		return 1;
	}
	user->sendMsg("Pong " + user->getNickname() + " :" + str[1]);
	return 0;
}

int	Server::cmdJoin(std::vector<std::string> str, User *user){
	//std::cout << "--cmdJoin--" << std::endl;
	std::cout << "--cmdJoin-- User: " << user->getNickname() << " str=" << str[1] << std::endl;
	return 0;
}

int	Server::cmdPart(std::vector<std::string> str, User *user){
	//std::cout << "--cmdPart--" << std::endl;
	
	if (str.size() < 2)
	{
		user->sendMsg(ERR_NEEDMOREPARAMS(str[0]));
		return 1;
	}
	for (size_t	i = 1; i < str.size(); i++)
	{
		if (isChannel(str[i]) == false)
		{
			user->sendMsg(ERR_NOSUCHCHANNEL(str[i]));
			return 1;
		}
		if (findChannel(str[i])->isInChannel(user) == false)
		{
			user->sendMsg(ERR_NOTONCHANNEL(str[i]));
			return 1;
		}
		findChannel(str[i])->delUser(user);
	}
	return 0;
}

int	Server::cmdMode(std::vector<std::string> str, User *user){
	std::cout << "--cmdMode-- User: " << user->getNickname() << " str=" << str[1] << std::endl;
	return 0;
}

int	Server::cmdKick(std::vector<std::string> str, User *user){
	std::cout << "--cmdKick-- User: " << user->getNickname() << " str=" << str[1] << std::endl;
	return 0;
}

int	Server::cmdTopic(std::vector<std::string> str, User *user){
	std::cout << "--cmdTopic--" << user->getNickname() << " str=" << str[1] << std::endl;
	return 0;
}

int	Server::cmdKill(std::vector<std::string> str, User *user){
	//std::cout << "--cmdKill--" << std::endl;
	if (str.size() < 2)
	{
		user->sendMsg(ERR_NEEDMOREPARAMS(str[0]));
		return 1;
	}
	if (findUser(user->getFd()) == NULL)
	{
		user->sendMsg(ERR_NOPRIVILEGES);
		return 1;
	}
	else
	{
		deleteUserFromChannels(user);
		delUser(user);
		return 0;
	}
	user->sendMsg(ERR_NOSUCHSERVER);
	return 1;
}
