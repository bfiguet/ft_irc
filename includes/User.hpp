/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   User.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aalkhiro <aalkhiro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/12 11:15:47 by bfiguet           #+#    #+#             */
/*   Updated: 2024/02/09 12:20:51 by aalkhiro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef USER_HPP
#define USER_HPP

#include "Irc.hpp"

class Channel;

class User
{
private:
	int						_sock;
	std::string				_nick;
	std::string				_user;
	std::string				_realname;
	std::string				_host;
	std::string				_pass;
	std::string				_msg;
	std::string				_msgsToSend;
	std::vector<Channel *>	_channels;
	bool					_isRegistered;
	//time_t					_timeOut;

public:
	User(int fd);
	~User();
	void		sendMsg(std::string msg);
	

	int			getFd() const;
	std::string	getNick()const;
	std::string	getUser() const;
	std::string	getRealname()const;
	std::string	getHost()const;
	std::string	getPass()const;
	std::string	getMsg()const;
	std::string	getMsgsToSend() const;
	bool		isRegisterd() const;

	void		setIsRegisterd(bool val);
	void		setNick(std::string str);
	void		setUser(std::string str);
	void		setRealname(std::string str);
	void		setHost(std::string str);
	void		setPass(std::string str);
	void		setMsg(std::string str);
	void		addMsg(std::string str);
	void		addMsgToSend(std::string str);
	void		setMsgsToSend(std::string str);

	std::string	extractCmd();
	
};

#endif
