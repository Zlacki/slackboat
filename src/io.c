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
#include <pthread.h>
#include <stdarg.h>
#include <signal.h>
#include "io.h"
#include "irc.h"
#include "util.h"

static FILE *srv;
static time_t trespond;
int socket_fd;

void ipc_add_module(FILE *fp, char *in, char *out) {
	ipc_handles[ipc_index].fp = fp;
	ipc_handles[ipc_index].in = in;
	ipc_handles[ipc_index++].out = out;
	return;
}

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

void irc_out(char *fmt, ...) {
	va_list ap;

	va_start(ap, fmt);
	vsnprintf(bufout, sizeof bufout, fmt, ap);
	va_end(ap);
    if (DEBUG)
        printf("OUT: %s\n", bufout);
	fprintf(srv, "%s\r\n", bufout);
}

void irc_in(char *cmd) {
	char *usr, *par, *txt;

	usr = SERVER;
	if(!cmd || !*cmd)
		return;
	printf("IN: %s", cmd);
	if(cmd[0] == ':') {
		usr = cmd + 1;
		cmd = skip(usr, ' ');
		if(cmd[0] == '\0')
			return;
		skip(usr, '!');
	}
	skip(cmd, '\r');
	par = skip(cmd, ' ');
	txt = skip(par, ':');
	trim(par);
	if(!strcmp("PONG", cmd))
		return;
	if(!strcmp("PING", cmd))
		irc_out("PONG %s", txt);
}

void *process_ipc_messages() {
	for(;;)
		for(int j = 0; j < 100; j++) {
			int i = ipc_read(ipc_handles[j]);
			if(i > 0) {
				if(DEBUG)
					printf("IPC IN: %s", ipc_handles[j].in);
			}
		}
}

int main(void) {
	int i;
	struct timeval tv;
	fd_set rd;

	i = dial(SERVER, PORT);
	srv = fdopen(i, "r+");

	irc_out("NICK %s", NICK);
	irc_out("USER %s localhost %s :%s", NICK, SERVER, USER);
	irc_join_channel("#meds");
	fflush(srv);
	setbuf(stdout, NULL);
	setbuf(srv, NULL);
	for(;;) {
		FD_ZERO(&rd);
		FD_SET(fileno(srv), &rd);
		tv.tv_sec = 120;
		tv.tv_usec = 0;

		i = select(fileno(srv) + 1, &rd, 0, 0, &tv);
		if(i < 0) {
			if(errno == EINTR)
				continue;
			eprint("sic: error on select():");
		} else if(i == 0) {
			if(time(NULL) - trespond >= 300)
				eprint("sic shutting down: parse timeout\n");
			irc_out("PING %s", SERVER);
			continue;
		}

		if(FD_ISSET(fileno(srv), &rd)) {
			if(fgets(bufin, sizeof bufin, srv) == NULL)
				eprint("sic: remote host closed connection\n");
			irc_in(bufin);
			trespond = time(NULL);
		}
	}

	return EXIT_SUCCESS;
}
