/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aalkhiro <aalkhiro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/09 15:56:39 by bfiguet           #+#    #+#             */
/*   Updated: 2024/02/06 12:48:01 by aalkhiro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include "Irc.hpp"

typedef int(*fun)(std::vector<std::string> arguments, User *user);

class Server
{
private:
	std::string 				_host;
	std::string 				_pw;
	int							_port;
	int							_sock;
	std::vector<pollfd>			_pollfds;
	std::vector<User *>			_users;
	std::vector<std::string>	_cmd;
	std::vector<Channel *>		_channels;

	int								cmdPass(std::vector<std::string> str, User *user);
	int								cmdUser(std::vector<std::string> str, User *user);
	int								cmdNick(std::vector<std::string> str, User *user);
	int								cmdPing(std::vector<std::string> str, User *user);
	int								cmdJoin(std::vector<std::string> str, User *user);
	int								cmdPart(std::vector<std::string> str, User *user);
	int								cmdMode(std::vector<std::string> str, User *user);
	int								cmdKick(std::vector<std::string> str, User *user);
	int								cmdTopic(std::vector<std::string> str, User *user);
	int								cmdKill(std::vector<std::string> str, User *user);
	int								pollinHandler(int fd);
	int								polloutHandler(int fd);
	int								pollerrHandler(int fd);

public:
	Server(int port, const std::string &pw);
	~Server();
	int								start();
	int								newSock();
	int								newUser();
	void							delUser(User*);
	void							disconnectUser(User* user);
	int								receiveMsg(int fd);
	void							executeCmd(std::string str, User* user);
	User*							findUser(int fd);
	User*							findUser(std::string nickname);
	std::vector<User>::iterator		findUserI(int fd);
	void							displayUser(User* user);
	Channel*						findChannel(std::string str);
	void							deleteUserFromChannels(User* user);
	bool							isChannel(std::string str);

};

#endif
