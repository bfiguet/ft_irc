/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmdUser.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bfiguet <bfiguet@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/16 15:48:25 by bfiguet           #+#    #+#             */
/*   Updated: 2024/02/16 18:07:15 by bfiguet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Irc.hpp"

//Command: USER <username> 0 * <realname>
int	cmdUser(Server *server, std::vector<std::string> str, User *user){
	(void)server;
	std::string	tmp;

	if (user->isRegisterd())
	{
		user->addMsgToSend(ERR_ALREADYREGISTERED);
		return 1;
	}
	else if (str.size() < 4)
	{
		user->addMsgToSend(ERR_NEEDMOREPARAMS(str[0]));
		return 1;
	}
	else if (str.size() >= 4)
	{
		user->setUser(str[1]);
		user->setHost(str[3]);
		if (str[4][0] == ':')
		{
			tmp = str[4].substr(1);
			user->setRealname(tmp);
		}
		tmp += " ";
		for (size_t i = 5; i < str.size(); i++)
		{
			tmp += " ";
			tmp += str[i];
		}
		user->setRealname(tmp);
	}
	return 0;
}
