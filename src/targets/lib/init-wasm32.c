
#include <stdio.h>

FILE *stdin = 0;
FILE *stdout = 0;
FILE *stderr = 0;
int errno = 0;

int main(int,char*[]);

void _start()
{
	main(0,0);
}

