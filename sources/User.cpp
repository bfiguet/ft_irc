/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   User.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aalkhiro <aalkhiro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/12 11:46:33 by bfiguet           #+#    #+#             */
/*   Updated: 2024/02/02 08:07:00 by aalkhiro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Irc.hpp"

User::User(int fd, std::string host): _sock(fd), _hostname(host) { _msg = ""; }

User::~User() {}

int	User::getFd()const{	return _sock; }

std::string	User::getNickname()const{ return _nickname; }

std::string	User::getUsername() const{ return _username; }

std::string	User::getRealname()const{	return _realname; }

std::string	User::getHostname()const{	return _hostname; }

std::string	User::getMsg()const{ return _msg; }

void		User::setNickname(std::string str){ _nickname = str; }

void		User::setUsername(std::string str){ _username = str; }

void		User::setRealname(std::string str){ _realname = str; }

void		User::setHostname(std::string str){ _hostname = str; }

void		User::setMsg(std::string str){ _msg = str; }

void		User::addMsg(std::string str){ _msg += str; }

void		User::sendMsg(std::string msg){
	std::cout << "---> " << msg << "\r\n" << std::endl;
	if (send(_sock, msg.c_str(), msg.length(), 0) < 0)
	{
		std::cout << "error send user's msg" << std::endl;
	}
}
