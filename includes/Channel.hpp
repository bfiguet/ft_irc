/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bfiguet <bfiguet@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/15 17:12:34 by bfiguet           #+#    #+#             */
/*   Updated: 2024/02/14 11:47:18 by bfiguet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include "Irc.hpp"

class Channel
{
private:
	std::string				_name;
	std::string				_pw;
	std::string				_operatorPw;
	std::string				_topic;
	//unsigned long			_userCount;
	//unsigned long			_userLimit;
	int						_userCount;
	int						_userLimit;
	bool					_invitOnly;
	bool					_TopicChangeRestriction;
	std::vector<User *>		_users;
	std::vector<User *>		_operators;
	std::vector<User *>		_invited;

public:
	Channel(std::string name);
	~Channel();

	std::string				getName()const;
	std::string				getPw() const;
	std::vector<User *>		getUsers() const;
	//unsigned long			getUserCount() const;
	int						getUserCount() const;
	std::string				getTopic() const;

	bool					isOperator(const User* user) const;
	bool					isInvited(const User* user) const;
	bool					isInvitOnly()const;
	bool					isTopicChange()const;
	bool					isInChannel(const User* user);
	bool					isValidName(std::string name);

	void					setTopic(std::string str);
	void					setPw(std::string str);
	void					setLimit(unsigned long val);
	void					setInvitOnly(bool val);
	void					setTopicChange(bool val);

	void					addUser(User* user);
	void					delUser(User* user);
	void					inviteUser(User* user);
	void					setOperators(User* user, bool isOperator);
};

#endif
