/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   User.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aalkhiro <aalkhiro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/12 11:15:47 by bfiguet           #+#    #+#             */
/*   Updated: 2024/02/07 14:09:13 by aalkhiro         ###   ########.fr       */
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
	std::string				_msg;
	std::string				_msgsToSend;
	std::vector<Channel *>	_channels;
	//time_t					_timeOut;

public:
	User(int fd);
	~User();
	void		sendMsg(std::string msg);
	

	int			getFd() const;
	std::string	getNick()const;
	std::string	getUser() const;
	std::string	getRealname()const;
	std::string	getMsg()const;
	std::string	getMsgsToSend() const;
	
	void		setNick(std::string str);
	void		setUser(std::string str);
	void		setRealname(std::string str);
	void		setMsg(std::string str);
	void		addMsg(std::string str);
	void		addMsgToSend(std::string str);

	std::string	extractCmd(std::string cmds);
	
};

#endif
