/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aalkhiro <aalkhiro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/11 14:48:05 by bfiguet           #+#    #+#             */
/*   Updated: 2024/02/28 15:12:27 by aalkhiro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Irc.hpp"

Server::Server(int port, const std::string &pw): _host(LOCAL_HOST), _pw(pw), _port(port), _sock(-1) {
	_sock = newSock();
	if (_sock < 0)
		throw(Server::BadServInit());
	pollfd	fd;
	fd.fd = _sock;
	fd.events = POLLIN;
	fd.revents = 0;
	_pollfds.push_back(fd);
}

Server::~Server() {
	std::cout << "~Server" << std::endl;
	if (_sock >= 0)
		close(_sock);
	for (std::vector<User*>::iterator i = _users.begin(); i != _users.end(); i++)
	{
		close((*i)->getFd());
		delete(*i);
	}
	for (std::vector<Channel*>::iterator i = _channels.begin(); i != _channels.end(); i++)
	{
		delete(*i);
		//i = _channels.erase(i);
	}
	std::cout << "END SERVER" <<std::endl;
}

int		Server::newSock(){
	int			serverSocket;
	struct		sockaddr_in server_addr = {};
	
	//create socket
	serverSocket = socket(AF_INET, SOCK_STREAM,0);
	if (serverSocket < 0)
		return (errMsg("Error: server socket error " + std::string(strerror(errno))));
	std::cout << "Server Socket connection created..." << std::endl;
	bzero((char *) &server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(_port);
	int optval = 1;
	if(setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0){
		close(serverSocket);
		return (errMsg("Error: setting socket options " + std::string(strerror(errno))));
	}
	if (fcntl(serverSocket,F_SETFL,O_NONBLOCK) < 0)
    {
        close(serverSocket);
        return (errMsg("Error: fcntl " + std::string(strerror(errno))));
    }
	//binds the socket to the address and port number specified in addr(custom data structure)
	if (bind(serverSocket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0)
	{
		close(serverSocket);
		return (errMsg("Error: binding socket " + std::string(strerror(errno))));
	}
	//Listening socket
	if (listen(serverSocket, 1000) < 0)
	{
		close(serverSocket);
		return (errMsg("Error: listening socket " + std::string(strerror(errno))));
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

//recv receive a message from a socket
int	Server::receiveMsg(int fd){
	char		buffer[1024];
	User*		user = findUser(fd);
	int valread = 1;
	user->setTimeStamp();
	bzero(buffer, BUFFERSIZE);
	valread = recv(fd, buffer, BUFFERSIZE, 0);
	if (valread <= 0)
	{
		if (valread == 0)
			std::cout << "User " << user->getNick() << " has disconnected with EOF" << std::endl;
		else
			std::cout << "Error recv: " << strerror(errno) << "disconnecting user " << user->getNick() << std::endl;
		user->setDisconnect(true);
		return(0);
	}
	std::cout << "Recieved msg from " << user->getNick() << ": " << buffer << std::endl;
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
	if (!user->getMsgsToSend().empty())
	{
		std::cout << user->getNick() << " messages to send:\n" << user->getMsgsToSend() << std::endl;
		if (send(user->getFd(), user->getMsgsToSend().c_str(), user->getMsgsToSend().length(), 0) < 0)
			errMsg("Error: send to user " + std::string(strerror(errno)));
		else
			user->setMsgsToSend("");
	}
	return (0);
}

int Server::pollerrHandler(int fd)
{
	if (fd == _sock)
		return(errMsg("Error: server socket " + std::string(strerror(errno))));
	findUser(fd)->setDisconnect(true);
	return (0);
}

int	Server::start(){
	int	events;
	std::cout << "Server IRC Start!" << std::endl;
	while (g_run == true)
	{
		events = poll((_pollfds.begin()).base(), _pollfds.size(), 0);
		if ( events < 0)
			return (errMsg("Error: poll error: " + std::string(strerror(errno))));
		if (events == 0)
				continue;
		for (std::vector<pollfd>::iterator i = _pollfds.begin(); i != _pollfds.end(); i++)
		{
			if ((*i).revents == 0)
				continue;
			else if ((*i).revents & POLLERR)
				if (pollerrHandler((*i).fd))
					return 1;
			if ((*i).revents & POLLIN)
			{
				pollinHandler(((*i)).fd);
				break;
			}
			else if ((*i).revents & POLLOUT)
				polloutHandler(((*i)).fd);
		}
		for (std::vector<pollfd>::iterator i = _pollfds.begin() + 1; i != _pollfds.end(); i++)
			callCmds(findUser(((*i)).fd));
		checkTimeout();
		deleteDisconnected();
		deleteEmptyChannels();
	}
	return 0;
}

void	Server::checkTimeout()
{
	struct timeval time;
	gettimeofday(&time, NULL);
	for (std::vector<User*>::iterator i = _users.begin(); i != _users.end(); i++)
	{
		if (time.tv_sec - (*i)->getTimeStamp().tv_sec >= 90)
		{
			std::cout << "User " << (*i)->getNick() << " has timed out." << std::endl;
			(*i)->setDisconnect(true);
		}
	}
}

void	Server::deleteEmptyChannels()
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

void	Server::deleteDisconnected()
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

int	Server::callCmds(User* user)
{
	std::string cmd = user->extractCmd();
	if (cmd.empty())
		return 0;
	executeCmd(cmd, user);
	if (std::strstr(user->getMsg().c_str(), "\r\n") != NULL)
		callCmds(user);
	if (!user->isRegisterd() && !user->getRealname().empty() && !user->getHost().empty())
	{
		if (user->getPass() == _pw)
		{
			user->setIsRegisterd(true);
			if (user->getNick().empty())
			{
				std::vector<std::string> args;
				args.push_back("");
				std::stringstream stream;
				stream << _users.size();
				std::string str;
				stream >> str;
				args.push_back("Guest" + str);
				cmdNick(this, args, user);
			}
			user->addMsgToSend(RPL_WELCOME(user->getNick(), user->getUser(), _host));
			user->addMsgToSend(RPL_YOURHOST(_host));
			user->addMsgToSend(RPL_CREATED(_host));
			user->addMsgToSend(RPL_MYINFO(_host));
			std::cout << "A new user:" << user;
		}
		else
		{
			user->setDisconnect(true);
			return 1;
		}
	}
	return 0;
}

void	Server::executeCmd(std::string str, User* user){
	std::vector<std::string>	arguments;
	std::string					word;

	word = str.substr(0, str.find(' '));
	int	(*fun[])(Server* server, std::vector<std::string> arguments, User* user) = {
		&cmdPass, &cmdNick, &cmdUser, &cmdInvite, &cmdTopic, &cmdKick, &cmdPart,
		&cmdPing, &cmdMode, &cmdQuit, &cmdPrivmsg, &cmdJoin, &cmdPrivmsg
	};
	const char* commands[] = {"PASS", "NICK", "USER", "INVITE", "TOPIC", "KICK", "PART", "PING", "MODE", "QUIT", "PRIVMSG", "JOIN", "MSG"};
	std::vector<std::string> _cmd(commands, commands + 13);
	int	ind = 0;
	for (std::vector<std::string>::iterator i = _cmd.begin(); i != _cmd.end(); i++)
	{
		if ( word.compare(*i) == 0)
		{
			arguments = ft_split(str, ' ');
			std::cout << "\nCommand= " << arguments[0] << std::endl;
			(*fun[ind])(this, arguments, user);
		}
		ind++;
	}
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

void	Server::deleteUserFromChannels(User* user)
{
	if (_channels.size() > 0)
		for (std::vector<Channel*>::iterator it=_channels.begin(); it == _channels.end(); it++)
			if ((*it)->isInChannel(user))
				(*it)->delUser(user);
}

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

const char* Server::BadServInit::what() const throw()
{
    return ("Failed to initilize server");
}

Channel*	Server::addChannel(std::string name)
{
	Channel* cha = new Channel(name);
	_channels.push_back(cha);
	return cha;
}
