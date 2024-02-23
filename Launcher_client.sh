#!/bin/bash

# Check if necessary parameters are provided
if [ "$#" -ne 2 ]; then
  echo "Usage: $0 <port> <password>"
  exit 1
fi

port=$1
password=$2


# Wait for the server to start
sleep 2

# Launch two instances of irssi, each in a separate terminal window, connecting to the server

terminator -e "irssi -c localhost -p $port -w $password -n ali; exec bash"
irssi -c localhost -p $port -w $password -n bland; exec bash