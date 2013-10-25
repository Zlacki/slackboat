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
 * @file util.c
 * @author Zach Knight
 * @date 09 Oct 2013
 * @brief File implementing various ‘peripheral’ functions.
 *
 * This code implements any functions that do not really have enough
 * common code to fit into its own file, and are useful to the project
 * to simplify programming.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <errno.h>
#include "util.h"
#include "io.h"

void eprint(const char *fmt, ...) {
	va_list ap;

	va_start(ap, fmt);
	vsnprintf(bufout, sizeof bufout, fmt, ap);
	va_end(ap);
	fprintf(stderr, "%s", bufout);
	if(fmt[0] && fmt[strlen(fmt) - 1] == ':')
		fprintf(stderr, " %s\n", strerror(errno));
	exit(1);
}

int dial(char *host, char *port) {
	static struct addrinfo hints;
	int srv;
	struct addrinfo *res, *r;

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	if(getaddrinfo(host, port, &hints, &res) != 0)
		eprint("error: cannot resolve hostname '%s':", host);
	for(r = res; r; r = r->ai_next) {
		if((srv = socket(r->ai_family, r->ai_socktype, r->ai_protocol)) == -1)
			continue;
		if(connect(srv, r->ai_addr, r->ai_addrlen) == 0)
			break;
		close(srv);
	}
	freeaddrinfo(res);
	if(!r)
		eprint("error: cannot connect to host '%s'\n", host);
	return srv;
}

#define strlcpy _strlcpy
void strlcpy(char *to, const char *from, int l) {
	memccpy(to, from, '\0', l);
	to[l-1] = '\0';
}

char *eat(char *s, int (*p)(int), int r) {
	while(*s != '\0' && p(*s) == r)
		s++;
	return s;
}

char* skip(char *s, char c) {
	while(*s != c && *s != '\0')
		s++;
	if(*s != '\0')
		*s++ = '\0';
	return s;
}

void trim(char *s) {
	char *e;

	e = s + strlen(s) - 1;
	while(isspace(*e) && e > s)
		e--;
	*(e + 1) = '\0';
}

void *safe_alloc(size_t size) {
	void *ptr;
	if((ptr = malloc(size)) == NULL) {
		perror("Out of memory");
		exit(EXIT_FAILURE);
	}

	return ptr;
}

void *safe_calloc(size_t n, size_t size) {
	void *ptr;
	if((ptr = calloc(n, size)) == NULL) {
		perror("Out of memory");
		exit(EXIT_FAILURE);
	}

	return ptr;
}

void *safe_realloc(void *orig_ptr, size_t size) {
	void *ptr;
	if((ptr = realloc(orig_ptr, size)) == NULL) {
		perror("Out of memory");
		exit(EXIT_FAILURE);
	}

	orig_ptr = ptr;

	return orig_ptr;
}

char *strformat(const char *s, ...) {
	char *buf = safe_alloc(BUFFER_SIZE);
	va_list vl;

	va_start(vl, s);
	vsnprintf(buf, BUFFER_SIZE, s, vl);
	va_end(vl);

	return buf;
}
