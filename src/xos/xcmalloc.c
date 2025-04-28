#include <Windows.h>

#include <stdlib.h>
#include <stdio.h>
#include <XCMALLOC.H>

char *getspace(long size)
{
	char *s;
	s = malloc(size);
	if (s != NULL) {
		memset(s, 0, size);
		return s;
	}
	puts("Not enough memory\n");
	exit(-1);
}

void *sbrk(long incr)
{
	void *s = malloc(incr);
	memset(s, 0, incr);
	return s;
}

void *sbrkx(long incr, MAB *mb)
{
	if (mb->mab_memtop == mb->mab_membase) {
		mb->mab_membase = malloc(0x40000);
		mb->mab_memtop = mb->mab_membase;
		mb->mab_mempnt = mb->mab_membase;
		mb->mab_memtop += 0x40000;
		memset(mb->mab_membase, 0, 0x40000);
	}
	incr += incr % 4;
	memset(mb->mab_mempnt, 0, incr);

	mb->mab_mempnt += incr;
	if (mb->mab_mempnt > mb->mab_memtop) return 0;
	return mb->mab_mempnt - incr;
}
