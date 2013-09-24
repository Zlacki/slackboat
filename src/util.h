#ifndef _UTIL_H
#define _UTIL_H

bool slack_connect(char*, unsigned int, int*);
int slack_send(int, char*, bool);
int slack_read(int, char*, bool);

#endif
