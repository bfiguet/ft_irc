/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bfiguet <bfiguet@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/15 17:18:55 by bfiguet           #+#    #+#             */
/*   Updated: 2024/01/25 18:52:15 by bfiguet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Irc.hpp"

Channel::Channel(std::string name): _name(name), _fd(0), _pw(""), _topic(){}

Channel::~Channel(){}

std::string	Channel::getName()const{ return _name; }

std::string	Channel::getPw()const{ return _pw; }

int	Channel::getFd()const{ return _fd; }

std::vector<User>	&Channel::getUsers(){ return _users; }

std::vector<std::string>	&Channel::getOperators(){ return _operators; } //! Added to remove the compilation error

unsigned long	Channel::getLimit() const{ return this->_limit; }

bool	Channel::getInvRight()const{ return _inv_right; }

bool	Channel::getTopRight()const{ return _top_right; }

void	Channel::setInvRight(bool val){ this->_inv_right = val; }

void	Channel::setTopRight(bool val){ this->_top_right = val; }

void	Channel::setLimit(unsigned long val){ this->_limit = val; }

void	Channel::setFd(int fd){ _fd = fd; }

void	Channel::setTopic(std::string str){ _topic = str; }

void	Channel::setPw(std::string str){ _pw = str; }

void	Channel::setLimit(bool set, std::string word)
{
	if (set == 1)
	{
		char			*ptr;
		unsigned long	number = std::strtoul(word.c_str(), &ptr, 10);
		this->setLimit(number);
		if (number != '\0')
			std::cerr << "error: conversion from str to unsigned long failed" << std::endl;
	}
	else if (set == 0)
		this->delPw();
}

void	Channel::delPw()
{
	this->_pw.clear();
}

void	Channel::setPw(bool set, std::string word)
{
	if (set == 1)
		this->setPw(word);
	else if (set == 0)
		this->delPw();
}

void	Channel::addOperator(std::string username){ _operators.push_back(username); }

void	Channel::delOperator(std::string username)
{
	std::vector<std::string>::iterator	it;

	for (it = this->_operators.begin(); it != this->_operators.end(); it++)
	{
		if ((*it) == username)
		{
			_operators.erase(it);
			return ;
		}
	}
}

void	Channel::handleOperator(bool set, std::string word)
{
	if (set == 1)
		addOperator(word);
	else if (set == 0)
		delOperator(word);
}

void	Channel::addUser(User &user){ _users.push_back(user); }

void	Channel::delUser(User &user){
	std::vector<User>::iterator	it;

	for (it = _users.begin(); it != _users.end(); it++)
	{
		if (it->getFd() == user.getFd())
		{
			std::cout << " delete user " << user.getNickname() << std::endl;
			_users.erase(it);
			return ;
		}
	}
	std::cout << "error not find user " << user.getNickname() << " for delete him" << std::endl;
}

bool	Channel::checkRights(User &init) const
{//verifie le droits de l'user init
	if (init.getOperator() == 1)
		return true;
	else
	{
		for (unsigned long i = 0; i < _operators.size(); ++i)
		{
			if (_operators[i] == init.getUsername())
				return true;
		}
	}
	return false;
}

void	Channel::kickUser(User &init, User &receiv)
{//l'user init renvois un utilisateur (receiv)
	if (this->checkRights(init) == true)
	{
		;//KICK receiv if suceeded then delUser
		this->delUser(receiv);
	}
}

void	Channel::inviteUser(User &init, User &receiv)
{//l'user init invite un utilisateur (receiv)
	if ((this->getInvRight() == 1 && this->checkRights(init) == true)
		|| this->getInvRight() == 0)
	{
		;//Invite receiv if succeeded then addUser
		addUser(receiv);
	}
}

void	Channel::topicChange(User &init, std::string str)//Call this when /TOPIC
{//init change topic to str /TOPIC start of copy after 1 space
	if ((this->getTopRight() == 1 && this->checkRights(init) == true)
		|| this->getTopRight() == 0)
	{
		if (!str.empty())
			this->setTopic(str);
		else
			std::cout << this->_topic << std::endl;//temp
	}
}

//if the first character is '/' then it's a command
void	Channel::modeChange(User &init, std::string str)//Call this when /MODE
//assuming modeChange start here|      remove from str + identify channel first
//  usage: /MODE #<nom du canal>|<+/-itkol> <option>
{
	if (this->checkRights(init) == true)//redondant, deja gerer par les fonctions ulterieur
	{
		bool	set = 0;
		
		for (unsigned long i = 0; i < str.size(); i++)
		{
			switch (str[i])
			{
			case '+':
				set = 1;
				break;
			case '-':
				set = 0;
				break;
			case 'i':
				this->setInvRight(set);
				break;
			case 't':
				this->setTopRight(set);
				break;
			case 'k':
			{
				std::string word = this->wordRemoveExtract(str, i);
				this->setPw(set, word);
				break;
			}
			case 'o':
			{
				std::string word = this->wordRemoveExtract(str, i);
				this->handleOperator(set, word);
				break;
			}
			case 'l':
			{
				std::string word = this->wordRemoveExtract(str, i);
				this->setLimit(set, word);
				break;
			}
			}
		}
	}
}

//A chaque lecture d'un mode avec option il check le prochain "mot" si c'est pas bon il skip

//weird behavior for "/MODE +i l kk t" then "/MODE -i l kk t" t is not removed in second
//+ "/MODE +l trente k 10" -> do nothing whereas "/MODE +t l trente k 10" work as expected

std::string	Channel::wordRemoveExtract(std::string &str, unsigned long i)
{
	std::string	word;

	while (i < str.size() && str[i] != ' ')
		i++;
	i++;
	while (i < str.size() && str[i] != ' ')
	{
		word += str[i];
		str.erase(i, 1);
	}
	return (word);
}
