
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <setjmp.h>
#include <errno.h>
#include <dirent.h>
#include <limits.h>
/*#include <sys/stat.h>
*/

#define MALLOC(szi) 		malloc(szi);
typedef unsigned char uint8;
char multi[3][4];
char multi2[][5] = {"ABC1", "DEF2", "GHI3"};
union kdev {
	int a;
	char b;
};

enum STBVorbisError {
	beata, alpha
};

struct corm {
  enum STBVorbisError error;
};

typedef struct sk sk_t;
sk_t *f();
struct sk {
	int i;
};
sk_t *f();


struct MacroBuf {
	int z;
        char params[100+1];
	int x[3][200];
        char args[1000+1];
	int y;
	struct sk so;
	union kdev ui; 
};

static struct MacroBuf Macexp[10];

typedef struct sqlite3_file sqlite3_file;
struct sqlite3_file {
	int x;
	char jml[100];
	int a;
	int b;
/*	struct bob {int io;} * po;
*/
};

const char msg[] = "bib";
const char msg1[] = {1, 2, 3, 0};
const char *msg2[] = {msg, (void*)0, (void*)0};
/*sqlite3_file msg3 = {0,{'b', 'o', 'b', 0}, 0, 0};
*/
  sqlite3_file msg3/* = {0,"TOP", 0, 7}*/;

/*
struct stttt {int a;int b; int c;} pttt[] = {0,2, 3};
*/

/*void fii(struct sqlite3_file *i) {
}*/

typedef struct {
	int x;
} td1;

void (*signal16)(int sig, void (*func)(int));

void (*(*signal19)(int sig1, void (*func)(int)))(int);



void (*signal21(short))(int);

void (*signal22(int sig2, void (*func)(int)))(int);

int	Verbose = 0;
int	Errors = 0;

void fail(char *name) {
	fprintf(stderr, "%s failed\n", name);
	Errors++;
}

void pr(char *s) {
	if (Verbose) fprintf(stderr, "%s\n", s);
}

int m(void *a, int b, int c) {
	return 0;
}

double sin(double s) {
	return 0;
}
double square(double s) {
	return 0;
}

#define DECODE_RAW                                  \
   if (f->valid_bits < STB_VORBIS_FAST_HUFFMAN_LENGTH)        \
   if (f->valid_bits < STB_VORBIS_FAST_HUFFMAN_LENGTH)        \
   if (f->valid_bits < STB_VORBIS_FAST_HUFFMAN_LENGTH)        \
   if (f->valid_bits < STB_VORBIS_FAST_HUFFMAN_LENGTH)        \
   if (f->valid_bits < STB_VORBIS_FAST_HUFFMAN_LENGTH)        \
   if (f->valid_bits < STB_VORBIS_FAST_HUFFMAN_LENGTH)        \
   if (f->valid_bits < STB_VORBIS_FAST_HUFFMAN_LENGTH)        \
   if (f->valid_bits < STB_VORBIS_FAST_HUFFMAN_LENGTH)        \
   if (f->valid_bits < STB_VORBIS_FAST_HUFFMAN_LENGTH)        \
   if (f->valid_bits < STB_VORBIS_FAST_HUFFMAN_LENGTH)        \
   if (f->valid_bits < STB_VORBIS_FAST_HUFFMAN_LENGTH)        \
   if (f->valid_bits < STB_VORBIS_FAST_HUFFMAN_LENGTH)        \
   if (f->valid_bits < STB_VORBIS_FAST_HUFFMAN_LENGTH)        \
   if (f->valid_bits < STB_VORBIS_FAST_HUFFMAN_LENGTH)        \
   if (f->valid_bits < STB_VORBIS_FAST_HUFFMAN_LENGTH)        \
	return;

#define M_PI 3

static void compute_window(int ni, int *window)
{
  /* int n2 = ni >> 1, i;
   window[0] = 0 * 3;
   for (i=0; i < n2; ++i) {;}
      /*window[i] = (float) sin(0.5 * M_PI * square((float) sin((i - 0 + 0.5) / n2 * 0.5 * M_PI)));
*/
}

