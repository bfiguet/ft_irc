/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aalkhiro <aalkhiro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/11 14:48:05 by bfiguet           #+#    #+#             */
/*   Updated: 2024/02/07 15:52:44 by aalkhiro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Irc.hpp"

Server::Server(int port, const std::string &pw): _host(LOCAL_HOST), _pw(pw), _port(port) {
	_sock = newSock();
	if (_sock < 0)
		throw(new std::exception);
	pollfd	fd;
	fd.fd = _sock;
	fd.events = POLLIN;
	fd.revents = 0;
	_pollfds.push_back(fd);
	std::string _cmd[11] = {"PASS", "NICK", "USER", "JOIN", "KILL", "TOPIC", "KICK", "PART", "PING", "MODE", "QUIT"};
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
	pollfd temppollfd;
	temppollfd.fd = userSocket;
	temppollfd.events = POLLIN | POLLOUT;
	temppollfd.revents = 0;
	_pollfds.push_back(temppollfd);
	User* user = new User(userSocket);
	_users.push_back(user);
	std::cout << "New user on fd " << user->getFd() << std::endl;
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
			std::cout << "User " << user->getNick() << " has disconnected with EOF" << std::endl;
		else
			std::cout << "Error recv: " << strerror(errno) << "disconnecting user " << user->getNick() << std::endl;
		delUser(user);
		return(0);
	}
	std::cout << "debug: message recieved " << buffer << std::endl;
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
		events = poll((_pollfds.begin()).base(), _pollfds.size(), 0);
		std::cout << "debug events " << events << std::endl;
		if ( events < 0)
		{
			std::cerr << "Error: poll error: " << strerror(errno) << std::endl;
			return 1;
		}
		if (events == 0)
				continue;
		for (std::vector<pollfd>::iterator i = _pollfds.begin(); i != _pollfds.end(); i++)
		{
			std::cout << "debug poll events on fd " << (*i).fd << std::endl;
			if (((*i)).revents == 0)
				continue;
			if ((((*i)).revents) == POLLIN)
			{
				pollinHandler(((*i)).fd);
				break;
			}
			else if (((*i)).revents == POLLOUT)
				polloutHandler(((*i)).fd);
			else if (((*i)).revents == POLLERR)
				if (pollerrHandler(((*i)).fd))
					return 1;
		}
		for (std::vector<pollfd>::iterator i = _pollfds.begin(); i != _pollfds.end(); i++)
		{
			std::cout << "debug cmds on fd " << (*i).fd << std::endl;
			if (i != _pollfds.begin())
				callCmds(findUser(((*i)).fd));
		}
	}
	return 0;
}

void	Server::callCmds(User* user)
{
	std::string cmd = user->extractCmd(user->getMsg());
	if (!cmd.empty())
		executeCmd(cmd, user);
	if (std::strstr(user->getMsg().c_str(), "\r\n") != NULL)
		callCmds(user);
	if (!user->getNick().empty() && !user->getRealname().empty() && !user->getHost().empty())
	{
		if (user->getPass() == _pw)
		{
			user->addMsgToSend(RPL_WELCOME(user->getNick(), user->getUser(), _host));
			user->addMsgToSend(RPL_YOURHOST(_host));
			user->addMsgToSend(RPL_CREATED(_host));
			user->addMsgToSend(RPL_MYINFO(_host));
			displayUser(user);
		}
		else
			delUser(user);
	}
}

void	Server::executeCmd(std::string str, User* user){
	//std::cout<< "executCmd"<<std::endl;
	std::vector<std::string>	arguments;
	std::stringstream			is(str);
	std::string					word;
	
	std::getline(is, word, ' ');
	int	(*fun[11])(Server* server, std::vector<std::string> arguments, User* user) = {
		&cmdNick, &cmdPass, &cmdUser, &cmdJoin,
		&cmdKill, &cmdTopic, &cmdKick, &cmdPart,
		&cmdPing, &cmdMode, &cmdQuit
	};
	for (int i = 0; i < 10; i++)
	{
		if (_cmd[i] == word)
		{
			while (std::getline(is, word, ' '))
				arguments.push_back(word);
			(*fun[i])(this, arguments, user);
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
	for (std::vector<pollfd>::iterator i = _pollfds.begin(); i != _pollfds.end(); i++)
	{
		if ((i)->fd == user->getFd())
			_pollfds.erase(i);
	}
	delUser(user);
}

User*	Server::findUser(std::string nickname)
{
	for (std::vector<User*>::iterator i = _users.begin(); i !=_users.end(); i++)
		if ((*i)->getNick() == nickname)
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
	std::cout <<"User " << user->getUser() << " are connected"<< std::endl;
	std::cout << " nickname: " << user->getNick() << std::endl;
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

	// std::vector<Channel*>::iterator chan = std::find(_channels.begin(), _channels.end(), channelName); DOESN'T WORK!!!
Channel*	Server::findChannel(std::string channelName)
{
	for (std::vector<Channel*>::iterator it = _channels.begin(); it != _channels.end(); it++)
		if ((*it)->getName() == channelName)
			return (*it);
	return (NULL);
}

std::vector<User *>	Server::getUsers() const {return _users;}

std::string	Server::getPw()const {return _pw;}

std::string	Server::getHost()const { return _host;}
