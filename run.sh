#!/bin/sh
while [[ 1 ]]; do
	echo 'hello' |socat - UNIX-CONNECT:./slackboat.sock
	sleep 3
done
exit
