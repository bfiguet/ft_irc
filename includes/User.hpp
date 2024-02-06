/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   User.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aalkhiro <aalkhiro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/12 11:15:47 by bfiguet           #+#    #+#             */
/*   Updated: 2024/02/06 14:00:38 by aalkhiro         ###   ########.fr       */
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
	std::string				_nickname;
	std::string				_fullname;
	std::string				_hostname;
	std::string				_msg;
	std::string				_msgsToSend;
	std::vector<Channel *>	_channels;
	time_t					_timeOut;

public:
	User(int fd);
	~User();
	void		sendMsg(std::string msg);

	int			getFd() const;
	std::string	getNickname()const;
	std::string	getUsername() const;
	std::string	getRealname()const;
	std::string	getHostname()const;
	std::string	getMsg()const;
	std::string	getMsgsToSend() const;
	
	void		setNickname(std::string str);
	void		setUsername(std::string str);
	void		setRealname(std::string str);
	void		setHostname(std::string str);
	void		setMsg(std::string str);
	void		addMsg(std::string str);
	
};

#endif
