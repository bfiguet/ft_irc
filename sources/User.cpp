/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   User.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aalkhiro <aalkhiro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/12 11:46:33 by bfiguet           #+#    #+#             */
/*   Updated: 2024/02/16 12:24:19 by aalkhiro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Irc.hpp"

	std::string				_nick;
	std::string				_user;
	std::string				_realname;
	std::string				_host;
	std::string				_pass;
	std::string				_msg;
	std::string				_msgsToSend;
	std::vector<Channel *>	_channels;
	bool					_isRegistered;
	bool					_disconnect;

User::User(int fd): _sock(fd), _nick(""), _user(""),
					_realname(""), _host(""), _pass(""),
					_msg(""), _msgsToSend(""), _isRegistered(false),
					_disconnect(false) {}

User::~User() {}

int	User::getFd()const{	return _sock; }

std::string	User::getNick()const{ return _nick; }

std::string	User::getUser() const{ return _user; }

std::string	User::getRealname()const{	return _realname; }

std::string	User::getHost()const {return _host;};

std::string	User::getPass()const {return _pass;};

std::string	User::getMsg()const{ return _msg; }

std::string	User::getMsgsToSend() const{return _msgsToSend;};

bool		User::isRegisterd() const {return _isRegistered;}

bool		User::isDisconnect() const {return _disconnect;}

void		User::setNick(std::string str){ _nick = str; }

void		User::setUser(std::string str){ _user = str; }

void		User::setRealname(std::string str){ _realname = str; }

void		User::setHost(std::string str){_host = str;}

void		User::setPass(std::string str){_pass = str;}

void		User::setIsRegisterd(bool val){_isRegistered = val;}

void		User::setDisconnect(bool val){_disconnect = val;}

void		User::setMsg(std::string str){ _msg = str; }

void		User::addMsg(std::string str){ _msg += str; }

void		User::addMsgToSend(std::string str){ _msgsToSend += str; }

void		User::setMsgsToSend(std::string str){_msgsToSend = str;}

void		User::sendMsg(std::string msg){
	if (send(_sock, msg.c_str(), msg.length(), 0) < 0)
		std::cout << "Error: send to user " << strerror(errno) << std::endl;
}

std::string	User::extractCmd()
{
	std::string cmd;
	char const* cmdEnd = std::strstr(_msg.c_str(), "\r\n");
	if (cmdEnd == NULL)
		return ("");
	cmd = _msg.substr(0, cmdEnd - _msg.c_str());
	_msg = _msg.substr(cmd.size() + 2, _msg.size() - cmd.size() + 2);
	// std::cout << "debug: command extraction: |cmd|buffer|" << std::endl << "|" << cmd << "|" << _msg << "|" << std::endl;
	return (cmd);
}

