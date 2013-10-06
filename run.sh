#!/bin/sh
while [[ 1 ]]; do
	socat - UNIX-RECVFROM:./slackboat.sock
	sleep .5
done
exit
