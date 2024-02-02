/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bfiguet <bfiguet@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/11 14:48:05 by bfiguet           #+#    #+#             */
/*   Updated: 2024/02/02 19:40:03 by bfiguet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Irc.hpp"

Server::Server(int port, const std::string &pw): _host(LOCAL_HOST), _pw(pw), _port(port) {
	_sock = newSock();
	pollfd	fd = {_sock, POLLIN, 0};
	_pollfds.push_back(fd);
	std::string _cmd[10] = {"NICK", "PASS", "USER", "JOIN", "KILL", "TOPIC", "KICK", "PART", "PING", "MODE"};
}

Server::~Server() {
	for (std::vector<pollfd>::iterator i = _pollfds.begin(); i != _pollfds.end(); i++)
		close((*i).fd);
	std::cout << "END DESTRUCTOR SERVER" <<std::endl;
}

void	Server::start(){
	std::cout << "Server IRC Start!" << std::endl;
	while (g_run == true)
	{
		if (poll(_pollfds.begin().base(), _pollfds.size(), 0) < 0)
			break;
		for (std::vector<pollfd>::iterator i = _pollfds.begin(); i != _pollfds.end(); i++)
		{
			if ((*i).revents == 0)
				continue;
			if ((*i).fd == _sock && ((*i).revents) == POLLIN)
			{
				newUser();
				continue;
			}
			//else if ((_pollfds[i].revents & POLLOUT) == POLLOUT)
			//{
			//	std::cout << "DS else if ((_pollfds[i].revents & POLLOUT) == POLLOUT)" << std::endl;
			//	if (_pollfds[i].fd != _sock)
			//	{
			//		std::cout << "error, no found connexion to user" << std::endl;
			//		break;
			//	}
			//	else
			//		std::cout << "error, ......" << std::endl;
			//}
			else if ((*i).revents == POLLERR)
			{
				if ((*i).fd == _sock)
				{
					std::cout << "error, Listen socket error" << std::endl;
					break;
				}
				else
				{
					delUser(findUser((*i).fd));
					break;
				}
			}
			receiveMsg((*i).fd);
		}
		//std::cout << "ds while" << std::endl;
	}
}

int		Server::newSock(){
	int			serverSocket;
	struct		sockaddr_in server_addr = {};
	
	//create socket
	serverSocket = socket(AF_INET, SOCK_STREAM,0);
	if (serverSocket < 0)
	{
		std::cout <<"Error establishing connection." << std::endl;
		exit (1);
	}
	std::cout << "Server Socket connection created..." << std::endl;
	bzero((char *) &server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(_port);

	//binds the socket to the address and port number specified in addr(custom data structure)
	if (bind(serverSocket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0)
	{
		std::cout << "Error binding socket." << std::endl;
		exit (1);
	}
	std::cout << "Looking for serverSocket..." <<std::endl;

	//Listening socket
	if (listen(serverSocket, 1000) < 0) //compare with others
	{
		std::cout << "Error listening socket." << std::endl;
		exit (1); //Need to be changed
	}
	return serverSocket;
}

void	Server::newUser(){
	int			userSocket;
	char		hostBuffer[BUFFERSIZE];
	struct		sockaddr_in server_addr = {};
	socklen_t	size = sizeof(server_addr);

	//accept user
	userSocket = accept(_sock, (sockaddr*)&server_addr, &size);
	if (userSocket < 0)
	{
		std::cout << "Error on accepting new user" << std::endl;
		exit(1); //need to be changed
	}
	if (getnameinfo((struct sockaddr *) &server_addr, sizeof(server_addr), hostBuffer, NI_MAXHOST, NULL, 0, NI_NUMERICSERV) != 0) //compare with others
	{
		std::cout << "Error on getting hostname new user" << std::endl;
		exit(1);
	}
	User* user = new User(userSocket, hostBuffer);
	_users.push_back(user);
	pollfd pollfd = {userSocket, POLLIN, 0};
	_pollfds.push_back(pollfd);
	user->sendMsg(RPL_WELCOME(user->getNickname(), user->getUsername(), user->getHostname()));
	user->sendMsg(RPL_YOURHOST(user->getHostname()));
	user->sendMsg(RPL_CREATED(user->getHostname()));
	user->sendMsg(RPL_MYINFO(user->getHostname()));
	displayUser(user);
}

void	Server::receiveMsg(int fd){
	std::string msg = readMsg(fd);
	if (msg.size() != 0)
		executeCmd(msg, findUser(fd));

	// try
	// {
	// 	this->_cmd = splitCmd(readMsg(fd));
	// }
	// catch(const std::exception& e)
	// {
	// 	disconnectUser(findUser(fd));
	// 	std::cerr << e.what() << '\n';
	// 	return;
	// }
	//std::cout << "DS printMsg AVANT parseCmd" << std::endl;
	// for (std::vector<std::string>::iterator it = this->_cmd.begin(); it != this->_cmd.end(); it++)
	// {
	// 	//std::cout << "Ds for avant parseCmd" << std::endl;
	// 	//std::cout << "apres parseCmd ds boucle for" << std::endl;
	// }
	//std::cout << "apres boucle for" << std::endl;
	//displayUser();
}

//recv receive a message from a socket
std::string	Server::readMsg(int fd){
	std::string	msg;
	char		buffer[512];
	User*		user = findUser(fd);
	bzero(buffer, BUFFERSIZE);
	msg = user->getMsg();
	int valread = 1;
	const char*	temp;

	while(valread != 0)
	{
		bzero(buffer, BUFFERSIZE);
		valread = recv(fd, buffer, BUFFERSIZE, 0);
		if (valread < 0)
		{
			std::cout << "Error read error" << std::endl;
			exit(1);// need to be changed
		}
		if (valread > 512 || msg.size() > 512)
		{
			std::cout << "Error message too long" << std::endl;
			exit(1);// need to be changed disconnect user
		}
		msg += buffer;
	}
	temp = strstr(msg.c_str(), "\r\n");
    if (temp)
    {
		user->addMsg(msg.substr(temp - &msg[0] + 2, msg.size()));
		msg = msg.substr(0, temp - &msg[0]);
    }
	else
	{
		user->addMsg(msg);
		msg = "";
	}
	return msg;
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
	std::vector<pollfd>::iterator i = _pollfds.begin();
	delUser(user);
	while (i != _pollfds.end())
	{
		if (i->fd == user->getFd())
		{
			_pollfds.erase(i);
			break;
		}
		i++;
	}
	close(user->getFd());
	//_users.erase(std::find(_users.begin(), _users.end(), user));	
	//_pollfds.erase(std::find(_pollfds.begin(), _pollfds.end(), user->getFd()));
	//close(user->getFd());
	std::cout<< "Disconnection Successful" <<std::endl;
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
