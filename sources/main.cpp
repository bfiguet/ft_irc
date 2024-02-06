/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bfiguet <bfiguet@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/09 14:51:01 by bfiguet           #+#    #+#             */
/*   Updated: 2024/02/06 19:04:04 by bfiguet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Irc.hpp"

bool	g_run = true;

void	ft_signal(int sig)
{
	(void)sig;
	g_run = false;
	std::cout << "\b\b"; 
}

bool	verifPort(std::string av, int &port)
{
	port = std::atoi(av.c_str());
	if (av.empty() || isdigit(port) || port < 1 || port > 65535)
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
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
	return 0;
}
