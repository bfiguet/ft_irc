/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bfiguet <bfiguet@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/15 17:12:34 by bfiguet           #+#    #+#             */
/*   Updated: 2024/01/22 16:13:31 by bfiguet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include "Irc.hpp"

class Channel
{
private:
	unsigned long				_limit;
	bool						_inv_right;
	bool						_top_right;
	std::string					_name;
	int							_fd;
	std::string					_pw;
	std::string					_topic;
	std::vector<User>			_users;
	std::vector<std::string>	_operators;//regroupe les username des operator pour le channel

public:
	Channel(std::string name);
	~Channel();

	std::string					getName()const;
	std::string					getPw()const;
	int							getFd()const;
	std::vector<User>			&getUsers();
	std::vector<std::string>	&getOperators();
	unsigned long				getLimit()const;
	bool						getInvRight()const;
	bool						getTopRight()const;

	void						setFd(int fd);
	void						setTopic(std::string str);
	void						setPw(std::string str);
	void						setPw(bool set, std::string word);
	void						setLimit(unsigned long val);
	void						setLimit(bool set, std::string word);
	void						setInvRight(bool val);
	void						setTopRight(bool val);

	void						addUser(User &user);
	void						delUser(User &user);
	void						kickUser(User &init, User &receiv);
	void						inviteUser(User &init, User &receiv);
	void						delPw();
	void						topicChange(User &init, std::string str);
	void						modeChange(User &init, std::string str);
	void						addOperator(std::string username);
	void						delOperator(std::string username);
	void						handleOperator(bool set, std::string word);
	bool						checkRights(User &init)const;
	std::string					wordRemoveExtract(std::string &str, unsigned long i);
};

#endif
