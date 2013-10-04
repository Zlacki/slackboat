#ifndef _IRC_H
#define _IRC_H

#define NICK "slackboat"
#define SERVER "irc.what.cd"
#define PASSWORD "PASSWORD"

void irc_notice_event(char *, char *, char *);
void irc_welcome_event(void);
void irc_privmsg_event(char *, char *, char *);
void irc_privmsg(const char *, const char *);
void irc_join_channel(const char *);

#endif
