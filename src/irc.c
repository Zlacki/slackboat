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
 * @file irc.c
 * @author Zach Knight
 * @date 09 Oct 2013
 * @brief File implementing an abstraction layer for the IRC protocol.
 *
 * This code implements a bunch of useful functions to make handling
 * the IRC protocol much easier on the programmer.  This will not
 * likely be used much because there will be one more abstraction layer
 * for implementing modules to communicate with this application.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "irc.h"
#include "io.h"
#include "util.h"

void irc_notice_event(char *sender, char *argument, char *content) {

}

void irc_welcome_event(void) {
	irc_join_channel("#meds");
}

void irc_privmsg_event(char *sender, char *argument, char *content) {
	if(memchr(content, '.', 1) != NULL) {
		char *args = skip(content, ' ');
		char *command = skip(content, '.');
		int argc = 0;
		char **argv = NULL;
		if(args != NULL) {
			argc = 1;
			for (int i = 0; args[i]; i++)
				argc += (args[i] == ' ');
			argv = (char **) safe_calloc(argc, sizeof(char *));
			argv[0] = args;
			for(int i = 1; i <= argc; i++) {
				trim(argv[i - 1]);
				argv[i] = skip(argv[i - 1], ' ');
			}
		}
		if(!strncmp(command, "load", 4) && argc > 0 && !strncmp(sender, "zach", 4)) {
			irc_privmsg(argument, strformat("Loading %s...", argv[0]));
			char *name = strformat("./modules/%s", argv[0]);
			if(access(name, F_OK) != 0) {
				char *out = strformat("Module %s not found; ignoring.", argv[0]);
				irc_privmsg(argument, out);
				free(out);
			} else {
				irc_privmsg(argument, strformat("%s loaded.", argv[0]));
				char *in = safe_alloc(BUFFER_SIZE);
				char *out = safe_alloc(BUFFER_SIZE);
				FILE *fp = popen(name, "r");
				ipc_add_module(fp, in, out);
			}
			free(name);
		} else {
			args = safe_alloc(BUFFER_SIZE);
			memset(args, 0, BUFFER_SIZE);
			for(int i = 0; i < argc; i++) {
				if(i > 0) {
					strcat(args, " ");
				}
				strcat(args, argv[i]);
			}
			for(int i = 0; i < ipc_index; i++) {
				char *msg = strformat("PRIVMSG %s:%s:%s %s\n", sender, argument, command, args);
				ipc_handles[i].out = msg;
				ipc_send(ipc_handles[i]);
				free(msg);
			}
			free(args);
		}
//		if(argc > 0)
	//		free(argv);
	}
}

void irc_privmsg(const char *recipient, const char *message) {
	if(recipient[0] == '\0') {
		puts("irc_privmsg WARNING: No recipient specified.");
		return;
	}
	irc_out("PRIVMSG %s :%s", recipient, message);
	return;
}

void irc_join_channel(const char *channel) {
	irc_out("JOIN %s", channel);
	return;
}