void test(void) {
	/*void *ptr;
	int stru1;*/
/*	struct sqlite3_file stru1;
	sqlite3_file stru2;
*/ 
	char **argv;
	int (*daytab1)[13];
	int *daytab2[13];
	void *comp1(int a, int *b, int (*c)(int ca));
	void *(comp3)();
	void (*comp2)();
	char (*(*x1())[])();
	char (*(*x2[3])())[5];

	int **start[8][18][160];
	char (*(*jioxu())[8])(); 
	static uint8 vorbis[6]  = { 'v', 'o', 'r', 'b', 'i', 's' };
	int	i = 0;
	int range = vorbis[i+1];
	int (*funky)[8];
	signed char ic = 0;
	unsigned int j = 0;
	char c3[] = {65, 66, 67, 68, 0};
	char (*(*jzi[3])());
	char (*(*ji[3])())[5];
	char (*(*jiox())[8])(); 
	int zu;
	char	i2 = {4,5,6};
	/*char c1[] = "test01";
	char *c2 = "test03";
	char *c4 = (char[]){65, 66, 67, 69, 0};
	static char c5[] = "test04";
	static char *c6 = "test05";
	static char *c8 = {"ABCG", 66, 67, 71, 0};
	static char c7[] = {65, 66, 67, 70, 0};
	*/
/*	extern char c10[];
*/	extern char *c11;
/*	extern char c12[];
	extern char *c13;
*/
/*
 *	unsigned n;
 *	int base;
 *	n = n % base;
 * */
	i2 = (char){4,8};

	if (c3 != &c3[0]) {fail("array address 1");}
	if (&c3 != &c3[0]) {fail("array address 2");}
	if (c3[2] != 67) {fail("c3[] init failed");}
	if (range != 'o') {fprintf(stderr, "i'%c'", range);fail("inex array");}
	j = 0;
	if (i2 != 4) {fail("scalar init i2 ");}
	i = vorbis[vorbis[0]+1];
	/*i = (int)(void*)(char (*(*[3])())[5])0;
	i = (char (*(*())[])())0;*/
/*	i = (int)(void*)((short (*)[8])0);
*/
  	  i = 0;
		switch(i) {
		case 1:
		default:
			
			printf("JJJJJJJJJJJJJJJJJJJ\n\n");
			break;
		}
	{ char i = 5;}

        i = 3;	
	j = i;
	j = -1;
	if ((void*)Macexp[0].x == (void*)&Macexp[0].y) {
		fail("struct / array bug");
	}
	printf("ABC\n");
	if (((void*)multi2[0] != (void*)0) /*|| strcmp("DEF2", multi2[1])*/) {
	printf("ABCi\n");
		fprintf(stderr, "%x %x: %x %x %x S:%d %d\n",&multi2[2][0], multi2/*, multi2[0], multi2[1], multi2[2], sizeof(multi2[1]), sizeof(multi2)*/);
		/*fail("multi-dimension array 1");*/
	}
	printf("DIC\n");
	goto next1;
	fail("goto");
pr:
next1:
	i--;
	printf("____ %d i-- %s\n", i, msg3.jml);
	if (i) goto pr; 

	if (j < i) {
		fail("unsigned lower than zero");
	}
	
	j = -1;
	if (j < 0) {
		fail("unsigned lower than zero (2)");

	}
	i = j >> 1;
	if (i < 0) {
		fail("unsigned shift is negative");

	}
	
	i = j / 3; if (i < 0) { fail("unsigned divide by 3 is negative"); }
	i = j / 2; if (i < 0) { fail("unsigned divide by 2 is negative"); }
	i = 2;
	i = j / i; if (i < 0) { fail("unsigned divide by int 2 is negative"); }
	i = -2;
	i = i / (int)j; if (i != 2) { fail("signed divide -2 by -1 != 2 "); }
	ic = 127;
	ic++;
	i = ic;
	if (i != -128) {fail("signed char assign");}

	/*
	if (1) {
		int o = 4;
		int oo = 3;
		int ooo = 7;
		printf("%d %dJJJJ 437 %d %d %d\n", sizeof(stru1), sizeof(stru2) , o, oo, ooo);

	}
	*/
	pr("auto test");
/*	printf("TEST %s\n", c1);*/
/*	if (memcmp(c1, "test01", 7)) fail("autoarray-1");
	if (memcmp(c2, "test03", 7)) fail("autopointer-1");
	if (memcmp(c3, "ABCD", 5)) fail("autoarray-2");
	if (memcmp(c4, "ABCE", 5)) fail("autopointer-2");
	if (memcmp(c5, "test04", 7)) fail("locstatarray-1");
	if (memcmp(c6, "test05", 7)) fail("locstatpointer-1");
	if (memcmp(c7, "ABCF", 5)) fail("locstatarray-2");
	if (memcmp(c8, "ABCG", 5)) fail("locstatpointer-2");
*//*	if (memcmp(c10, "test06", 7)) fail("locextarray-1");
*/	if (memcmp(c11, "test07", 7)) fail("locextpointer-1");
/*	if (memcmp(c12, "ABCF", 5)) fail("locextarray-2");
	if (memcmp(c13, "ABCG", 5)) fail("locextpointer-2");
*/
}

char c10[] = "test06";
char *c11; /*= "test07";*/
char c12[] = {65, 66, 67, 70, 0};
/*char *c13 = (char[]){65, 66, 67, 71, 0};
*/
int main(int argc, char **argv) {
	c11 = "test07";
	if (argc > 1) {
		if (!strcmp(argv[1], "v")) {
			Verbose = 1;
		}
	}
	test();
	if (Errors > 0) return -1;
	return 0;
}
