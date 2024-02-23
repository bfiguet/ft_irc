/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bfiguet <bfiguet@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/15 17:18:55 by bfiguet           #+#    #+#             */
/*   Updated: 2024/02/23 14:55:53 by bfiguet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Irc.hpp"

Channel::Channel(std::string name): _name(name), _pw(""), _topic(""), _userCount(0)
	, _userLimit(100), _invitOnly(false), _TopicChangeRestriction(false){}

Channel::~Channel(){}

std::string	Channel::getName()const
{return _name;}

std::string	Channel::getPw()const
{return _pw;}

std::vector<User *>	Channel::getUsers() const
{return _users;}

std::string	Channel::getTopic() const{ return _topic;}

bool	Channel::isInvited(const User* user) const
{ return (std::find(_invited.begin(), _invited.end(), user) != _invited.end());}

int	Channel::getUserCount() const
{return (_userCount);}

int	Channel::getLimit() const
{return (_userLimit);}

bool	Channel::isInvitOnly()const
{return (_invitOnly);}

bool	Channel::isTopicChange()const
{return _TopicChangeRestriction;}

bool	Channel::isInChannel(const User* user)
{
	return (std::find(_users.begin(), _users.end(), user) != _users.end());
}

bool	Channel::isOperator(const User* user) const
{
	return(std::find(_operators.begin(), _operators.end(), user) != _operators.end());
}

//Channels names beginning with a '&', '#'
// length up to 200 char whithout the first char
// no coma space or ASCII 7 or :
bool	Channel::isValidName(std::string name){
	if (name.length() > 201 || !name.length())
		return false;
	if (name[0] != '#' && name[0] != '&')
		return false;
	if ((name.find(',') != std::string::npos) || name.find(' ') != std::string::npos 
		|| name.find(7) != std::string::npos || name.find(':') != std::string::npos)
		return false;
	return true;
}

void	Channel::setTopic(std::string topic)
{ _topic = topic;}

void	Channel::setPw(std::string passWord)
{_pw = passWord;}

void	Channel::setLimit(int userLimit)
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
	_userCount--;
	_users.erase(std::find(_users.begin(), _users.end(), user));
}

void	Channel::setInviteUser(User* user, bool onoff)
{
	if (onoff == true)
		_invited.push_back(user);
	else if (isInvited(user) == true)
		_invited.erase(std::find(_invited.begin(), _invited.end(), user));
}

void	Channel::setOperators(User* user, bool onoff)
{
	if (onoff == true)
		_operators.push_back(user);
	else if (isOperator(user) == true)
		_operators.erase(std::find(_operators.begin(), _operators.end(), user));
}
