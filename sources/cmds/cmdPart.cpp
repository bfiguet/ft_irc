/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmdPart.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aalkhiro <aalkhiro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/16 15:50:31 by bfiguet           #+#    #+#             */
/*   Updated: 2024/02/21 10:54:53 by aalkhiro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Irc.hpp"

//Command: PART <channel> <reason>
int	cmdPart(Server *server, std::vector<std::string> str, User *user)
{
	//std::cout << "--cmdPart--" << std::endl;
	std::string	reason = "";

	if (str.size() < 2)
	{
		user->addMsgToSend(ERR_NEEDMOREPARAMS(str[0]));
		return 1;
	}
	if (server->findChannel(str[1]) == NULL)
	{
		user->addMsgToSend(ERR_NOSUCHCHANNEL(str[1]));
		return 1;
	}
	if (server->findChannel(str[1])->isInChannel(user) == false)
	{
		user->addMsgToSend(ERR_NOTONCHANNEL(str[1]));
		return 1;
	}
	if (str.size() > 1)
		for (size_t i = 2; i < str.size(); i++)
		{
			reason += " ";
			reason += str[i];
		}
	// std::cout << "debug: msg to send" << std::endl;
	user->addMsgToSend(PART(user->getNick(), user->getUser(), user->getHost(), str[1], reason));
	// std::cout << "debug: msg sent" << std::endl;
	server->findChannel(str[1])->delUser(user);
	// std::cout << "debug: user removed from channel" << std::endl;
	return 0;
}
