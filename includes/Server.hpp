/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aalkhiro <aalkhiro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/09 15:56:39 by bfiguet           #+#    #+#             */
/*   Updated: 2024/02/23 14:56:33 by aalkhiro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include "Irc.hpp"

class Server
{
private:
	std::string 			_host;
	std::string 			_pw;
	int						_port;
	int						_sock;
	std::vector<pollfd>		_pollfds;
	std::vector<User *>		_users;
	std::vector<Channel *>	_channels;

	int						pollinHandler(int fd);
	int						polloutHandler(int fd);
	int						pollerrHandler(int fd);

public:
	Server(int port, const std::string &pw);
	~Server();
	
	std::vector<User *>		getUsers() const;
	std::string				getHost()const;
	std::string				getPw()const;
	int						start();
	int						newSock();
	int						newUser();
	std::vector<User*>::iterator	delUser(User* user);
	void					deleteDisconnected();
	void					deleteEmptyChannels();
	int						receiveMsg(int fd);
	void					executeCmd(std::string str, User* user);
	User*					findUser(int fd);
	User*					findUser(std::string nickname);
	void					displayUser(User* user);
	Channel*				findChannel(std::string str);
	void					deleteUserFromChannels(User* user);
	int						callCmds(User* user);
	void					addChannel(std::string name);
	void					checkTimeout();

	class BadServInit: public std::exception
    {
        public:
        virtual const char* what() const throw();
    };
};

// void					deleteEmptyChannel(std::vector<Channel*>& _channels);
	typedef int				(*fun)(Server* server, std::vector<std::string> str, User *user);
	int						cmdPass(Server *server, std::vector<std::string> str, User *user);
	int						cmdUser(Server *server, std::vector<std::string> str, User *user);
	int						cmdNick(Server *server, std::vector<std::string> str, User *user);
	int						cmdPing(Server *server, std::vector<std::string> str, User *user);
	int						cmdInvite(Server *server, std::vector<std::string> str, User *user);
	int						cmdPart(Server *server, std::vector<std::string> str, User *user);
	int						cmdMode(Server *server, std::vector<std::string> str, User *user);
	int						cmdKick(Server *server, std::vector<std::string> str, User *user);
	int						cmdTopic(Server *server, std::vector<std::string> str, User *user);
	int						cmdKill(Server *server, std::vector<std::string> str, User *user);
	int						cmdQuit(Server *server, std::vector<std::string> str, User *user);
	int						cmdJoin(Server *server, std::vector<std::string> str, User *user);
	int						cmdPrivmsg(Server *server, std::vector<std::string> str, User *user);

#endif
