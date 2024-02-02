/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aalkhiro <aalkhiro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/09 14:51:01 by bfiguet           #+#    #+#             */
/*   Updated: 2024/02/02 10:50:07 by aalkhiro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Irc.hpp"

bool	g_run = true;

void	ft_signal(int sig)
{
	(void)sig;
	g_run = false;
	signal(sig, SIG_DFL); // permet de fermer le programme en rappuyant sur ctrl+c
	//ne permet pas de close proprement les fds ds server.start()
	std::cout << "\b\b"; // Supprime le ctrl+c du terminal
	//std::cout << "Press ctrl+c once again to close the server" << std::endl;
}

bool	verifPort(std::string av, int &port)
{
	port = std::atoi(av.c_str());
	if (av.empty() || !av.find_first_not_of("0123456789") || port < 1 || port > 65535)
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
		server.start();
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
	return 0;
}
