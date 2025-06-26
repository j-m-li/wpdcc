
#include <stdio.h>

void loop()
{
	printf(".");
}

void setup()
{
	printf("Hello World!\n");
}

int main(int argc, char *argv[])
{
	setup();
	while (1) {
		loop();
		getchar();
	}
	return -1;
}

