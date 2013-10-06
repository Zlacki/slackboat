#ifndef _IPC_H
#define _IPC_H

void init_ipc(void);
int ipc_send(char *);
void init_module(char *);
void *handle_ipc_calls();
void child_handler(int);

#endif
