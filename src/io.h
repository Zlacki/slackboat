#ifndef _IO_H
#define _IO_H

#include <stdbool.h>

#define BUFFER_SIZE 1024
#define DEBUG true

int socket_fd;

bool slack_connect(char *, unsigned int);
int slack_send(char *);
int slack_read(char *);

#endif
