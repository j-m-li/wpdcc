/*
 *	NMH's Simple C Compiler, 2012
 *	getchar()
 */

#include <stdio.h>
#include <unistd.h>


int getchar(void) {
	return fgetc(stdin);
}
