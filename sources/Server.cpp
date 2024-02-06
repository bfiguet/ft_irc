/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aalkhiro <aalkhiro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/11 14:48:05 by bfiguet           #+#    #+#             */
/*   Updated: 2024/02/06 15:46:32 by aalkhiro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Irc.hpp"

Server::Server(int port, const std::string &pw): _host(LOCAL_HOST), _pw(pw), _port(port) {
	_sock = newSock();
	if (_sock < 0)
		throw(new std::exception);
	pollfd	fd = {_sock, POLLIN, 0};
	_pollfds.push_back(fd);
	std::string _cmd[10] = {"NICK", "PASS", "USER", "JOIN", "KILL", "TOPIC", "KICK", "PART", "PING", "MODE"};
}

Server::~Server() {
	for (std::vector<User*>::iterator i = _users.begin(); i != _users.end(); i++)
		{
			close((*i)->getFd());
			delete(*i);
		}
	std::cout << "END SERVER" <<std::endl;
}

int		Server::newSock(){
	int			serverSocket;
	struct		sockaddr_in server_addr = {};
	
	//create socket
	serverSocket = socket(AF_INET, SOCK_STREAM,0);
	if (serverSocket < 0)
	{
		std::cout <<"Error: server socket error " << strerror(errno) << std::endl;
		return (-1);
	}
	std::cout << "Server Socket connection created..." << std::endl;
	bzero((char *) &server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(_port);

	//binds the socket to the address and port number specified in addr(custom data structure)
	if (bind(serverSocket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0)
	{
		std::cout << "Error: binding socket " << strerror(errno) << std::endl;
		return (-1);
	}
	//Listening socket
	if (listen(serverSocket, 1000) < 0) //compare with others
	{
		std::cout << "Error: listening socket " << strerror(errno) << std::endl;
		return (-1); //Need to be changed
	}
	return serverSocket;
}

int	Server::newUser(){
	int			userSocket;
	struct		sockaddr_in server_addr = {};
	socklen_t	size = sizeof(server_addr);
	//accept user
	userSocket = accept(_sock, (sockaddr*)&server_addr, &size);
	if (userSocket < 0)
	{
		std::cerr << "Error: failed to accept new connection" << strerror(errno) << std::endl;
		return(1);
	}
	pollfd pollfd = {userSocket, POLLIN | POLLOUT, 0};
	_pollfds.push_back(pollfd);
	User* user = new User(userSocket);
	_users.push_back(user);
	// user->sendMsg(RPL_WELCOME(user->getNickname(), user->getUsername(), user->getHostname()));
	// user->sendMsg(RPL_YOURHOST(user->getHostname()));
	// user->sendMsg(RPL_CREATED(user->getHostname()));
	// user->sendMsg(RPL_MYINFO(user->getHostname()));
	// displayUser(user);
	return (0);
}

//recv receive a message from a socket
int	Server::receiveMsg(int fd){
	char		buffer[1024];
	User*		user = findUser(fd);
	bzero(buffer, BUFFERSIZE);
	int valread = 1;

	bzero(buffer, BUFFERSIZE);
	valread = recv(fd, buffer, BUFFERSIZE, 0);
	if (valread < 0 || valread == 0)
	{
		if (valread == 0)
			std::cout << "User " << user->getNickname() << " has disconnected with EOF" << std::endl;
		else
			std::cout << "Error recv: " << strerror(errno) << "disconnecting user " << user->getNickname() << std::endl;
		delUser(user);
		return(0);
	}
	user->addMsg(buffer);
	return (0);
}

int	Server::pollinHandler(int fd)
{
	if (fd == _sock)
		return(newUser());
	return(receiveMsg(fd));
}

int Server::polloutHandler(int fd)
{
	User* user = findUser(fd);
	user->sendMsg(user->getMsgsToSend());
	return (0);
}

int Server::pollerrHandler(int fd)
{
	if (fd == _sock)
	{
		std::cout << "Error: server socket " << strerror(errno) << std::endl;
		return(1);
	}
	delUser(findUser(fd));
	return (0);
}

int	Server::start(){
	int	events;
	std::cout << "Server IRC Start!" << std::endl;
	while (g_run == true)
	{
		events = poll(_pollfds.begin().base(), _pollfds.size(), 0);
		if ( events < 0)
		{
			std::cerr << "Error: poll error: " << strerror(errno) << std::endl;
			return;
		}
		if (events == 0)
				continue;
		for (std::vector<pollfd>::iterator i = _pollfds.begin(); i != _pollfds.end(); i++)
		{
			//call cmds \r\n
			if ((*i).revents == 0)
				continue;
			if (((*i).revents) == POLLIN)
				pollinHandler((*i).fd);
			else if ((*i).revents == POLLOUT)
				polloutHandler((*i).fd);
			else if ((*i).revents == POLLERR)
				if (pollerrHandler((*i).fd))
					return (1);
		}
	}
}

void	Server::executeCmd(std::string str, User* user){
	//std::cout<< "executCmd"<<std::endl;
	std::vector<std::string>	arguments;
	std::stringstream			is(str);
	std::string					word;
	std::getline(is, word, ' ');

	int	(Server::*fun[10])(std::vector<std::string> arguments, User* user) = {
		&Server::cmdNick,
		&Server::cmdPass,
		&Server::cmdUser,
		&Server::cmdJoin,
		&Server::cmdKill,
		&Server::cmdTopic,
		&Server::cmdKick,
		&Server::cmdPart,
		&Server::cmdPing,
		&Server::cmdMode
	};
	for (int i = 0; i < 10; i++)
	{
		if (_cmd[i] == word)
		{
			while (std::getline(is, word, ' '))
				arguments.push_back(word);
			(this->*fun[i])(arguments, user);
		}
	}
}

void	Server::delUser(User* user)
{
	deleteUserFromChannels(user);
	disconnectUser(user);
	_users.erase(std::find(_users.begin(), _users.end(), user));
}

void	Server::disconnectUser(User* user)
{	
	close(user->getFd());
	for (std::vector<pollfd>::iterator it = _pollfds.begin(); it != _pollfds.end(); i++)
	{
		if ((*i)->fd == user->getFd())
			_pollfds.erase(i);
	}
	delUser(user);
}

//std::cout<< "DS findUser by nickname" <<std::endl;
User*	Server::findUser(std::string nickname)
{
	for (std::vector<User*>::iterator i = _users.begin(); i !=_users.end(); i++)
		if ((*i)->getNickname() == nickname)
			return (*i);
	return (NULL);
}

User*	Server::findUser(int fd){
	for (std::vector<User*>::iterator i = _users.begin(); i != _users.end(); i++)
	{
		if ((*i)->getFd() == fd)
			return(*i);
	}
	return (NULL);
}

void	Server::displayUser(User* user){
	std::cout << "New user received" << std::endl;
	std::cout <<"User " << user->getUsername() << " are connected"<< std::endl;
	std::cout << " nickname: " << user->getNickname() << std::endl;
	std::cout << " realname: " <<user->getRealname() << std::endl;
	std::cout << std::endl;
}

void	Server::deleteUserFromChannels(User* user)
{
	for (std::vector<Channel*>::iterator it=_channels.begin(); ;it++)
    {
		if ((*it)->isInChannel(user))
			(*it)->delUser(user);
        if((*it)->getUserCount() == 0)
            _channels.erase(it);
        if (it == _channels.end())
            break;
    }
}

Channel*	Server::findChannel(std::string channelName)
{
	for (std::vector<Channel *>::iterator i = _channels.begin(); i != _channels.end(); i++)
		if ((*i)->getName() == channelName)
			return (*i);
	return (NULL);
}

bool	Server::isChannel(std::string str){
	for (size_t i = 0; i < _channels.size(); i++)
	{
		if (_channels[i]->getName() == str)
			return true;
	}
	return false;
}
