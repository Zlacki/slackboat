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
	/* TODO: create savestate to load info at startup instead of compile-time */
	if(strstr(content, "*** Looking up your hostname") != NULL) {
		char *out = strformat("NICK %s\n", NICK);
		irc_send(out);
		free(out);
		out = strformat("USER %s 8 * :%s\n", NICK, NAME);
		irc_send(out);
		free(out);
	}

	if(!strncmp(sender, "NickServ", 8) && strstr(content, "please choose a different nick") != NULL) {
		char *out = strformat("IDENTIFY %s\n", PASSWORD);
		irc_privmsg("NickServ", out);
		free(out);
	}
}

void irc_welcome_event(void) {
	irc_join_channel("#pharmaceuticals");
}

void irc_privmsg_event(char *sender, char *argument, char *content) {
	if(!strncmp(content, ".", 1)) {
		char **argv;
		char *command = safe_alloc(128);
		char *args = safe_alloc(256);
		int argc;
		if(strstr(content, " ") != NULL) {
			argc = 1;
			sscanf(content, ".%127s %255[^\r\n]", command, args);
			for (int i = 0; args[i]; i++)
				argc += (args[i] == ' ');
		} else {
			argc = 0;
			sscanf(content, ".%127s[^\r\n]", command);
		}
		argv = (char **) safe_calloc(argc, sizeof(char *));
		if(argc > 0) {
			argv[0] = strtok(args, " ");
			for(int i = 1; i < argc; i++)
				argv[i] = strtok(NULL, " ");
		}
		if(!strncmp(command, "load", 4) && argc > 0 && !strncmp(sender, "zlacki", 6)) {
			irc_privmsg(argument, strformat("Loading %s...\n", argv[0]));
			char *name = strformat("./modules/%s", argv[0]);
			if(access(name, F_OK) != 0) {
				char *out = strformat("Module %s not found; ignoring.\n", argv[0]);
				irc_privmsg(argument, out);
				free(out);
			} else {
				irc_privmsg(argument, strformat("%s loaded.\n", argv[0]));
				char *in = safe_alloc(BUFFER_SIZE);
				char *out = safe_alloc(BUFFER_SIZE);
				FILE *fp = popen(name, "r");
				ipc_add_module(fp, in, out);
			}
			free(name);
		} else {
			for(int i = 0; i < ipc_index; i++) {
				ipc_handles[i].out = strformat("PRIVMSG %s:%s:%s\n", sender, argument, content);
				ipc_send(ipc_handles[i]);
			}
		}
		/*if(!strncmp(command, "kick", 4) && argc > 0 && !strncmp(sender, "zlacki", 6)) {
			for(int i = 0; i < 100; i++) {
				ipc_handle_t handle = ipc_handles[i];
				if(handle.fp != NULL) {
					char *raw = safe_alloc(BUFFER_SIZE);
					if(argc > 1) {
						raw = strformat("%s", argv[1]);
						for(int j = 2; j < argc; j++)
							strcat(raw, argv[j]);
					}
					strcpy(handle.out, strformat("KICK %s %s :%s\n", argument, argv[0], (raw == NULL ? "Requested" : raw)));
					ipc_send(handle);
				}
			}
		}*/
		free(command);
		free(args);
		free(argv);
	}
}

void irc_privmsg(const char *recipient, const char *message) {
	char *out = strformat("PRIVMSG %s :%s", recipient, message);
	irc_send(out);
	free(out);
	return;
}

void irc_join_channel(const char *channel) {
	char *out = strformat("JOIN %s\n", channel);
	irc_send(out);
	free(out);
	return;
}
