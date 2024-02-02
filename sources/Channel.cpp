/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bfiguet <bfiguet@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/15 17:18:55 by bfiguet           #+#    #+#             */
/*   Updated: 2024/02/02 18:34:07 by bfiguet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Irc.hpp"

Channel::Channel(std::string name): _name(name), _pw(""), _topic(){}

Channel::~Channel(){}

std::string	Channel::getName()const
{return _name;}

std::string	Channel::getPw()const
{return _pw;}

std::vector<User *>	Channel::getUsers() const
{return _users;}

bool	Channel::isOperator(const User* user) const
{ return (std::find(_operators.begin(), _operators.end(), user) != _operators.end());}

bool	Channel::isInvited(const User* user) const
{ return (std::find(_invited.begin(), _invited.end(), user) != _invited.end());}

unsigned long	Channel::getUserCount() const
{return (_userCount);}

bool	Channel::isInvitOnly()const
{return (_invitOnly);}

bool	Channel::isTopicChange()const
{return _TopicChangeRestriction;}

bool	Channel::isInChannel(const User* user)
{
	return (std::find(_users.begin(), _users.end(), user) != _users.end());
}

void	Channel::setTopic(std::string topic)
{ _topic = topic;}

void	Channel::setPw(std::string passWord)
{_pw = passWord;}

void	Channel::setLimit(unsigned long userLimit)
{_userLimit = userLimit;}

void	Channel::setInvitOnly(bool onOff)
{_invitOnly = onOff;}

void	Channel::setTopicChange(bool onOff)
{_TopicChangeRestriction = onOff;}

void	Channel::addUser(User* user)
{
	_users.push_back(user);
	_userCount++;
}

void	Channel::delUser(User* user)
{
	_users.erase(std::find(_users.begin(), _users.end(), user));
	_userCount--;
}

void	Channel::inviteUser(User* user)
{_invited.push_back(user);}

void	Channel::setOperator(User* user, bool isOperator)
{
	if (isOperator)
		_operators.push_back(user);
	else
		_operators.erase(std::find(_operators.begin(), _operators.end(), user));
}
