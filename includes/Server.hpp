/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aalkhiro <aalkhiro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/09 15:56:39 by bfiguet           #+#    #+#             */
/*   Updated: 2024/02/02 13:04:10 by aalkhiro         ###   ########.fr       */
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
	std::vector<User *>			_users;
	std::vector<std::string>	_cmd;
	std::vector<Channel *>		_channels;

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
	void							delUser(User*);
	void							disconnectUser(User* user);
	void							receiveMsg(int fd);
	std::string						readMsg(int fd);
	// std::vector<std::string>		splitCmd(std::string str);
	void							executeCMD(std::string str, User* user);
	User*							findUser(int fd);
	User*							findUser(std::string nickname);
	std::vector<User>::iterator		findUserI(int fd);
	void							displayUser(User* user);
	Channel*						findChannel(std::string str);
	void							deleteUserFromChannels(User* user);
	bool							isChannel(std::string str);

};

#endif
