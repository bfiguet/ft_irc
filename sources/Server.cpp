/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aalkhiro <aalkhiro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/11 14:48:05 by bfiguet           #+#    #+#             */
/*   Updated: 2024/02/09 13:15:52 by aalkhiro         ###   ########.fr       */
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
}

Server::~Server() {
	close(_sock);
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
		delUser(user);
		return(0);
	}
	// std::cout << "debug: message received " << buffer << std::endl;
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
		std::cout << user->getNick() << " messages to send:" << user->getMsgsToSend() << std::endl;
		user->sendMsg(user->getMsgsToSend());
		user->setMsgsToSend("");
	}
	return (0);
}

int Server::pollerrHandler(int fd)
{
	// std::cout << "debug: pollerr event " << fd << std::endl;
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
		// std::cout << "debug: events " << events << std::endl;
		if ( events < 0)
		{
			std::cerr << "Error: poll error: " << strerror(errno) << std::endl;
			return 1;
		}
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
				callCmds(findUser(((*i)).fd));
		}
	}
	return 0;
}

void	Server::callCmds(User* user)
{
	// std::cout << "debug: executing cmd on user " << user->getFd() << std::endl;
	std::string cmd = user->extractCmd();
	// std::cout << "debug: executing cmd ->" << cmd << "|" << cmd.size() << std::endl;
	if (cmd.empty())
		return;
	executeCmd(cmd, user);
	// std::cout << "debug: execution done" << std::endl;
	if (std::strstr(user->getMsg().c_str(), "\r\n") != NULL)
	{
		// std::cout << "debug: recalling callCmds:" << user->getMsg() << std::endl;
		callCmds(user);
	}
	if (!user->isRegisterd() && !user->getNick().empty() && !user->getRealname().empty() && !user->getHost().empty())
	{
		// std::cout << "debug: checking registeration" << std::endl;
		if (user->getPass() == _pw)
		{
			user->setIsRegisterd(true);
			user->addMsgToSend(RPL_WELCOME(user->getNick(), user->getUser(), _host));
			user->addMsgToSend(RPL_YOURHOST(_host));
			user->addMsgToSend(RPL_CREATED(_host));
			user->addMsgToSend(RPL_MYINFO(_host));
			displayUser(user);
			// std::cout << "debug: registeration done" << std::endl;
		}
		else
			delUser(user);
	}
}

void	Server::executeCmd(std::string str, User* user){
	std::vector<std::string>	arguments;
	// std::stringstream			is(str);
	std::string					word;
	char const*                 index;

	// std::cout << "debug: executeCmd " << str << " for user " << user->getFd() << std::endl;
	word = str.substr(0, str.find(' '));
	// std::cout << "debug: cmd word obtained " << word << std::endl;
	int	(*fun[11])(Server* server, std::vector<std::string> arguments, User* user) = {
		&cmdPass, &cmdNick, &cmdUser, &cmdInvite,
		&cmdKill, &cmdTopic, &cmdKick, &cmdPart,
		&cmdPing, &cmdMode, &cmdQuit
	};
	const char* commands[] = {"PASS", "NICK", "USER", "INVITE", "KILL", "TOPIC", "KICK", "PART", "PING", "MODE", "QUIT"};
	std::vector<std::string> _cmd(commands, commands + 11);
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
			// std::cout << "debug: calling function for cmd " << word << std::endl;
			(*fun[ind])(this, arguments, user);
		}
		ind++;
	}
	displayUser(user);
	// std::cout << "debug: command " << word << " not found" << std::endl;
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
	std::cout << " server: " <<user->getHost() << std::endl;
	std::cout << " pass: " <<user->getPass() << std::endl;
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
