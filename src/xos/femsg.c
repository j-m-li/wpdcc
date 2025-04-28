#include <stdlib.h>
#include <stdio.h>

void femsg(char *prog, long code, char *file)
{
	printf("%s (%s) Error code %x\n", prog, file ? file : "?", code);
	exit(-1);
}

void femsg2(char *prog, char *msg, long code, char *file)
{
	printf("%s (%s) Error code %x: %s\n", prog, file ? file : "?", code, msg);
	exit(-1);

}