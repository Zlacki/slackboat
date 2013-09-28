PREFIX=/usr

INCS = -I. -I/usr/include -I/usr/local/include -I/usr/X11R6
LIBS = -L/usr/lib -L/usr/local/lib -lpthread

CFLAGS = -std=c99 -Wall -pedantic -O2 -ggdb3 -D_BSD_SOURCE ${INCS}
LDFLAGS = -g ${LIBS}

CC = gcc
