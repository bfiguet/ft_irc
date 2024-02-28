/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmdUser.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aalkhiro <aalkhiro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/16 15:48:25 by bfiguet           #+#    #+#             */
/*   Updated: 2024/02/28 14:18:16 by aalkhiro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Irc.hpp"

//Command: USER <username> 0 * <realname>
int	cmdUser(Server *server, std::vector<std::string> args, User *user){
	(void)server;
	std::string	tmp;

	if (user->isRegisterd())
	{
		user->addMsgToSend(ERR_ALREADYREGISTERED);
		return 1;
	}
	else if (args.size() < 4)
	{
		user->addMsgToSend(ERR_NEEDMOREPARAMS(args[0]));
		return 1;
	}
	else if (args.size() >= 4)
	{
		user->setUser(args[1]);
		user->setHost(args[3]);
		if (args[4][0] == ':')
		{
			if (args[4].size() > 2)
				tmp = args[4].substr(1);
		}
		if (args.size() > 5)
		{
			tmp += " ";
			tmp += joinArgs(5, args, ' ');
		}
		user->setRealname(tmp);
	}
	return 0;
}
