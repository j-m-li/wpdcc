/*
 * Ad-hoc signal() test
 * Nils M Holm, 2014
 * In the public domain
 */

#include <stdlib.h>
#include <stdio.h>
#include <signal.h>

int	Errors = 0;

void fail(char *name) {
	puts(name);
	puts(" failed!");
	Errors++;
}

volatile caught = 0;

int catch(void) {
	caught = 1;
}

struct struct_s {
	signed int bit : 3;
	signed int bio : 6;
	signed int : 0;
	char a[3];
	int b;
	int c[4];
};

int f()
{
	return 8;
}

int main(int argc, char **argv) {
	kprintf(1, "==========================================\n");
	kprintf(1, "This test may result in a core dump.\n");
	kprintf(1, "If it does, see the signal() function of\n");
	kprintf(1, "your lib/crt0.s file for hints.\n");
	kprintf(1, "If there are none, please mail the author!\n");
	kprintf(1, "==========================================\n");
	if (1) {
		int k = 2;
		int *a = (void*)f();
		//struct struct_s c2 = {"AB", 10, {100, 101, 102, 103}};
		//struct struct_s c2 = {65,66,0, 10, 100, 101, 102, 103};
		char c1[10] = {'A', 'B', 'C', 'D', 'E', (char)(long)main, 0};
		char c[10] = "ABCDEF";
		kprintf(1, c);
		kprintf(1, "ptr %d\n", a);
		if (k != 2) fail("int auto int fail");
		if (c[1] != 'B') fail("string auto init fail");
		if (1) {
			int ki = 3;
			a = &ki;
			if (ki != 3 || k != 2) fail("local auto init fail 1");
		}
		if (1) {
			int ko = 7;
			if (a != &ko) fail("local auto address fail");
			if (ko != 7 || k != 2) fail("local auto init fail 2");
		}
	}
	signal(SIGABRT, catch);
	raise(SIGABRT);
	if (!caught) fail("signal-1");
	caught = 0;
	signal(SIGABRT, SIG_IGN);
	raise(SIGABRT);
	if (caught) fail("signal-1");
	return Errors? EXIT_FAILURE: EXIT_SUCCESS;
}
