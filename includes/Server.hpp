/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bfiguet <bfiguet@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/09 15:56:39 by bfiguet           #+#    #+#             */
/*   Updated: 2024/01/25 18:53:02 by bfiguet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include "Irc.hpp"

class Server
{
private:
	std::string 				_host;
	std::string 				_pw;
	int							_port;
	int							_sock;
	std::vector<pollfd>			_pollfds;
	std::vector<User>			_users;
	std::vector<std::string>	_cmd;
	std::vector<Channel>		_channels;

	int								cmdPw(std::vector<std::string> str, User &user);
	int								cmdNick(std::vector<std::string> str, User &user);
	int								cmdUser(std::vector<std::string> str, User &user);
	int								cmdPing(std::vector<std::string> str, User &user);
	int								cmdJoin(std::vector<std::string> str, User &user);
	int								cmdPart(std::vector<std::string> str, User &user);
	int								cmdMode(std::vector<std::string> str, User &user);
	int								cmdKick(std::vector<std::string> str, User &user);
	int								cmdTopic(std::vector<std::string> str, User &user);
	int								cmdKill(std::vector<std::string> str, User &user);
	
public:
	Server(int port, const std::string &pw);
	~Server();
	void							start();
	int								newSock();
	void							newUser();
	void							delUser(int fd);
	void							disconnectUser(int fd);
	void							printMsg(int fd);
	std::string						readMsg(int fd);
	std::vector<std::string>		splitCmd(std::string str);
	void							parseCmd(std::string str, User &user);
	User							&findUser(int fd);
	User							&findUser(std::string nickname);
	std::vector<User>::iterator		findUserI(int fd);
	void							displayUser();
	std::vector<Channel>::iterator	findChannelI(std::string name);
	Channel							&findChannel(std::string name);
	void							delChannel(User &user);
	bool							isChannel(std::string str);

};

#endif
