#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "util.h"

#define MAXLINES 4096

int main(void) {
	int socket_fd;
	bool debug = true;
	char out[MAXLINES + 1];
	char *server = "irc.what.cd";

	struct hostent *hp = gethostbyname(server);

	if(!slack_connect(inet_ntoa(*(struct in_addr*) (hp->h_addr_list[0])), 6667, &socket_fd)) {
		printf("Failed to connect to %s.\n", server);
		return 1;
	}


	for(;;) {
		char recv[MAXLINES + 1];
		recv[0] = 0;
		int i = slack_read(socket_fd, recv, debug);
		if(i > 0) {
			recv[i] = 0;
			if(strstr(recv, "NOTICE Auth") != NULL) {
				slack_send(socket_fd, "NICK slackboat\r\n", debug);
				slack_send(socket_fd, "USER slackboat 8 * : Slack the Boat\r\n", debug);
				slack_send(socket_fd, "JOIN #pharmaceuticals\r\n", debug);
			}

			if(strstr(recv, "PING") != NULL) {
				out[0] = 0;
				char *pos = strstr(recv, " ") + 1;
				sprintf(out, "PONG %s\r\n", pos);
				slack_send(socket_fd, out, debug);
			}
		}
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
    if (debug) {
        printf("OUT: %s", out);
    }
    return send(socket_fd, out, strlen(out), 0);
}

int slack_read(int socket_fd, char *recv, bool debug) {
    int i = read(socket_fd, recv, MAXLINES);
    if (i > 0 && debug) {
        printf("IN: %s", recv);
    }

    return i;
}
