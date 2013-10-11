/**
 * Copyright © 2013, Zach Knight <zach@libslack.so>
 *
 * Permission to use, copy, modify, and/or distribute this software
 * for any purpose with or without fee is hereby granted, provided
 * that the above copyright notice and this permission notice appear
 * in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
 * WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL
 * THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING
 * FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF
 * CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT
 * OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/**
 * @file io.c
 * @author Zach Knight
 * @date 09 Oct 2013
 * @brief File implementing entry-point and I/O handling code.
 *
 * This code is the entry-point for the IRC bot core, it
 * also contains the I/O code for connecting to and processing
 * the IRC protocol as best as possible, as well as the I/O code
 * for handling IPC of the modules that implement the bots features
 * as seperate applications through a custom protocol.
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <signal.h>
#include "io.h"
#include "irc.h"
#include "util.h"

int socket_fd;
/*
void ipc_add_module(FILE *fp, char *in, char *out) {
	ipc_handles[ipc_index].fp = fp;
	ipc_handles[ipc_index].in = in;
	ipc_handles[ipc_index++].out = out;
	return;
}
*/
int ipc_send(ipc_handle_t handle) {
    if (DEBUG)
        printf("IPC OUT: %s", handle.out);
    return fwrite(handle.out, sizeof(ipc_handle_t), 1, handle.fp);
}

int ipc_read(ipc_handle_t handle) {
	size_t tread = 0;
	char c;

	if (handle.in == NULL) {
		errno = EINVAL;
		return -1;
	}

	while((c = fgetc(handle.fp)) != EOF) {
		if (tread < BUFFER_SIZE - 1) {
			tread++;
			*(handle.in)++ = c;
		}

		if (c == '\n')
			break;
	}

	*(handle.in) = '\0';
	return tread;
}

bool irc_connect(char *server, unsigned int port) {
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

int irc_send(char *out) {
    if (DEBUG)
        printf("OUT: %s", out);
    return send(socket_fd, out, strlen(out), 0);
}

int irc_read(char *in) {
	ssize_t nread = 0;
	size_t tread = 0;
	char c;

	if (in == NULL) {
		errno = EINVAL;
		return -1;
	}

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
				*in++ = c;
			}

			if (c == '\n')
				break;
		}
	}

	*in = '\0';
	return tread;
}


int main(void) {
	if (signal(SIGCHLD, SIG_IGN) == SIG_ERR) {
		perror(0);
		return EXIT_FAILURE;
	}

	/* TODO: Save to binary format compressed with xz, load on boot */
/*	ipc_handles = safe_calloc(100, sizeof(ipc_handle_t));
	ipc_index = 0;
*/
	struct hostent *hp = gethostbyname(SERVER);
	if(!irc_connect(inet_ntoa(*(struct in_addr*) (hp->h_addr_list[0])), 6667)) {
		printf("Failed to connect to %s.\n", SERVER);
		return EXIT_FAILURE;
	}

	for(;;) {
		char *buf = safe_alloc(BUFFER_SIZE);
		int i = irc_read(buf);
		if(i > 0) {
			if(DEBUG)
				printf("IN: %s", buf);
			char *sender = safe_alloc(64);
			char *command = safe_alloc(32);
			char *argument = safe_alloc(32);
			char *content = safe_alloc(256);
			sscanf(buf, ":%63s %31s %31s :%255[^\n]", sender, command, argument, content);

			if(!strncmp(command, "NOTICE", 6))
				irc_notice_event(sender, argument, content);

			if(!strncmp(command, "001", 3) && strstr(content, "Welcome") != NULL)
				irc_welcome_event();

			if(!strncmp(command, "PRIVMSG", 7))
				irc_privmsg_event(sender, argument, content);

			if(!strncmp(buf, "PING :", 6)) {
				buf[1] = 'O';
				irc_send(buf);
			}
			free(sender);
			free(command);
			free(argument);
			free(content);
		} else if(i < 0)
			perror("Unexpected error while reading from IRC socket");
		free(buf);
		usleep(50 * 1000); /* 50ms */
	}

	return EXIT_SUCCESS;
}
