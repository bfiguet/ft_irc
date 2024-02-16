/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmdNick.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bfiguet <bfiguet@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/16 15:46:13 by bfiguet           #+#    #+#             */
/*   Updated: 2024/02/16 15:49:07 by bfiguet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Irc.hpp"

//alphanimeric {} [] \ | ok
// no space 
// no : and no num at first
bool	checkNick(std::string str){
	
	int	i = 0;
	if (str[i] == ':' || str[i] == '#' || isdigit(str[i]))
		return false;
	while (str[i])
	{
		if (isalnum(str[i]) || str[i] == '{' || str[i] == '}' || str[i] == '['
			|| str[i] == ']' || str[i] == '|' || str[i] == '\\')
			i++;
		else
			return false;
	}
	return true;
}

//Command: NICK <nickname>
int	cmdNick(Server *server, std::vector<std::string> str, User *user){
	std::vector<User*>	listUser = server->getUsers();

	if (str.size() < 2)
	{
		user->addMsgToSend(ERR_NONICKNAMEGIVEN);
		return 1;
	}
	for (std::vector<User*>::iterator i = listUser.begin(); i != listUser.end(); i++)
    {
        if ((*i)->getNick().compare(str[1]) == 0)
        {
			user->addMsgToSend(ERR_NICKNAMEINUSE(user->getNick()));
        	return 1;
		}
	}
	if (checkNick(str[1]) == false)
	{
		user->addMsgToSend(ERR_ERRONEUSNICKNAME(user->getNick()));
			return 1;
	}
	if (user->getNick() == "")
	{
		user->sendMsg(NICK(str[1]));
	}
	else
		user->addMsgToSend(NICK_CHANGE(user->getNick(), str[1]));
	user->setNick(str[1]);
	return 0;
}
