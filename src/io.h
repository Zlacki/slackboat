#ifndef _IO_H
#define _IO_H

#include <stdbool.h>

#define BUFFER_SIZE 1024
#define DEBUG true

bool irc_connect(char *, unsigned int);
int irc_send(char *);
int irc_read(char *);

#endif
