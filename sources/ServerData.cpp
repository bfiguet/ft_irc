/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerData.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aalkhiro <aalkhiro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/11 14:48:05 by bfiguet           #+#    #+#             */
/*   Updated: 2024/02/29 09:59:57 by aalkhiro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Irc.hpp"

ServerData::ServerData(pollfd serverPollfd, std::string pw)
{
	_pw = pw;
	_host = LOCAL_HOST;
	_pollfds.push_back(serverPollfd);
}

ServerData::~ServerData() {
	std::cout << "~ServerData" << std::endl;
	for (std::vector<User*>::iterator i = _users.begin(); i != _users.end(); i++)
	{
		close((*i)->getFd());
		delete(*i);
	}
	for (std::vector<Channel*>::iterator i = _channels.begin(); i != _channels.end(); i++)
		delete(*i);
	std::cout << "END ServerData" <<std::endl;
}

int	ServerData::newUser(int servSock){
	int			userSocket;
	struct		sockaddr_in ServerData_addr = {};
	socklen_t	size = sizeof(ServerData_addr);
	//accept user
	userSocket = accept(servSock, (sockaddr*)&ServerData_addr, &size);
	if (userSocket < 0)
		return(errMsg("Error: failed to accept new connection" + std::string(strerror(errno))));
	pollfd temppollfd;
	temppollfd.fd = userSocket;
	temppollfd.events = POLLIN | POLLOUT;
	temppollfd.revents = 0;
	_pollfds.push_back(temppollfd);
	User* user = new User(userSocket);
	user->setTimeStamp();
	_users.push_back(user);
	std::cout << "New user on fd " << user->getFd() << std::endl;
	return (0);
}

Channel*	ServerData::addChannel(std::string name)
{
	Channel* cha = new Channel(name);
	_channels.push_back(cha);
	return cha;
}

User*	ServerData::findUser(int fd){
	for (std::vector<User*>::iterator i = _users.begin(); i != _users.end(); i++)
	{
		if ((*i)->getFd() == fd)
			return(*i);
	}
	return (NULL);
}

User*	ServerData::findUser(std::string nickname)
{
	for (std::vector<User*>::iterator i = _users.begin(); i !=_users.end(); i++)
		if ((*i)->getNick() == nickname)
			return (*i);
	return (NULL);
}

Channel*	ServerData::findChannel(std::string channelName)
{
	for (std::vector<Channel*>::iterator it = _channels.begin(); it != _channels.end(); it++)
		if ((*it)->getName() == channelName)
			return (*it);
	return (NULL);
}

void	ServerData::deleteDisconnected()
{
	for (std::vector<User*>::iterator i = _users.begin(); i != _users.end();)
	{
		if ((*i)->isDisconnect())
		{
			std::cout << "Disconnecting user " << (*i)->getNick() << std::endl;
			for(std::vector<pollfd>::iterator it = _pollfds.begin(); it != _pollfds.end(); it++)
				if ((*it).fd == (*i)->getFd())
				{
					close((*it).fd);
					_pollfds.erase(it);
					break;
				}
			deleteUserFromChannels(*i);
			delete(*i);
			i = _users.erase(i);
		}
		else
			i++;
	}
}

void	ServerData::deleteEmptyChannels()
{
	for (std::vector<Channel*>::iterator i = _channels.begin(); i != _channels.end();)
		if ((*i)->getUserCount() == 0)
		{
			delete(*i);
			i = _channels.erase(i);
		}
		else
			i++;
}

void	ServerData::deleteUserFromChannels(User* user)
{
	if (_channels.size() > 0)
		for (std::vector<Channel*>::iterator it=_channels.begin(); it != _channels.end(); it++)
			if ((*it)->isInChannel(user))
				(*it)->delUser(user);
}

std::string				ServerData::getPw()const {return _pw;}

std::string				ServerData::getHost()const { return _host;}

std::vector<User *>		ServerData::getUsers() const {return _users;}

std::vector<pollfd>&	ServerData::getPollfds() {return _pollfds;}