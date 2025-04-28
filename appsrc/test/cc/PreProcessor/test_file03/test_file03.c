//#include <stdlib.h>

void foc(char **a, char *b[])
{
	b = a;
}


int main(int argc, char **argv)
{
	char **p = NULL;

	p = realloc(p, 10);

	
	return 0;	
}

