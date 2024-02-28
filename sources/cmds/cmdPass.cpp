/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmdPass.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bfiguet <bfiguet@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/16 15:47:42 by bfiguet           #+#    #+#             */
/*   Updated: 2024/02/28 13:42:12 by bfiguet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Irc.hpp"

//Command: PASS <password>
int	cmdPass(Server *server, std::vector<std::string> args, User *user){
	
	if (args.size() < 2)
	{
		user->addMsgToSend(ERR_NEEDMOREPARAMS(args[0]));
		return 1;
	}
	if (args[1] != server->getPw())
	{
		user->addMsgToSend(ERR_PASSWDMISMATCH);
		return 1;
	}
	user->setPass(args[1]);
	return 0;
}
