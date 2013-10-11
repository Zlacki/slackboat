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
 * @file util.h
 * @author Zach Knight
 * @date 09 Oct 2013
 * @brief File declaring various ‘peripheral’ functions and constants.
 *
 * This code declares any functions that do not really have enough
 * common code to fit into its own file, and are useful to the project
 * to simplify programming.  It also implements various constants to
 * simplify the programming as well.
 */

#ifndef _UTIL_H
#define _UTIL_H

#define BUFFER_SIZE 512
#define DEBUG true

void *safe_alloc(size_t);
void *safe_calloc(size_t, size_t);
void *safe_realloc(void *, size_t);
char *strformat(const char *, ...);

#endif
