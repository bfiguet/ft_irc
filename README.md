# ft_irc
## IRC Server
Internet Relay Chat Server we're created in C++ 98 standard.

Any external library and Boost libraries are forbidden.

IRC is a protocol for real-time messaging and communication in a distributed network environment.

![irc](https://github.com/user-attachments/assets/5eff403c-6c25-4e69-8cfe-c704e7ba86de)

### Features
- Multi-threaded architecture for handling concurrent client connections.
- Support for multiple simultaneous connections.
- Creation and management of IRC channels.
- User authentication and registration.
- Broadcasting messages to all users in a channel.
- Private messaging between users.
- Handling of various IRC commands such as JOIN, PRIVMSG, PART, etc.
- Support for user nicknames and channel names.
- Connect to the IRC server.
- Join channels and participate in group conversations.
- Send and receive messages.
- Change user nickname.
- Send private messages to other users.

### Run the project
Just do make 

and execute with ./ircserv [port] [password]
- port: The port number on which IRC server will be listening to for incominig IRC connections.
- password: The connection password. It will be needed by any IRC client that tries to connect to your server.

### Run any IRC client or use NetCut
- nc localhost [port]
- pass [password]
- nick [your_nickname]
- user [your_username]

### Run IRSSI client
If you dont have irssi client on your computer: sudo apt install irssi

then, on your terminal: irssi and do like the video above.
