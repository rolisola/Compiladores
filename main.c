#include <stdio.h>
#include <stdlib.h>
#include <main.h>

FILE *source;
FILE *objcode;

int main(void) {
	lookahead = gettoken(source = stdin);
	objcode = stdout;

	E();

	#ifndef DEBUG
		putchar('\n');
	#endif
	return 0;
}
