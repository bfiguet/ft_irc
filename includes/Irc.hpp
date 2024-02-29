/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Irc.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aalkhiro <aalkhiro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/09 14:44:38 by bfiguet           #+#    #+#             */
/*   Updated: 2024/02/29 13:35:30 by aalkhiro         ###   ########.fr       */
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
# include "NumericRPLs.hpp"
# include "User.hpp"
# include "Channel.hpp"
# include "ServerData.hpp"
# include "Server.hpp"
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

#endif
