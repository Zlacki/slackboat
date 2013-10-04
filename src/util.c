#include <stdlib.h>
#include <string.h>
#include "util.h"

void strprepend(char *s, const char *t) {
	size_t tlen = strlen(t);
	size_t slen = strlen(s);

	s = realloc(s, slen + tlen);

	memmove(s + tlen, s, slen);
	memmove(s, t, tlen);
}
