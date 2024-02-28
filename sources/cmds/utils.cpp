/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aalkhiro <aalkhiro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/28 15:12:06 by aalkhiro          #+#    #+#             */
/*   Updated: 2024/02/28 15:12:59 by aalkhiro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Irc.hpp"

int	errMsg(std::string msg)
{
	std::cerr <<  msg << std::endl;
	return (-1);
}

std::string joinArgs(size_t i, std::vector<std::string> args, char add)
{
	std::string str = "";
	for (; i < args.size(); i++)
	{
		str += add;
		str += args[i];
	}
	return str;
}

std::vector<std::string>	ft_split(std::string str, char delimiter)
{
	std::string					temp;
	std::vector<std::string>	strs;

	while (!str.empty())
	{
		temp = str.substr(0, str.find(delimiter));
		strs.push_back(temp);
		if (temp.size() == str.size())
		    str = "";
		else
    		str = str.substr(temp.size() + 1, str.size() - temp.size());
		std::cout << "debug: splitting result:" << temp << std::endl;
	}
	return (strs);
}