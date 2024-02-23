/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmdNick.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bfiguet <bfiguet@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/16 15:46:13 by bfiguet           #+#    #+#             */
/*   Updated: 2024/02/23 14:44:55 by bfiguet          ###   ########.fr       */
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
			|| str[i] == ']' || str[i] == '|' || str[i] == '\\' || str[i] == '_')
			i++;
		else
			return false;
	}
	return true;
}

//Command: NICK <nickname>
int	cmdNick(Server *server, std::vector<std::string> str, User *user){
	std::vector<User*>	listUser = server->getUsers();
	std::cout << "cmdNick " << str[1] << std::endl;

	for (std::vector<User*>::iterator i = listUser.begin(); i != listUser.end(); i++)
    {
        if ((*i)->getNick().compare(str[1]) == 0)
        {
			user->addMsgToSend(ERR_NICKNAMEINUSE(str[1]));
        	return 1;
		}
	}
	if (checkNick(str[1]) == false)
	{
		user->addMsgToSend(ERR_ERRONEUSNICKNAME(user->getHost(), user->getNick()));
			return 1;
	}
	if (user->getNick() == "")
	{
		user->addMsgToSend(NICK(str[1]));
	}
	else
		user->addMsgToSend(NICK_CHANGE(user->getNick(), str[1]));
	user->setNick(str[1]);
	return 0;
}
