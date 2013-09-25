#ifndef _UTIL_H
#define _UTIL_H

#define BUFFER_SIZE 1024
#define NICK "slackboat"
#define SERVER "irc.what.cd"

bool slack_connect(char*, unsigned int, int*);
int slack_send(int, char*, bool);
int slack_read(int, char*);

void irc_notice_event(char *, char *, char *, char *);
void irc_privmsg_event(char *, char *, char *, char *);
void irc_privmsg(const char *, const char *);
void irc_join_channel(const char *);

#endif
