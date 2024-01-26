/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmdNick.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bfiguet <bfiguet@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/25 18:17:57 by bfiguet           #+#    #+#             */
/*   Updated: 2024/01/25 20:54:21 by bfiguet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Irc.hpp"

int	Server::cmdNick(std::vector<std::string> str, User &user){
	std::cout << "--cmdNick--" << std::endl;
	if (str.size() < 2)
	{
		user.sendMsg("error nickname: no valid nickname");
		return 1;
	}
	for (std::vector<User>::iterator it = this->_users.begin(); it != this->_users.end(); ++it)
	{
		if (it->getNickname() == str[1] && it->getFd() != user.getFd())
		{
			user.sendMsg("error nickname is already in use");
			return 1;
		}
	}
	user.setNickname(str[1]);
	//user.sendMsg(user.getNickname());
	std::cout << user.getNickname() << " :Welcome in our ircsev" << std::endl;
	std::cout << user.getNickname() << " you are register" << std::endl;
	return 0;
}

int	Server::cmdPw(std::vector<std::string> str, User &user){
	std::cout << "--cmdPw-- User: " << user.getNickname() << " str=" << str[1] << std::endl;
	return 0;
}

int	Server::cmdUser(std::vector<std::string> str, User &user){
	std::cout << "--cmdUser-- user: " << user.getNickname() << " str=" << str[1] << std::endl;
	return 0;
}

int	Server::cmdPing(std::vector<std::string> str, User &user){
	std::cout << "--cmdPing-- User: " << user.getNickname() << " str=" << str[1] << std::endl;
	return 0;
}

int	Server::cmdJoin(std::vector<std::string> str, User &user){
	std::cout << "--cmdJoin-- User: " << user.getNickname() << " str=" << str[1] << std::endl;
	return 0;
}

int	Server::cmdPart(std::vector<std::string> str, User &user){
	std::cout << "--cmdPart-- User: " << user.getNickname() << " str=" << str[1] << std::endl;
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
	std::cout << "--cmdKill-- User: " << user.getNickname() << " str=" << str[1] << std::endl;
	return 0;
}
