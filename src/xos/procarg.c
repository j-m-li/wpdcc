
#include <xos.h>
#include <ctype.h>
#include <procarg.h>

int procarg(char **apnt, unsigned int flags, arg_spec *optbl, arg_spec *kwtbl,
	int (*fnc)(char *), void (*uerrrtn)(char *, char *), int (*contfnc)(void),
    char *indirext)
{
	while (*apnt) {
		if ((*apnt)[0] == '/') {
			arg_spec *a;
			int done = 0;
			a = optbl;
			while (a->name) {
				char *n, *o;
				n = a->name;
				o = (*apnt) + 1;
				while (*o && *n && *o != '=' && *n != '*') {
					if (toupper(*o) != *n) break;
					o++;
					n++;
				}
				while (*n == '*' && *o != 0 && *o != '=') o++; 
				if ((*n != 0 && *n != '*') || (*o != '=' && *o != '\0')) {
					a++;
					continue;
				}
				if (a->func) {
					arg_data d;
					d.name = a->name;
					d.flags = a->flags;
					d.val.s = o;
					if (*o == '=') {
						d.val.s++;
					} 
					a->func(&d);
				}
				done = 1;
				break;
			}
			if (!done) {
				if (uerrrtn) uerrrtn("unknown option", a->name);
			}
		} else {
			if (fnc) fnc(*apnt);
		}
		apnt++;
	}
	return 0;
	//printf("FIXME %x\n");
}

