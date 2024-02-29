/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aalkhiro <aalkhiro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/11 14:48:05 by bfiguet           #+#    #+#             */
/*   Updated: 2024/02/29 12:56:22 by aalkhiro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Irc.hpp"

Server::Server(int port, const std::string &pw): _port(port), _sock(-1) {
	_sock = newSock();
	if (_sock < 0)
		throw(Server::BadServInit());
	pollfd	fd;
	fd.fd = _sock;
	fd.events = POLLIN;
	fd.revents = 0;
	_data = new ServerData(fd, pw);
}

Server::~Server() {
	std::cout << "~Server" << std::endl;
	if (_sock >= 0)
		close(_sock);
	delete(_data);
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

//recv receive a message from a socket
int	Server::receiveMsg(int fd){
	char		buffer[1024];
	User*		user = _data->findUser(fd);
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

void	Server::checkTimeout()
{
	struct timeval time;
	std::vector<User*> users = _data->getUsers();
	gettimeofday(&time, NULL);
	for (std::vector<User*>::iterator i = users.begin(); i != users.end(); i++)
	{
		if (time.tv_sec - (*i)->getTimeStamp().tv_sec >= 90)
		{
			std::cout << "User " << (*i)->getNick() << " has timed out." << std::endl;
			(*i)->setDisconnect(true);
		}
	}
}

void	Server::executeCmd(std::string str, User* user){
	std::vector<std::string>	arguments;
	std::string					word;

	word = str.substr(0, str.find(' '));
	int	(*fun[])(ServerData* serverData, std::vector<std::string> arguments, User* user) = {
		&cmdPass, &cmdNick, &cmdUser, &cmdInvite, &cmdTopic, &cmdKick, &cmdPart,
		&cmdPing, &cmdMode, &cmdQuit, &cmdPrivmsg, &cmdJoin, &cmdPrivmsg
	};
	const char* commands[] = {"PASS", "NICK", "USER", "INVITE", "TOPIC", "KICK", "PART", "PING", "MODE", "QUIT", "PRIVMSG", "JOIN", "MSG"};
	std::vector<std::string> cmd(commands, commands + 13);
	int	ind = 0;
	for (std::vector<std::string>::iterator i = cmd.begin(); i != cmd.end(); i++)
	{
		if ( word.compare(*i) == 0)
		{
			arguments = ft_split(str, ' ');
			std::cout << "\nCommand= " << arguments[0] << std::endl;
			(*fun[ind])(_data, arguments, user);
		}
		ind++;
	}
}

int	Server::callCmds(User* user)
{
	std::string host = _data->getHost();
	std::string cmd = user->extractCmd();
	if (cmd.empty())
		return 0;
	executeCmd(cmd, user);
	if (std::strstr(user->getMsg().c_str(), "\r\n") != NULL)
		callCmds(user);
	if (!user->isRegisterd() && !user->getRealname().empty() && !user->getHost().empty())
	{
		if (user->getPass() == _data->getPw())
		{
			user->setIsRegisterd(true);
			if (user->getNick().empty())
			{
				std::vector<std::string> args;
				args.push_back("");
				std::stringstream stream;
				stream << _data->getUsers().size();
				std::string str;
				stream >> str;
				args.push_back("Guest" + str);
				cmdNick(_data, args, user);
			}
			user->addMsgToSend(RPL_WELCOME(user->getNick(), user->getUser(), host));
			user->addMsgToSend(RPL_YOURHOST(host));
			user->addMsgToSend(RPL_CREATED(host));
			user->addMsgToSend(RPL_MYINFO(host));
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

int	Server::pollinHandler(int fd)
{
	if (fd == _sock)
		return(_data->newUser(_sock));
	return(receiveMsg(fd));
}

int Server::polloutHandler(int fd)
{
	User* user = _data->findUser(fd);
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
	_data->findUser(fd)->setDisconnect(true);
	return (0);
}

int	Server::start(){
	int	events;
	std::vector<pollfd>& pollfds = _data->getPollfds();
	std::cout << "Server IRC Start!" << std::endl;
	while (g_run == true)
	{
		events = poll((pollfds.begin()).base(), pollfds.size(), 0);
		if ( events < 0)
			return (errMsg("Error: poll error: " + std::string(strerror(errno))));
		if (events == 0)
				continue;
		for (std::vector<pollfd>::iterator i = pollfds.begin(); i != pollfds.end(); i++)
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
		for (std::vector<pollfd>::iterator i = pollfds.begin() + 1; i != pollfds.end(); i++)
			callCmds(_data->findUser(((*i)).fd));
		checkTimeout();
		_data->deleteDisconnected();
		_data->deleteEmptyChannels();
	}
	return 0;
}

const char* Server::BadServInit::what() const throw()
{
    return ("Failed to initilize server");
}
