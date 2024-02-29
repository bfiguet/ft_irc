/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmdNick.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aalkhiro <aalkhiro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/16 15:46:13 by bfiguet           #+#    #+#             */
/*   Updated: 2024/02/29 14:53:01 by aalkhiro         ###   ########.fr       */
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
int	cmdNick(ServerData *serverData, std::vector<std::string> args, User *user){
	std::vector<User*>	listUser = serverData->getUsers();
	if (args.size() < 2)
	{
		user->addMsgToSend(ERR_NEEDMOREPARAMS(args[0]));
		return 1;
	}
	for (std::vector<User*>::iterator i = listUser.begin(); i != listUser.end(); i++)
    {
        if ((*i)->getNick().compare(args[1]) == 0)
        {
			user->addMsgToSend(ERR_NICKNAMEINUSE(args[1]));
        	return 1;
		}
	}
	if (checkNick(args[1]) == false)
	{
		user->addMsgToSend(ERR_ERRONEUSNICKNAME(user->getHost(), user->getNick()));
			return 1;
	}
	if (user->getNick() == "")
		user->addMsgToSend(NICK(args[1]));
	else
		user->addMsgToSend(NICK_CHANGE(user->getNick(), args[1]));
	user->setNick(args[1]);
	return 0;
}
