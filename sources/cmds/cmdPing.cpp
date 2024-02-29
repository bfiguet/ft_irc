/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmdPing.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aalkhiro <aalkhiro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/16 15:49:38 by bfiguet           #+#    #+#             */
/*   Updated: 2024/02/29 12:33:24 by aalkhiro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Irc.hpp"

//Command: PING <token>
int	cmdPing(ServerData *serverData, std::vector<std::string> args, User *user){
	(void)serverData;

	if (args[1].size() == 0)
	{
		user->addMsgToSend(ERR_NOORIGIN(user->getNick()));
		return 1;
	}
	user->addMsgToSend(PONG(user->getHost(), args[1]));
	return 0;
}
