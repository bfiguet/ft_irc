/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bfiguet <bfiguet@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/09 14:51:01 by bfiguet           #+#    #+#             */
/*   Updated: 2024/02/28 15:16:03 by bfiguet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Irc.hpp"

bool	g_run = true;

void	ft_signal(int sig)
{
	(void)sig;
	g_run = false;
}

bool	verifPort(std::string av, int &port)
{
	port = std::atoi(av.c_str());
	std::cout << "port " <<  port << std::endl;
	if (av.empty() || port < 1 || port > 65535)
	{
		std::cout << "error: " << std::endl << "port must be a number between 1 and 65535" << std::endl;
		return true;
	}
	return false;
}

int main(int ac, char **av)
{
	int port;

	signal(SIGINT, ft_signal);
	if (ac != 3)
	{
		std::cout << "error: " << std::endl << "Usage: ./ircserv <port> <password>" << std::endl;
		return 1;
	}
	if (verifPort(av[1], port))
		return 1;
	try
	{
		Server server(port, av[2]);
		if (server.start())
			return (1);
	}
	catch(std::exception& e)
	{
		std::cerr << e.what() << '\n';
		return (1);
	}
	return 0;
}
