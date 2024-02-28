/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmdPing.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bfiguet <bfiguet@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/16 15:49:38 by bfiguet           #+#    #+#             */
/*   Updated: 2024/02/28 13:42:25 by bfiguet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Irc.hpp"

//Command: PING <token>
int	cmdPing(Server *server, std::vector<std::string> args, User *user){
	(void)server;

	if (args[1].size() == 0)
	{
		user->addMsgToSend(ERR_NOORIGIN(user->getNick()));
		return 1;
	}
	user->addMsgToSend(PONG(user->getHost(), args[1]));
	return 0;
}
