/*
 *	NMH's Simple C Compiler, 2011--2022
 *	Error handling
 */

#include "defs.h"
#include "data.h"
#include "decl.h"

void warning(char *s, char *a) {
	fprintf(stderr, "warning: %s: %d: ", File, Line);
	fprintf(stderr, s, a);
	fprintf(stderr, "\n");
}


void error(char *s, char *a) {
	if (Syntoken) return;
	if (!Errors) cleanup();
	fprintf(stderr, "error: %s: %d: ", File, Line);
	if (O_preponly) {
		fprintf(Outfile, "\n#error %s: %d \"%s\"\n", File, Line, Text);
	}
	fprintf(stderr, s, a);
	fprintf(stderr, "\n");
	if (++Errors > 10) {
		Errors = 0;
		fatal("too many errors");
	}
}

void fatal(char *s) {
	Syntoken = 0;
	error(s, NULL);
	error("fatal error, stop", NULL);
	exit(EXIT_FAILURE);
}

void scnerror(char *s, int c) {
	char	buf[32];

	if (isprint(c))
		sprintf(buf, "'%c' (\\x%x)", c, c);
	else
		sprintf(buf, "\\x%x", c);
	error(s, buf);
}

int synch(int syn) {
	int	t;

	t = scan();
	while (t != syn) {
		if (XEOF == t)
			fatal("error recovery failed");
		t = scan();
	}
	Syntoken = syn;
	return t;
}
