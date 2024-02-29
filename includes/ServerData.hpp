/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerData.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aalkhiro <aalkhiro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/09 15:56:39 by bfiguet           #+#    #+#             */
/*   Updated: 2024/02/29 12:55:28 by aalkhiro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVERDATA_HPP
#define SERVERDATA_HPP

#include "Irc.hpp"

class ServerData
{
private:
	std::string 			_pw;
	std::string				_host;
	std::vector<User*>		_users;
	std::vector<Channel*>	_channels;
	std::vector<pollfd>		_pollfds;

public:
	ServerData(pollfd serverPollfd, std::string pw);
	~ServerData();
	
	std::string				getHost() const;
	std::string				getPw()const;
	std::vector<User*>		getUsers() const;
	std::vector<pollfd>&	getPollfds();
	int						newUser(int servSock);
	Channel*				addChannel(std::string name);
	User*					findUser(int fd);
	User*					findUser(std::string nickname);
	Channel*				findChannel(std::string str);
	void					deleteUserFromChannels(User* user);
	void					deleteDisconnected();
	void					deleteEmptyChannels();
};

#endif
