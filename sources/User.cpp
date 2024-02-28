/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   User.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aalkhiro <aalkhiro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/12 11:46:33 by bfiguet           #+#    #+#             */
/*   Updated: 2024/02/28 13:48:55 by aalkhiro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Irc.hpp"

User::User(int fd): _sock(fd), _nick(""), _user(""),
					_realname(""), _host(""), _pass(""),
					_msg(""), _msgsToSend(""), _isRegistered(false),
					_disconnect(false), _nbChannel(10) {}

User::~User() {}

int	User::getFd()const{	return _sock; }

std::string	User::getNick()const{ return _nick; }

std::string	User::getUser() const{ return _user; }

std::string	User::getRealname()const{	return _realname; }

std::string	User::getHost()const {return _host;};

std::string	User::getPass()const {return _pass;};

std::string	User::getMsg()const{ return _msg; }

std::string	User::getMsgsToSend() const{return _msgsToSend;};

timeval		User::getTimeStamp() const
{
	return (_timestamp);
}


bool		User::isRegisterd() const {return _isRegistered;}

bool		User::isDisconnect() const {return _disconnect;}

bool		User::canAddNewChannel() const {return (_nbChannel - 1 >= 0);}

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

void		User::setTimeStamp()
{
	gettimeofday(&_timestamp, NULL);
}

std::string	User::extractCmd()
{
	std::string cmd;
	char const* cmdEnd = std::strstr(_msg.c_str(), "\r\n");
	if (cmdEnd == NULL)
		return ("");
	cmd = _msg.substr(0, cmdEnd - _msg.c_str());
	_msg = _msg.substr(cmd.size() + 2, _msg.size() - cmd.size() + 2);
	return (cmd);
}

std::ostream& operator<<(std::ostream& os, User const* user)
{
	os << std::endl << "User " << user->getUser() << " are connected on fd " << user->getFd() << std::endl;
	os << "nickname: " << user->getNick() << std::endl;
	os << "realname: " <<user->getRealname() << std::endl;
	os << "server: " <<user->getHost() << std::endl;
	os << "pass: " <<user->getPass() << std::endl << std::endl;
    return os;
}