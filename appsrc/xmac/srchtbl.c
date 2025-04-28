#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <xcmalloc.h>
#include <xos.h>
#include "xmac.h"

int compsym(char *sym1, char *sym2, int s)
{
	while (*sym1 == *sym2 && s > 0) {
		s--;
		if (*sym1 == ' ' || s == 0) return 0;
		sym1++;
		sym2++;
	}
	return 1;
}

SY *srchtbl(char *sym, void *tbl, int tblsize, int strsize, int entsize)
{
	int i;
	char *s = tbl;

	s = tbl;	
	i = 0;
	while (i < tblsize) {
		if (compsym(sym, s, strsize) == 0) {
			return (void*)s;
		}
		s += entsize;
		i++;
	}
	return 0;
}


