/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   User.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aalkhiro <aalkhiro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/12 11:46:33 by bfiguet           #+#    #+#             */
/*   Updated: 2024/02/08 15:43:23 by aalkhiro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Irc.hpp"

User::User(int fd): _sock(fd) { _msg = ""; }

User::~User() {}

int	User::getFd()const{	return _sock; }

std::string	User::getNick()const{ return _nick; }

std::string	User::getUser() const{ return _user; }

std::string	User::getRealname()const{	return _realname; }

//std::string	User::getHost()const {return _host;};

std::string	User::getPass()const {return _pass;};

std::string	User::getMsg()const{ return _msg; }

std::string	User::getMsgsToSend() const{return _msgsToSend;};



void		User::setNick(std::string str){ _nick = str; }

void		User::setUser(std::string str){ _user = str; }

void		User::setRealname(std::string str){ _realname = str; }

//void		User::setHost(std::string str){_host = str;};

void		User::setPass(std::string str){_pass = str;};

void		User::setMsg(std::string str){ _msg = str; }

void		User::addMsg(std::string str){ _msg += str; }

void		User::addMsgToSend(std::string str){ _msgsToSend += str; }

void		User::sendMsg(std::string msg){
	// std::cout << "---> " << msg << std::endl;
	if (send(_sock, msg.c_str(), msg.length(), 0) < 0)
	{
		std::cout << "Error: send to user " << strerror(errno) << std::endl;
	}
}

std::string	User::extractCmd(std::string cmds)
{
    std::string cmd;
	char const* cmdEnd = std::strstr(cmds.c_str(), "\r\n");
	if (cmdEnd == NULL)
		return ("");
	cmd = cmds.substr(0, cmdEnd - cmds.c_str());
	setMsg(cmds.substr(cmdEnd - cmds.c_str() + 2, cmds.size() - cmd.size()));
	std::cout << "debug: command extraction: |cmd|buffer|" << std::endl << "|" << cmd << "|" << cmds << "|" << std::endl;
	return (cmd);
}
