/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bfiguet <bfiguet@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/15 17:12:34 by bfiguet           #+#    #+#             */
/*   Updated: 2024/02/28 12:04:12 by bfiguet          ###   ########.fr       */
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
	std::string				_topic;
	int						_userCount;
	int						_userLimit;
	bool					_invitOnly;
	bool					_TopicChangeRestriction;
	bool					_isLimited;
	std::vector<User *>		_users;
	std::vector<User *>		_operators;
	std::vector<User *>		_invited;

public:
	Channel(std::string name);
	~Channel();

	std::string				getName()const;
	std::string				getPw() const;
	std::vector<User *>		getUsers() const;
	int						getUserCount() const;
	int						getLimit() const;
	std::string				getTopic() const;
	std::string				listNames();

	bool					isOperator(const User* user) const;
	bool					isInvited(const User* user) const;
	bool					isInvitOnly()const;
	bool					isTopicUnprotected()const;
	bool					isInChannel(const User* user);
	bool					isValidName(std::string name);
	bool					isLimited()const;

	void					setTopic(std::string str);
	void					setPw(std::string str);
	void					setInvitOnly(bool val);
	void					setTopicChange(bool val);
	void					setInviteUser(User* user, bool onoff);
	void					setOperators(User* user, bool onoff);
	void					setIsLimited(int nb, bool onOff);

	void					addUser(User* user);
	void					delUser(User* user);
	void					broadcast(std::string msg);
};

#endif
