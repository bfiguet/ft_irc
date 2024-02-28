/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Irc.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aalkhiro <aalkhiro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/09 14:44:38 by bfiguet           #+#    #+#             */
/*   Updated: 2024/02/28 11:40:06 by aalkhiro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IRC_HPP
#define IRC_HPP

# include <cstring>
# include <cstdlib>
# include <string>
# include <sstream>
# include <errno.h>
# include <fcntl.h>
# include <fstream>
# include <iostream>
# include <poll.h>
# include <signal.h>
# include <sys/socket.h>
# include <sys/types.h>
# include <vector>
# include <netinet/in.h>
# include <netdb.h>
# include <limits>
# include <unistd.h>
# include <algorithm>
# include <sys/time.h>
# include "Server.hpp"
# include "User.hpp"
# include "Channel.hpp"
#define LOCAL_HOST "127.0.0.1"
#define BUFFERSIZE 1024
#define MAX_EVENTS 10

extern bool g_run;

#define SERVERNAME "ft_irc"
#define SERVER "127.0.0.1" 
#define VERSION "0.8"
#define DATE "Mon Feb 31 25:02:33 2023"
#define DEFAULT_NAME "pouet"
#define TIME_LIMIT 60

//RPL_MSG

# define RPL_WELCOME(nick, user, host)("001 " + nick + " :Welcome to the Internet Relay Network " + nick + "!" + user + "@" + host + "\r\n")

# define RPL_YOURHOST(localhost)(":" + localhost + " 002 :Your host is " + SERVER + ", running version " + VERSION + "\r\n")

# define RPL_CREATED(localhost)(":" + localhost + " 003 :This server was created " DATE "\r\n")

# define RPL_MYINFO(localhost)(":" + localhost + " 004 :" SERVERNAME " " VERSION ", group La Team \r\n")

# define RPL_NOTOPIC(nick, user, host, chan)("331 " + nick + "!" + user + "@" + host + " " + chan + " :No topic is set\r\n")

# define RPL_TOPIC(nick, user, host, chan, topic)("332 " + nick + "!" + user + "@" + host + " " + chan + " :" + topic + "\r\n")

# define RPL_INVITING(nick, user, host, invitenick, channel)("341 " + nick + "!" + user + "@" + host + " " + invitenick + " " + channel + "\r\n")

//>> :choopa.nj.us.dal.net 353 bland = #walk :bland @Jacques 
//>> :choopa.nj.us.dal.net 366 bland #walk :End of /NAMES list.
# define RPL_NAMREPLY(nick, user, host, channel, all_users)("353 " + nick + "!" + user + "@" + host + " = " + channel + " :" + all_users + "\r\n")

# define RPL_ENDOFNAMES(nick, user, host, channel)("366 " + nick + "!" + user + "@" + host + " " + channel + " :End of /NAMES list.\r\n")

# define RPL_CHANNELMODEIS(nick, user, host, channel, mode, param)("324 " + nick + "!" + user + "@" + host + " " + channel + " " + mode + param + " \r\n") 

//ERR_MSG

# define ERR_NOSUCHNICK(invitenick)("401 " + invitenick + " :No such nick/channel\r\n")

# define ERR_NOSUCHSERVER "402 ircserv :No such server\r\n"

# define ERR_NOSUCHCHANNEL(channel)("403 " + channel + " :No such channel\r\n")

# define ERR_CANNOTSENDTOCHAN(nick, chan)("404 " + nick + " " + chan + " :Cannot send to channel\r\n")

# define ERR_TOOMANYCHANNELS(nick, channel)("405 " + nick + " " + channel + " :You have joined too many channels\r\n")

# define ERR_NOORIGIN(nick)("409 " + nick + " :No origin specified\r\n")

# define ERR_NONICKNAMEGIVEN(localhost) (":" + localhost + " 431 :" + " :No nickname given\r\n")

# define ERR_ERRONEUSNICKNAME(localhost, nick) (":" + localhost + " 432 " + nick + " :Erroneus nickname\r\n")

# define ERR_NICKNAMEINUSE(nick)(": 433 localhost " + nick + " :Nickname is already in use\r\n")

# define ERR_USERNOTINCHANNEL(nick, chan)("441 " + nick + " " + chan + " :They aren't on that channel\r\n")

# define ERR_NOTONCHANNEL(chan)("442 " + chan + " :You're not on that channel\r\n")

# define ERR_USERONCHANNEL(nick, invitenick, chan)("443 " + nick + " " + invitenick + " " + chan + ":is already on channel\r\n")

# define ERR_NEEDMOREPARAMS(command)("461 " + command + " :Not enough parameters\r\n")

# define ERR_ALREADYREGISTERED "462 :You may not reregister\r\n"

# define ERR_PASSWDMISMATCH "464 :Password incorrect\r\n"

# define ERR_USERMISMATCH "464 : User incorrect\r\n"

# define ERR_KEYSET(channel) (": 467 " + channel + " :Channel key already set\r\n")

# define ERR_CHANNELISFULL(nick, channel)("471 " + nick + " " + channel + " :Cannot join channel (+l)\r\n")

# define ERR_UNKNOWNMODE(modechar)(": 472 " + modechar + " :is unknown mode char to me\r\n")

# define ERR_INVITEONLYCHAN(nick, chan)("473 " + nick + " " + chan + " :Cannot join channel (+i)\r\n")

# define ERR_BADCHANNELKEY(nick, channel)("475 " + nick + " " + channel + " :Cannot join channel (+k)\r\n")

# define ERR_BADCHANMASK(channel)("476 " + channel + " :Bad Channel Mask\r\n")

# define ERR_NOPRIVILEGES "481 :Permission Denied- You're not an IRC operator\r\n"

# define ERR_CHANOPRIVSNEEDED(chan, nick)("482 " + nick + " " + chan + " :You're not channel operator\r\n")

# define ERR_INVALIDKEY(channel)("525 " + channel + " :Key is not well-formed\r\n")

//OTHER_MSG

# define PONG(localhost, msg)("PONG " + localhost + ": " + msg + "\r\n")

# define NICK(nick)("Your nickname is : " + nick + "\n")

# define NICK_CHANGE(old_nick, nick)(":" + old_nick + " NICK " + nick + "\r\n")

# define JOIN(nick, user, host, chan)(":" + nick + "!" + user + "@" + host  + " JOIN " + chan + "\r\n")

# define PART(nick, user, host, chan, reason)(":" + nick + "!" + user + "@" + host + " PART " + chan + " " + reason + "\r\n")

# define KILL(nick, user, host, nickToKill, msg)(":" + nick + "!" + user + "@" + host + " KILL " + nickToKill + " " + msg + "\r\n")

# define KICK(nick, user, host, chan, kicked, reason)(":" + nick + "!" + user + "@" + host + " KICK " + chan + " " + kicked + " :" + reason + "\r\n")

# define INVITE(nick, user, host, nickinvite, channel)(":" + nick + "!" + user + "@" + host + " INVITE " + nickinvite + " " + channel + "\r\n")

# define TOPIC(nick, user, host, chan, topic)(":" + nick + "!" + user + "@" + host + " TOPIC " + chan + " :" + topic + "\r\n")

# define MSG(nick, username, host, cmd, dest_name, msg)(":" + nick + "!" + username + "@" + host + " " + cmd + " " + dest_name + " :" + msg + "\r\n")

# define MODE(nick, channel, mode, param) (":" + nick + "!" + "@localhost MODE " + channel + " " + mode + " " + param + "\r\n")

#endif
