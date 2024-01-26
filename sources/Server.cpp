/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bfiguet <bfiguet@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/11 14:48:05 by bfiguet           #+#    #+#             */
/*   Updated: 2024/01/25 21:31:24 by bfiguet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Irc.hpp"

Server::Server(int port, const std::string &pw): _host(LOCAL_HOST), _pw(pw), _port(port) {
	_sock = newSock();
}

Server::~Server() {
	for (size_t i = 0; i < _pollfds.size(); i++)
		close(_pollfds[i].fd);
	std::cout << "ds DESTRUCTOR SERVER" <<std::endl;
}

/*int poll(struct pollfd *fds, nfds_t nfds, int délai);

struct pollfd {
    int   fd;         => Descripteur de fichier 
    short events;     => Événements attendus    
    short revents;    => Événements détectés    

--------------------------------------------------------------

htons():
The htons() function converts the unsigned short integer hostshort
from host byte order to network byte order.

--------------------------------------------------------------

recv():
int recv(int socket, void* buffer, size_t len, int flags);

Réceptionne des données sur le socket en paramètre. 

- socket est le socket duquel réceptionner les données. 
- buffer est un tampon où stocker les données reçues. 
- len est le nombre d'octets maximal à réceptionner. Typiquement, il s'agira de la place disponible dans le tampon. 
- flags est un masque d'options. Généralement 0.
Retourne le nombre d'octets reçus et stockés dans buffer. 
Peut retourner 0 si la connexion a été terminée. Retourne -1 en cas d'erreur.

--------------------------------------------------------------*/

