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
 * @file io.h
 * @author Zach Knight
 * @date 09 Oct 2013
 * @brief File declaring functions for I/O.
 *
 * This code declares what functions need to be shared with other
 * parts of the application, which are implemented in the file io.c
 * and includes header for boolean types for C.
 */

#ifndef _IO_H
#define _IO_H

#include <stdbool.h>

typedef struct {
	FILE *fp;
	char *in, *out;
} ipc_handle_t;

ipc_handle_t *ipc_handles;
int ipc_index;

void ipc_add_module(FILE *, char *, char *);
int ipc_read(ipc_handle_t);
int ipc_send(ipc_handle_t);
bool irc_connect(char *, unsigned int);
int irc_send(char *);
int irc_read(char *);

#endif
