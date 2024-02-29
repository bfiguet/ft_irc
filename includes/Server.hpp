/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aalkhiro <aalkhiro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/09 15:56:39 by bfiguet           #+#    #+#             */
/*   Updated: 2024/02/29 12:35:03 by aalkhiro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include "Irc.hpp"

class Server
{
private:
	int						_port;
	int						_sock;
	ServerData*				_data;

	int						pollinHandler(int fd);
	int						polloutHandler(int fd);
	int						pollerrHandler(int fd);
	int						newSock();
	int						receiveMsg(int fd);
	void					executeCmd(std::string str, User* user);
	int						callCmds(User* user);
	void					checkTimeout();

public:
	Server(int port, const std::string &pw);
	~Server();
	int						start();
	

	class BadServInit: public std::exception
    {
        public:
        virtual const char* what() const throw();
    };
};

	typedef int				(*fun)(ServerData* serverData, std::vector<std::string> args, User *user);
	int						cmdPass(ServerData *serverData, std::vector<std::string> args, User *user);
	int						cmdUser(ServerData *serverData, std::vector<std::string> args, User *user);
	int						cmdNick(ServerData *serverData, std::vector<std::string> args, User *user);
	int						cmdPing(ServerData *serverData, std::vector<std::string> args, User *user);
	int						cmdInvite(ServerData *serverData, std::vector<std::string> args, User *user);
	int						cmdPart(ServerData *serverData, std::vector<std::string> args, User *user);
	int						cmdMode(ServerData *serverData, std::vector<std::string> args, User *user);
	int						cmdKick(ServerData *serverData, std::vector<std::string> args, User *user);
	int						cmdTopic(ServerData *serverData, std::vector<std::string> args, User *user);
	int						cmdKill(ServerData *serverData, std::vector<std::string> args, User *user);
	int						cmdQuit(ServerData *serverData, std::vector<std::string> args, User *user);
	int						cmdJoin(ServerData *serverData, std::vector<std::string> args, User *user);
	int						cmdPrivmsg(ServerData *serverData, std::vector<std::string> args, User *user);
	std::vector<std::string>	ft_split(std::string str, char delimiter);
	std::string 			joinArgs(size_t i, std::vector<std::string> args, char add);
	int						errMsg(std::string msg);
#endif