void	Server::start(){
	pollfd	fd = {_sock, POLLIN, 0};
	_pollfds.push_back(fd);

	std::cout << "Server IRC Start!" << std::endl;
	while (g_run == true)
	{
		if (poll(_pollfds.begin().base(), _pollfds.size(), -1) < 0)
			break;
		for (size_t i = 0; i < _pollfds.size(); i++)
		{
			if (_pollfds[i].revents == 0)
				continue;
			if ((_pollfds[i].revents & POLLIN) == POLLIN)
			{
				if (_pollfds[i].fd == _sock)
				{
					newUser();
					//displayUser();
					break;
				}
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
			//else if ((_pollfds[i].revents & POLLERR) == POLLERR)
			//{
			//	std::cout << "DS else if ((_pollfds[i].revents & POLLERR) == POLLERR)" << std::endl;
			//	if (_pollfds[i].fd == _sock)
			//	{
			//		std::cout << "error, Listen socket error" << std::endl;
			//		break;
			//	}
			//	else
			//	{
			//		delUser(_pollfds[i].fd);
			//		break;
			//	}
			//}
			printMsg(_pollfds[i].fd);
		}
		printf("ds while\n");
	}
	//for (size_t i = 0; i < _pollfds.size(); i++)
	//	close(_pollfds[i].fd);
}

//int socket(int domain, int type, int protocol);
//AF_INET For communicating between processes on different hosts with IPV4
//SOCK_STREAM: TCP(reliable, connection-oriented)

//struct sockaddr_in, struct in_addr:
//Structures for handling internet addresses

//int setsockopt(int sockfd, int level, int optname,  const void *optval, socklen_t optlen);

int		Server::newSock(){
	int			user;
	struct		sockaddr_in server_addr = {};
	
	//create socket
	user = socket(AF_INET, SOCK_STREAM,0);
	if (user < 0)
	{
		std::cout <<"Error establishing connection." << std::endl;
		exit (1);
	}
	bzero((char *) &server_addr, sizeof(server_addr));
	std::cout << "Server Socket connection created..." << std::endl;
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(_port);

	//binds the socket to the address and port number specified in addr(custom data structure)
	if (bind(user, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0)
	{
		std::cout << "Error binding socket." << std::endl;
		exit (1);
	}
	std::cout << "Looking for user..." <<std::endl;

	//Listening socket
	if (listen(user, 1000) < 0)
	{
		std::cout << "Error listening socket." << std::endl;
		exit (1);
	}
	return user;
}

//int getnameinfo(const struct sockaddr *sa, socklen_t salen,
//               char *host, size_t hostlen,
//                char *serv, size_t servlen, int flags);

// NI_NUMERICSERV If set, then the numeric form of the service address is returned.  
//in c++ #define NI_MAXHOST  1025
void	Server::newUser(){
	int			server;
	char		host[BUFFERSIZE];
	struct		sockaddr_in server_addr = {};
	socklen_t	size = sizeof(server_addr);

	//accept user
	server = accept(_sock, (sockaddr*)&server_addr, &size);
	if (server < 0)
	{
		std::cout << "Error on accepting new user" << std::endl;
		exit(1);
	}
	if (getnameinfo((struct sockaddr *) &server_addr, sizeof(server_addr), host, NI_MAXHOST, NULL, 0, NI_NUMERICSERV) != 0)
	{
		std::cout << "Error on getting hostname new user" << std::endl;
		exit(1);
	}
	_users.push_back(User(server, host));
	pollfd pollfd = {server, POLLIN, 0};
	_pollfds.push_back(pollfd);
}

void	Server::printMsg(int fd){
	try
	{
		this->_cmd = splitCmd(readMsg(fd));
	}
	catch(const std::exception& e)
	{
		disconnectUser(fd);
		std::cerr << e.what() << '\n';
		return;
	}
	//std::cout << "DS printMsg AVANT parseCmd" << std::endl;
	for (std::vector<std::string>::iterator it = this->_cmd.begin(); it != this->_cmd.end(); it++)
	{
		//std::cout << "Ds for avant parseCmd" << std::endl;
		parseCmd(*it, findUser(fd));
		//std::cout << "apres parseCmd ds boucle for" << std::endl;
	}
	//std::cout << "apres boucle for" << std::endl;
	//displayUser();
}

//recv receive a message from a socket
std::string	Server::readMsg(int fd){
	std::string	msg;
	char		buffer[BUFFERSIZE];
	std::vector<User>::iterator user = findUserI(fd);
	bzero(buffer, BUFFERSIZE);
	msg = user->getMsg();

	while(!std::strstr(buffer, "\n"))
	{
		int valread;
		bzero(buffer, BUFFERSIZE);
		valread = recv(fd, buffer, BUFFERSIZE, 0);
		if (valread < 0)
		{
			std::cout << "Error No bytes are there to read" << std::endl;
			exit(1);
		}
		//int i = atoi(buffer);
		//std::cout << "->" << buffer << "<-FIN"<< " en nb= " << i<<std::endl;
		user->addMsg(buffer);
		msg += buffer;
		
	}
	user->setMsg("");
	return msg;
}

std::vector<std::string>	Server::splitCmd(std::string str)
{
	std::vector<std::string>	cmd;
	std::stringstream		is(str);
	std::string				tmp;
	int						i;

	i = 0;
	if (str == "\n")
		return cmd;
	while (std::getline(is, tmp))
	{
		cmd.push_back(tmp);
		std::cout << cmd.at(i++)<<std::endl;
	}
	return cmd;
}

void	Server::parseCmd(std::string str, User &user){
	//std::cout<< "DS PARSECMD"<<std::endl;
	std::vector<std::string>	cmds;
	std::stringstream			is(str);
	std::string					word;
	std::getline(is, word, ' ');
	
	cmds.push_back(word);
	std::cout<< "cmd=" << word << std::endl;

	std::string keyW[10] = {"NICK", "PASS", "USER", "JOIN", "KILL", "TOPIC", "KICK", "PART", "PING", "MODE"};
	int	(Server::*fun[10])(std::vector<std::string> cmds, User &user) = {
		&Server::cmdNick,
		&Server::cmdPw,
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
		if (keyW[i] == word)
		{
			while (std::getline(is, word, ' '))
				cmds.push_back(word);
			(this->*fun[i])(cmds, user);
		}
	}
	//std::cout << "Error "<< word << " doesn't exist" << std::endl;
}

void	Server::delUser(int fd){
	std::vector<User>::iterator i = _users.begin();

	while (i != _users.end())
	{
		if (i->getFd() == fd)
		{
			_users.erase(i);
			return ;
		}
		i++;
	}
}

void	Server::disconnectUser(int fd){
	std::vector<pollfd>::iterator i = _pollfds.begin();
	delUser(fd);
	while (i != _pollfds.end())
	{
		if (i->fd == fd)
		{
			_pollfds.erase(i);
			break;
		}
		i++;
	}
	close(fd);
	std::cout<< "Disconnection Successful" <<std::endl;
}

User		&Server::findUser(int fd){
	//std::cout<< "DS findUser by fd" <<std::endl;
	for (size_t i = 0; i < _users.size(); i++)
	{
		if (_users[i].getFd() == fd)
			return (_users[i]);
	}
	std::cout << "Error on searching client" << std::endl;
	exit(1);
}

User		&Server::findUser(std::string nickname){
	//std::cout<< "DS findUser by nickname" <<std::endl;
	for (size_t i = 0; i < _users.size(); i++)
	{
		if (_users[i].getNickname() == nickname)
			return (_users[i]);
	}
	std::cout << "Error on searching client" << std::endl;
	exit(1);
}

std::vector<User>::iterator	Server::findUserI(int fd){
	std::vector<User>::iterator begin = _users.begin();
	std::vector<User>::iterator end = _users.end();
	while (begin != end)
	{
		if (begin->getFd() == fd)
			return (begin);
		begin++;
	}
	std::cout << "Error on searching client" << std::endl;
	exit(1);
}

//.at() ex: std::cout << ' ' << myvector.at(i);
// at-> Returns a reference to the element at position n in the vector.

void	Server::displayUser(){
	size_t nb = _users.size();
	std::cout << "DS DISPLAY" << std::endl;
	std::cout <<"User " << nb << " is connected"<< std::endl;
	for(size_t i = 1; i <= nb; i++)
	{
		std::cout << "user " << i << std::endl;
		std::cout << " nickname: " << _users.at(i).getNickname() << std::endl;
		std::cout << " user: " <<_users.at(i).getUsername() << std::endl;
		std::cout << " realname: " <<_users.at(i).getRealname() << std::endl;
	}
	std::cout << std::endl;
}

std::vector<Channel>::iterator	Server::findChannelI(std::string name){
	std::vector<Channel>::iterator begin = _channels.begin();
	std::vector<Channel>::iterator end = _channels.end();
	while (begin != end)
	{
		if (begin->getName() == name)
			return (begin);
		begin++;
	}
	std::cout << "Error on searching channel" << std::endl;
	exit(1);
}

Channel	&Server::findChannel(std::string name){
	for (size_t i = 0; i < _channels.size(); i++)
	{
		if (_channels[i].getName() == name)
			return (_channels[i]);
	}
	std::cout << "Error on searching channel whith name's" << std::endl;
	exit(1);
}

void	Server::delChannel(User &user){
	std::vector<Channel>::iterator it = _channels.begin();
	for (size_t i = 0; i < _channels.size(); i++)
		_channels[i].delUser(user);
	while (it != _channels.end())
	{
		if (it->getUsers().empty())
			it = _channels.erase(it);
		else
			it++;
	}
	std::cout << "Delete channel ok" << std::endl;
}

bool	Server::isChannel(std::string str){
	for (size_t i = 0; i < _channels.size(); i++)
	{
		if (_channels[i].getName() == str)
			return true;
	}
	return false;
}
