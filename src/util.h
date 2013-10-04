#ifndef _UTIL_H
#define _UTIL_H

#define BUFFER_SIZE 1024
#define NICK "slackboat"
#define SERVER "irc.what.cd"
#define PASSWORD "PASSWORD"

bool slack_connect(char*, unsigned int);
int slack_send(char*);
int slack_read(char*);

void irc_notice_event(char *, char *, char *);
void irc_welcome_event(void);
void irc_privmsg_event(char *, char *, char *);
void irc_privmsg(const char *, const char *);
void irc_join_channel(const char *);

#endif
