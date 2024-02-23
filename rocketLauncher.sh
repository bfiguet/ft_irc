#!/bin/bash

# Check if necessary parameters are provided
if [ "$#" -ne 2 ]; then
  echo "Usage: $0 <port> <password>"
  exit 1
fi

port=$1
password=$2

make re

sleep 2

terminator -e "./Launcher_client.sh $port $password; exec bash"

# Launch the IRC server
valgrind --track-fds=all ./ircserv $port $password

# Waimaket for the server to start
sleep 1

# Launch two instances of irssi, each in a separate terminal window, connecting to the server

