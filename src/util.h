#ifndef _UTIL_H
#define _UTIL_H

#define BUFFER_SIZE 1024
#define NICK "slackboat"
#define SERVER "irc.what.cd"

bool slack_connect(char*, unsigned int, int*);
int slack_send(int, char*, bool);
int slack_read(int, char*, bool);

#endif
