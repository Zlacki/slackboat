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
 * @file irc.h
 * @author Zach Knight
 * @date 09 Oct 2013
 * @brief File declaring an abstraction layer for the IRC protocol.
 *
 * This code declares a bunch of useful functions to make handling
 * the IRC protocol much easier on the programmer.  This will not
 * likely be used much because there will be one more abstraction layer
 * for implementing modules to communicate with this application.
 *
 * It also declares some IRC variables for identification, etc.
 */

#ifndef _IRC_H
#define _IRC_H

#define NICK "slackboat"
#define SERVER "irc.what.cd"
#define PASSWORD "PASSWORD"

void irc_notice_event(char *, char *, char *);
void irc_welcome_event(void);
void irc_privmsg_event(char *, char *, char *);
void irc_privmsg(const char *, const char *);
void irc_join_channel(const char *);

#endif
