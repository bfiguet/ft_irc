/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bfiguet <bfiguet@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/11 14:48:05 by bfiguet           #+#    #+#             */
/*   Updated: 2024/02/23 13:38:53 by bfiguet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Irc.hpp"

int	errMsg(std::string msg)
{
	std::cerr <<  msg << std::endl;
	return (-1);
}

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
	for (std::vector<Channel*>::iterator i = _channels.begin(); i != _channels.end();)
	{
		delete(*i);
		i = _channels.erase(i);
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

	//binds the socket to the address and port number specified in addr(custom data structure)
	if (bind(serverSocket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0)
	{
		close(_sock);
		return (errMsg("Error: binding socket " + std::string(strerror(errno))));
	}
	//Listening socket
	if (listen(serverSocket, 1000) < 0) //compare with others
	{
		close(_sock);
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
	user->setIsRegisterd(false);
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

	// std::cout << "debug: Receiving message" << buffer << std::endl;
	bzero(buffer, BUFFERSIZE);
	valread = recv(fd, buffer, BUFFERSIZE, 0);
	if (valread < 0 || valread == 0)
	{
		if (valread == 0)
			std::cout << "User " << user->getNick() << " has disconnected with EOF" << std::endl;
		else
			std::cout << "Error recv: " << strerror(errno) << "disconnecting user " << user->getNick() << std::endl;
		user->setDisconnect(true);
		return(0);
	}
	std::cout << "debug: message received " << buffer << std::endl;
	user->addMsg(buffer);
	return (0);
}

int	Server::pollinHandler(int fd)
{
	// std::cout << "debug: pollin event " << fd << std::endl;
	if (fd == _sock)
		return(newUser());
	return(receiveMsg(fd));
}

int Server::polloutHandler(int fd)
{
	// std::cout << "debug: pollout event " << fd << std::endl;
	User* user = findUser(fd);
	// std::cout << "User is " << user->getFd() << " " << user->getNick() << std::endl;
	if (!user->getMsgsToSend().empty())
	{
		std::cout << user->getNick() << " messages to send:\n" << user->getMsgsToSend() << std::endl;
		user->sendMsg(user->getMsgsToSend());
		user->setMsgsToSend("");
	}
	return (0);
}

int Server::pollerrHandler(int fd)
{
	// std::cout << "debug: pollerr event " << fd << std::endl;
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
		// std::cout << "debug: events " << events << std::endl;
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
		for (std::vector<pollfd>::iterator i = _pollfds.begin(); i != _pollfds.end(); i++)
		{
			// std::cout << "debug cmds on fd " << (*i).fd << std::endl;
			if (i != _pollfds.begin())
				if (callCmds(findUser(((*i)).fd)) == 1)
					std::cout << "need password to connect" << std::endl;
		}
		deleteDisconnected();
		deleteEmptyChannels();
	}
	return 0;
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
	// std::cout << "debug: executing cmd on user " << user->getFd() << std::endl;
	std::string cmd = user->extractCmd();
	 //std::cout << "debug: executing cmd ->" << cmd << "|" << cmd.size() << std::endl;
	if (cmd.empty())
		return 0;
	executeCmd(cmd, user);
	// std::cout << "debug: execution done" << std::endl;
	if (std::strstr(user->getMsg().c_str(), "\r\n") != NULL)
	{
		// std::cout << "debug: recalling callCmds:" << user->getMsg() << std::endl;
		callCmds(user);
	}
	if (!user->isRegisterd() && !user->getRealname().empty() && !user->getHost().empty())
	{
		if (user->getPass() == _pw)
		{
			user->setIsRegisterd(true);
			if (user->getNick().empty())
			{
				//user->addMsgToSend(ERR_NONICKNAMEGIVEN(user->getHost()));
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
			displayUser(user);
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
	// std::stringstream			is(str);
	std::string					word;
	char const*                 index;

	word = str.substr(0, str.find(' '));
	//std::cout << "debug: executeCmd " << str << std::endl;
	int	(*fun[])(Server* server, std::vector<std::string> arguments, User* user) = {
		&cmdPass, &cmdNick, &cmdUser, &cmdInvite, &cmdKill, &cmdTopic, &cmdKick, &cmdPart,
		&cmdPing, &cmdMode, &cmdQuit, &cmdPrivmsg, &cmdJoin, &cmdPrivmsg
	};
	const char* commands[] = {"PASS", "NICK", "USER", "INVITE", "KILL", "TOPIC", "KICK", "PART", "PING", "MODE", "QUIT", "PRIVMSG", "JOIN", "MSG"};
	std::vector<std::string> _cmd(commands, commands + 14);
	// std::cout << "debug: function pointer array done" << _cmd.size() << std::endl;
	// std::cout << "debug: _cmd " << _cmd[0] << std::endl;
	// for (int i = 0; i < 10; i++)
	int	ind = 0;
	for (std::vector<std::string>::iterator i = _cmd.begin(); i != _cmd.end(); i++)
	// for (std::size_t i = 0; i < _cmd.size(); ++i)
	{
		// std::cout << "debug: searching for command" << std::endl;
		if ( word.compare(*i) == 0)
		{
			// std::cout << "debug: command found" << std::endl;
			// while (!str.empty())
			// {
			// 	word = str.substr(0, str.find(' '));
			// 	str = str.substr(str.find(' ') + 1, -1);
			// 	arguments.push_back(word);
			// }
			while (!str.empty())
			{
				index = std::strstr(str.c_str(), " ");
				if (index == NULL)
				{
					word = str;
					str.clear();
				}
				else
				{
					word = str.substr(0, index - str.c_str());
					str = str.substr(index - str.c_str() + 1, str.size() - word.size() + 1);
				}
				arguments.push_back(word);
			}
			std::cout << "\nCommand= " << arguments[0] << std::endl;
			(*fun[ind])(this, arguments, user);
		}
		ind++;
	}
	//displayUser(user);
	// std::cout << "debug: command " << word << " not found" << std::endl;
}

std::vector<User*>::iterator	Server::delUser(User* user)
{
	//std::cout << "--delUser--" << std::endl;
	// std::cout << "debug: " << (*it)->getNick() << std::endl;
	deleteUserFromChannels(user);
	std::cout << "debug: 1" << std::endl;
	for(std::vector<pollfd>::iterator i = _pollfds.begin(); i != _pollfds.end(); i++)
		if ((*i).fd == user->getFd())
		{
			close((*i).fd);
			_pollfds.erase(i);
			break;
		}
	std::cout << "debug: 2" << std::endl;
	return (_users.erase(std::find(_users.begin(), _users.end(), user)));
	// std::cout << "debug: 3" << std::endl;
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
	std::cout << "User " << user->getUser() << " are connected"<< std::endl;
	std::cout << "nickname: " << user->getNick() << std::endl;
	std::cout << "realname: " <<user->getRealname() << std::endl;
	std::cout << "server: " <<user->getHost() << std::endl;
	std::cout << "pass: " <<user->getPass() << std::endl;
	std::cout << std::endl;
}

void	Server::deleteUserFromChannels(User* user)
{
	if (_channels.size() > 0)
		for (std::vector<Channel*>::iterator it=_channels.begin(); it == _channels.end(); it++)
			if ((*it)->isInChannel(user))
				(*it)->delUser(user);
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

const char* Server::BadServInit::what() const throw()
{
    return ("Failed to initilize server");
}

void	Server::addChannel(std::string name)
{
	Channel* cha = new Channel(name);
	_channels.push_back(cha);
}
