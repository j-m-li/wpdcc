#include <stdio.h>

int ccc;
int hhh;
char buffer[100];

#define _XC_ __attribute__ ((stdcall))


long svcIoInBlock_F_(long c, char b[], long d, long h) _XC_;

#define svcIoInBlock(a1, a2, a3) svcIoInBlock_F_(a3, a2, 0, a1)


int main(void)

{
	svcIoInBlock(hhh, buffer, ccc);
	return (0);
}
