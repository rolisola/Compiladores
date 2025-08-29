#include <stdio.h>
#include <stdlib.h>
#include <main.h>

FILE *source;

int main(void)
{
	lookahead = gettoken(source = stdin);

	E();

	return 0;
}
