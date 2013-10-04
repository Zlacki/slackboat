#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <sys/wait.h>
#include "ipc.h"
#include "irc.h"
#include "util.h"

int pipe_fd[2];
int ipc_index = 0;
char *ipc_names[256];

void init_ipc(void) {
	pipe(pipe_fd);
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
	if(child == -1) {
		perror("Error starting module");
		return;
	}
	if(child == 0) {
		close(pipe_fd[0]);
		ipc_names[ipc_index++] = name;
		char s[256];
		snprintf(s, 255, "Module ‘%s’ starting...", name);
		irc_privmsg("#pharmaceuticals", s);
		strprepend(name, "./");
		dup2(pipe_fd[1], STDOUT_FILENO);
		close(pipe_fd[1]);
		execl(name, name, NULL);
	}

	return;
}

void *handle_ipc_calls() {
	close(pipe_fd[1]);
	char cbuf[256];
	int tread;
	for(;;) {
		while((tread = read(pipe_fd[0], cbuf, 256)) > 0) {
			cbuf[tread] = '\0';
			char msg[256];
			snprintf(msg, 255, "From a running module: %s", cbuf);
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
