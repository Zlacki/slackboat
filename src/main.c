#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>

#include "util.h"

int socket_fd;
int ipc_fd;
bool debug = true;

int main(void) {
	if((ipc_fd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0 {
		perror("Cannot initialize IPC");
		return 1;
	}

	struct hostent *hp = gethostbyname(SERVER);
	if(!slack_connect(inet_ntoa(*(struct in_addr*) (hp->h_addr_list[0])), 6667)) {
		printf("Failed to connect to %s.\n", SERVER);
		return 1;
	}

	for(;;) {
		char in_buffer[BUFFER_SIZE];
		int i = slack_read(in_buffer);
		if(i > 0) {
			if(debug)
				printf("IN: %s", in_buffer);

			char sender[64], command[32], argument[32], content[256];
			sscanf(in_buffer, ":%63s %31s %31s :%255[^\n]", sender, command, argument, content);

			if(!strncmp(command, "NOTICE", 6))
				irc_notice_event(sender, argument, content);

			if(!strncmp(command, "001", 3) && strstr(content, "Welcome") != NULL)
				irc_welcome_event();

			if(!strncmp(command, "PRIVMSG", 7))
				irc_privmsg_event(sender, argument, content);

			if(!strncmp(in_buffer, "PING :", 6)) {
				char out[BUFFER_SIZE];
				memset(out, 0, BUFFER_SIZE);
				char *pos = strstr(in_buffer, " ") + 1;
				snprintf(out, 8 + strlen(pos), "PONG %s", pos);
				slack_send(out);
			}
		} else if(i < 0)
			perror("Unexpected error while reading from IRC socket");
		usleep(50 * 1000); /* 50ms */
	}

	return 0;
}

bool slack_connect(char *server, unsigned int port) {
	struct sockaddr_in servaddr;
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(port);

	if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		return false;

	if (inet_pton(AF_INET, server, &servaddr.sin_addr) <= 0)
		return false;

	if (connect(socket_fd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0 )
		return false;

	return true;
}

int slack_send(char *out) {
    if (debug)
        printf("OUT: %s", out);
    return send(socket_fd, out, strlen(out), 0);
}

int slack_read(char *in_buffer) {
	ssize_t nread = 0;
	size_t tread = 0;
	char c;

	if (in_buffer == NULL) {
		errno = EINVAL;
		return -1;
	}

	tread = 0;
	for (;;) {
		nread = read(socket_fd, &c, 1);

		if (nread == -1) {
			if (errno == EINTR)
				continue;
			else
				return -1;
		} else if (nread == 0) {
			if (tread == 0)
				return 0;
			else
				break;

		} else {
			if (tread < BUFFER_SIZE - 1) {
				tread++;
				*in_buffer++ = c;
			}

			if (c == '\n')
				break;
		}
	}

	*in_buffer = '\0';
	return tread;
}

void irc_privmsg(const char *recipient, const char *message) {
	char out[256];
	memset(out, 0, 256);
	snprintf(out, 13 + strlen(recipient) + strlen(message), "PRIVMSG %s :%s\r\n", recipient, message);
	slack_send(out);
	return;
}

void irc_join_channel(const char *channel) {
	char out[BUFFER_SIZE];
	memset(out, 0, BUFFER_SIZE);
	snprintf(out, 8 + strlen(channel), "JOIN %s\r\n", channel);
	slack_send(out);
	return;
}
