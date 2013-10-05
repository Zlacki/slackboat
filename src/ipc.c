#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <string.h>
#include <errno.h>
#include "ipc.h"
#include "io.h"
#include "irc.h"
#include "util.h"

int ipc_index = 0;
char *ipc_names[256];

void init_ipc(void) {
	pthread_t ipc_thread;
	pthread_create(&ipc_thread, NULL, handle_ipc_calls, NULL);

	struct sigaction sa;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sa.sa_handler = child_handler;
	sigaction(SIGCHLD, &sa, NULL);
}

void init_module(char *name) {
	pid_t child = fork();

	if(child < 0) {
		perror("Error starting module");
		return;
	}

	/* TODO: Handle errors starting modules without killing IRC I/O */

	if(!child) {
		ipc_names[ipc_index++] = name;
		char s[256];
		snprintf(s, 255, "Module ‘%s’ starting...", name);
		irc_privmsg("#pharmaceuticals", s);
		strprepend(name, "./");
		execl(name, name, NULL);
	}

	return;
}

void *handle_ipc_calls() {
	int s, s2, t, len;
	struct sockaddr_un local, remote;
	char str[256];

	if ((s = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
		perror("socket");
		exit(1);
	}

	local.sun_family = AF_UNIX;
	strcpy(local.sun_path, "./slackboat.sock");
	unlink(local.sun_path);
	len = strlen(local.sun_path) + sizeof(local.sun_family);
	if (bind(s, (struct sockaddr *)&local, len) == -1) {
		perror("bind");
		exit(1);
	}

	if (listen(s, 5) == -1) {
		perror("listen");
		exit(1);
	}

	for(;;) {
		int n;
		t = sizeof(remote);

		if ((s2 = accept(s, (struct sockaddr *)&remote, (socklen_t *) &t)) == -1) {
			perror("accept");
			exit(1);
		}

		while((n = recv(s2, str, 255, 0)) > 0) {
			char msg[256];
			snprintf(msg, 255, "From a running module: %s", str);
			irc_privmsg("#pharmaceuticals", msg);
		}
		usleep(50 * 1000);
	}

	return NULL;
}

void child_handler(int sig) {
	pid_t pid;
	int status;

	while((pid = waitpid(-1, &status, WNOHANG)) > 0) {
		char *name = ipc_names[--ipc_index];
		char s[256];
		snprintf(s, 255, "Module ‘%s’ has stopped.", name);
		irc_privmsg("#pharmaceuticals", s);
	}
}
