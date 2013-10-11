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
		irc_send("NICK slackboat\r\n");
		irc_send("USER slackboat 8 * :Slack the Boat\r\n");
	}
	if(!strncmp(sender, "NickServ", 8) && strstr(content, "please choose a different nick") != NULL) {
		char out[256];
		memset(out, 0, 256);
		snprintf(out, 12 + strlen(PASSWORD), "IDENTIFY %s\n", PASSWORD);
		irc_privmsg("NickServ", out);
	}
}

void irc_welcome_event(void) {
	irc_join_channel("#pharmaceuticals");
}

void irc_privmsg_event(char *sender, char *argument, char *content) {
	if(!strncmp(content, ".", 1) && !strncmp(sender, "zlacki", 6)) {
		char **argv, command[128], args[256];
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
		argv = (char **) malloc(sizeof(char *) * argc);
		if(argc > 0) {
			argv[0] = strtok(args, " ");
			for(int i = 1; i < argc; i++)
				argv[i] = strtok(NULL, " ");
		}
		if(!strncmp(command, "load", 4) && argc > 0) {
			char *name = malloc(strlen(argv[0]));
			strcpy(name, argv[0]);
			if(access(name, F_OK) != 0)
				irc_privmsg(argument, "Module not found; ignoring.");
			else {
				strprepend(name, "./");
				FILE *fp = popen(name, "r");
				char buffer[512];
				char c;
				int i = 0;
				while((c = fgetc(fp)) != '\n')
					buffer[i++] = c;
				buffer[i] = '\0';
				irc_privmsg(argument, buffer);
				pclose(fp);
			}
			free(name);
		}
		free(argv);
	}
}

void irc_privmsg(const char *recipient, const char *message) {
	char out[256];
	memset(out, 0, 256);
	snprintf(out, 13 + strlen(recipient) + strlen(message), "PRIVMSG %s :%s\n", recipient, message);
	irc_send(out);
	return;
}

void irc_join_channel(const char *channel) {
	char out[BUFFER_SIZE];
	memset(out, 0, BUFFER_SIZE);
	snprintf(out, 8 + strlen(channel), "JOIN %s\n", channel);
	irc_send(out);
	return;
}
