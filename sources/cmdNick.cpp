/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmdNick.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bfiguet <bfiguet@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/25 18:17:57 by bfiguet           #+#    #+#             */
/*   Updated: 2024/01/30 11:40:43 by bfiguet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Irc.hpp"

int	Server::cmdNick(std::vector<std::string> str, User &user){
	std::cout << "--cmdNick--" << std::endl;
	if (str.size() < 2)
	{
		user.sendMsg("Error nickname: no valid nickname");
		return 1;
	}
	for (std::vector<User>::iterator it = this->_users.begin(); it != this->_users.end(); ++it)
	{
		if (it->getNickname() == str[1] && it->getFd() != user.getFd())
		{
			user.sendMsg("Error nickname is already in use");
			return 1;
		}
	}
	user.setNickname(str[1]);
	//std::cout << "nickname is " << user.getNickname() << std::endl;
	std::cout << user.getNickname() << " :Welcome in our ircsev" << std::endl;
	std::cout << user.getNickname() << " you are register" << std::endl;
	return 0;
}

int	Server::cmdPw(std::vector<std::string> str, User &user){
	std::cout << "--cmdPw-- User: " << user.getNickname() << " str=" << str[1] << std::endl;
	std::string	pw = str[1];
	if (pw != _pw)
	{
		user.sendMsg("Error passeword: incorrect password");
		return 1;
	}
	if (str.size() < 2)
	{
		user.sendMsg("Error password: need more arguments");
		return 1;
	}
	std::cout << "password ok" << std::endl;
	return 0;
}

int	Server::cmdUser(std::vector<std::string> str, User &user){
	//std::cout << "--cmdUser--: " << std::endl;
	std::string	tmp;

	if (user.getUsername() == str.at(1))
	{
		user.sendMsg("Error user: already registered");
		return 1;
	}
	else if (str.size() < 4)
	{
		user.sendMsg("Error: need more arguments");
		return 1;
	}
	else if (str.size() >= 4)
	{
		user.setUsername(str.at(1));
		if (str.at(4)[0] == ':')
			tmp = str.at(4).substr(1);
		if (str.size() == 4)
		{
			user.setRealname(tmp);
			return 0;
		}
		tmp += " ";
		tmp += str.at(5);
		user.setRealname(tmp);
		//std::cout << "realname is " << user.getRealname() << std::endl;
	}
	return 0;
}

//The PING command is sent by either clients or servers to check the other side of the connection is still connected
//and/or to check for connection latency, at the application layer.
int	Server::cmdPing(std::vector<std::string> str, User &user){
	std::cout << "--cmdPing--" << std::endl;
	if (str.size() < 2)
	{
		user.sendMsg("Error ping: need more arguments");
		return 1;
	}
	std::cout << "cmd ping ok" << std::endl;
	user.sendMsg(str[1]);
	return 0;
}

int	Server::cmdJoin(std::vector<std::string> str, User &user){
	std::cout << "--cmdJoin-- User: " << user.getNickname() << " str=" << str[1] << std::endl;
	
	//ICI////////////
	
	return 0;
}

//The PART command removes the client from the given channel(s).
int	Server::cmdPart(std::vector<std::string> str, User &user){
	std::cout << "--cmdPart--" << std::endl;
	if (str.size() < 2)
	{
		user.sendMsg("Error Part: need more arguments");
		return 1;
	}
	try
	{
		std::vector<Channel>::iterator	cha = findChannelI(str[1]);
		if (cha->getFd() == user.getFd())
			cha->setFd(0);
		cha->delUser(user);
		if (cha->getUsers().empty())
			_channels.erase(cha);
			std::cout << "cmd part ok" << std::endl;
	}
	catch(const std::exception& e)
	{
		user.sendMsg("Error Part: no such channel");
	}
	return 0;
}

int	Server::cmdMode(std::vector<std::string> str, User &user){
	std::cout << "--cmdMode-- User: " << user.getNickname() << " str=" << str[1] << std::endl;
	return 0;
}

int	Server::cmdKick(std::vector<std::string> str, User &user){
	std::cout << "--cmdKick-- User: " << user.getNickname() << " str=" << str[1] << std::endl;
	return 0;
}

int	Server::cmdTopic(std::vector<std::string> str, User &user){
	std::cout << "--cmdTopic-- User: " << user.getNickname() << " str=" << str[1] << std::endl;
	return 0;
}

int	Server::cmdKill(std::vector<std::string> str, User &user){
	std::cout << "--cmdKill--" << std::endl;
	
	if (str.size() < 2)
	{
		user.sendMsg("Error kill: need more arguments");
		return 1;
	}
	for (size_t i = 0; i < _users.size(); i++)
	{
		if (str.at(1) == _users.at(i).getNickname())
		{
			delUser(_users.at(i).getFd());
			std::cout << "user is kill" << std::endl;
			return 0;
		}
	}
	user.sendMsg("Error kill: no such user");
	return 1;
}
