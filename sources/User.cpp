/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   User.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bfiguet <bfiguet@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/12 11:46:33 by bfiguet           #+#    #+#             */
/*   Updated: 2024/02/06 16:23:45 by bfiguet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Irc.hpp"

User::User(int fd): _sock(fd) { _msg = ""; }

User::~User() {}

int	User::getFd()const{	return _sock; }

std::string	User::getNick()const{ return _nick; }

std::string	User::getUser() const{ return _user; }

std::string	User::getRealname()const{	return _realname; }

std::string	User::getMsg()const{ return _msg; }

std::string	User::getMsgsToSend() const{return _msgsToSend;};

void		User::setNick(std::string str){ _nick = str; }

void		User::setUser(std::string str){ _user = str; }

void		User::setRealname(std::string str){ _realname = str; }

void		User::setMsg(std::string str){ _msg = str; }

void		User::addMsg(std::string str){ _msg += str; }

void		User::sendMsg(std::string msg){
	std::cout << "---> " << msg << "\r\n" << std::endl;
	if (send(_sock, msg.c_str(), msg.length(), 0) < 0)
	{
		std::cout << "error send user's msg" << std::endl;
	}
}
