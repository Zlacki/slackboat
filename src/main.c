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

int main(void) {
	int socket_fd;
	bool debug = true;

	struct hostent *hp = gethostbyname(SERVER);
	if(!slack_connect(inet_ntoa(*(struct in_addr*) (hp->h_addr_list[0])), 6667, &socket_fd)) {
		printf("Failed to connect to %s.\n", SERVER);
		return 1;
	}

	for(;;) {
		char in_buffer[BUFFER_SIZE];
		int i = slack_read(socket_fd, in_buffer, debug);
		if(i > 0) {
			printf("IN: %s", in_buffer);
			char sender[64], command[32], argument[32], content[256];

			sscanf(in_buffer, ":%63s %31s %31s :%255[^\n]", sender, command, argument, content);

			if(!strcmp(command, "NOTICE") && !strcmp(argument, "Auth")) {
				if(strstr(content, "Looking up your hostname") != NULL) {
					slack_send(socket_fd, "NICK slackboat\r\n", debug);
					slack_send(socket_fd, "USER slackboat 8 * :Slack the Boat\r\n", debug);
				} else if(strstr(content, "Welcome") != NULL)
					slack_send(socket_fd, "JOIN #pharmaceuticals\r\n", debug);
			}

			if(strstr(in_buffer, "PING") != NULL) {
				char out[BUFFER_SIZE];
				out[0] = 0;
				char *pos = strstr(in_buffer, " ") + 1;
				sprintf(out, "PONG %s\r\n", pos);
				slack_send(socket_fd, out, debug);
			}
		} else if(i < 0)
			perror("Unexpected error while reading from IRC socket");
		usleep(5 * 10000); /* 50ms */
	}

	return 0;
}

bool slack_connect(char *server, unsigned int port, int *socket_fd) {
	struct sockaddr_in servaddr;
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(port);

	if ( (*socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		return false;

	if (inet_pton(AF_INET, server, &servaddr.sin_addr) <= 0)
		return false;

	if (connect(*socket_fd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0 )
		return false;

	return true;
}

int slack_send(int socket_fd, char *out, bool debug) {
    if (debug)
        printf("OUT: %s", out);
    return send(socket_fd, out, strlen(out), 0);
}

int slack_read(int socket_fd, char *in_buffer, bool debug) {
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
