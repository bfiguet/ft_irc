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

# Launch the IRC server
terminator -e "./ircserv $port $password; exec bash"

# Waimaket for the server to start
sleep 1

# Launch two instances of irssi, each in a separate terminal window, connecting to the server

terminator -e "irssi -c localhost -p $port -w $password -n Jacques; exec bash"
terminator -e "irssi -c localhost -p $port -w $password -n Magalie; exec bash"
