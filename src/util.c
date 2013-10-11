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
#include "util.h"

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
