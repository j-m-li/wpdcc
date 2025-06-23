/*
 *	NMH's Simple C Compiler, 2011--2021
 *	Symbol table management
 */

#include "defs.h"
#include "data.h"
#include "decl.h"
#include "cgen.h"

static char *typename(int p);

int findglob(char *s) {
	int	i;

	for (i=0; i<Globs; i++) {
		if (	Types[i] != TMACRO && Types[i] != TSTRUCT &&
			Types[i] != TUNION &&
			Types[i] != TPTR && Types[i] != TTARRAY &&
			Stcls[i] != CMEMBER &&
			*s == *Names[i] && !strcmp(s, Names[i])
		)
			return i;
	}
	return 0;
}

int findloc(char *s) {
	int	i;

	for (i=Locs; i<NSYMBOLS; i++) {
		if (	Stcls[i] != CMEMBER &&
			*s == *Names[i] && !strcmp(s, Names[i])
		)
			return i;
	}
	return 0;
}

int findlocrd(char *s) {
	int	i;

	for (i=Locs; i<Locsbb; i++) {
		if (	Stcls[i] != CMEMBER &&
			*s == *Names[i] && !strcmp(s, Names[i])
		)
			return i;
	}
	return 0;
}


int findsym(char *s) {
	int	y;

	if ((y = findloc(s)) != 0) {
		if (Stcls[y] != CLABEL) {
			return y;
		}
	}
	return findglob(s);
}

int findmac(char *s) {
	int	i;

	for (i=0; i<Globs; i++) {
		if (	TMACRO == Types[i] &&
			*s == *Names[i] && !strcmp(s, Names[i])
		) {
			return i;
		}
	}
	return 0;
}

int findstruct(char *s) {
	int	i;

	for (i=Locs; i<NSYMBOLS; i++)
		if (	(TSTRUCT == Types[i]  || TUNION == Types[i])&&
			*s == *Names[i] && !strcmp(s, Names[i])
		)
			return i;
	for (i=0; i<Globs; i++)
		if (	(TSTRUCT == Types[i]  || TUNION == Types[i])&&
			*s == *Names[i] && !strcmp(s, Names[i])
		)
			return i;
	return 0;
}

int findmem(int y, char *s) {
	y++;
	while (	(y < Globs ||
		 (y >= Locs && y < NSYMBOLS)) &&
		(CMEMBER == Stcls[y] || TPTR == Types[y] || TTARRAY == Types[y])
	) {
		if (*s == *Names[y] && !strcmp(s, Names[y])) {
			if (TPTR != Types[y] && TTARRAY != Types[y]) { 
				return y;
			}
		}
		y++;
	}
	return 0;
}

int newglob(void) {
	int	p;

	if ((p = Globs++) >= Locs)
		fatal("too many global symbols");
	return p;
}

int findptr(int p) {
	int	i;
	for (i=FUNPTR + 1; i<Globs; i++) {
		if (TPTR == Types[i] && p == Prims[i]) {
			return i | PPTR;
		}
	}
	i = newglob();
	Types[i] = TPTR;
	Prims[i] = p;
	Names[i] = "";
	Stcls[i] = 0;
	Sizes[i] = 0;
	Vals[i] = 0;
	Stext[i] = NULL;
	return i | PPTR;
}

int findarray(int p, int s) {
	int	i;
	for (i=0; i<Globs; i++) {
		if (TTARRAY == Types[i] && p == Prims[i] && s == Sizes[i]) {
			return i | PARRAY;
		}
	}
	i = newglob();
	Types[i] = TTARRAY;
	Prims[i] = p;
	Names[i] = "";
	Stcls[i] = 0;
	Sizes[i] = s;
	Vals[i] = 0;
	Stext[i] = NULL;
	return i | PARRAY;
}

int newloc(void) {
	int	p;

	if ((p = --Locs) <= Globs)
		fatal("too many local symbols");
	return p;
}

char *galloc(int k, int align) {
	int	p, mask;

	k += align * sizeof(int);
	if (Nbot + k >= Ntop)
		fatal("out of space for symbol names");
	p = Nbot;
	Nbot += k;
	mask = sizeof(int)-1;
	if (align)
		while ((int)(long)&Nlist[p] & mask)
			p++;
	return &Nlist[p];
}

char *globname(char *s) {
	char	*p;
	
	p = galloc(strlen(s)+1, 0);
	strcpy(p, s);
	return p;
}

char *locname(char *s) {
	int	p, k;

	k = strlen(s) + 1;
	if (Nbot + k >= Ntop)
		fatal("out of space for symbol names");
	Ntop -= k;
	p = Ntop;
	strcpy(&Nlist[p], s);
	return &Nlist[p];
}

static void defglob(char *name, int prim, int type, int size, int val,
			int scls)
{
	int	st;

	if (TCONSTANT == type || TFUNCTION == type) return;
	gendata();
	st = scls == CSTATIC;
	if (CPUBLIC == scls) genpublic(name);
	if (!(prim & STCMASK)) genname(name);
	if (prim & STCMASK) {
		genbss(gsym(name), objsize(prim), st);
	}
	else if (PCHAR == prim || PSCHAR == prim) {
		gendefb(val);
		genalign(1);
	}
	else if (PINT == prim || PUNSIG == prim) {
		gendefw(val);
	}
	else if (PFLOAT == prim) {
		gendefw(val);
	}
	else {
		gendefp(val);
	}
}

int redeclare(char *name, int oldcls, int newcls) {
	switch (oldcls) {
	case CEXTERN:
		if (newcls != CPUBLIC && newcls != CEXTERN) {
			error("extern symbol redeclared static: %s", name);
		}
		return newcls;
	case CPUBLIC:
		if (CEXTERN == newcls)
			return CPUBLIC;
		if (newcls != CPUBLIC) {
			error("public symbol redeclared static: %s", name);
			return CPUBLIC;
		}
		break;
	case CSPROTO:
		if (newcls != CSTATIC && newcls != CSPROTO)
			error("static symbol redeclared extern: %s", name);
		return newcls;
	case CSTATIC:
		if (CSPROTO == newcls)
			return CSTATIC;
		if (newcls != CSTATIC) {
			error("static symbol redeclared extern: %s", name);
			return CSTATIC;
		}
		break;
	case CTYPE:
		error("redefinition of typedef '%s'", name);
		return CTYPE;
		break;
	}
	error("redefined symbol: %s", name);
	return newcls;
}

int addglob(char *name, int prim, int type, int scls, int size, int val,
		char *mtext, int init)
{
	int	y = 0;
	int 	oy = 0;
	int	i;

	if (0 == *name)
		y = 0;
	else if (scls != CMEMBER && TSTRUCT != type && TUNION != type && 
			(y = findglob(name)) != 0) 
	{
		if (type != TMACRO && Types[y] != TMACRO) {
			scls = redeclare(name, Stcls[y], scls);
		}
		if (CTYPE == scls) return y;
	} else if ((TSTRUCT == type || TUNION == type) && 
			(y = findstruct(name)) != 0) 
	{
		if (Sizes[y] != 0) {
			error("struct redefinition '%s'\n", name);
		}
		oy = y;
 		y = newglob();
		Names[y] = Names[oy];
		Types[oy] = TPROXY;
		Prims[oy] = y;
		for (i=0; i<Globs; i++) {
			if ((Prims[i] & ~PRIMASK) == oy) {
				Prims[i] = y | (Prims[i] & PRIMASK);
			}
		}
	}
	if (0 == y) {
 		y = newglob();
		Names[y] = globname(name);
	} else if (oy != 0) {
	} else if (TMACRO != type) {
		if (realtype(Prims[y]) != realtype(prim) || Types[y] != type) 
		{
			error("redefinition does not match prior type: %s",
				name);
		}
	} else {
		y = findmac(name);
		if (y) {
			if (strcmp(Stext[y], mtext)) {
				error("redefinition of macro: %s", name);
			}
			return y;	
		}
 		y = newglob();
		Names[y] = globname(name);
	}
	if ((CPUBLIC == scls || CSTATIC == scls) && 0 == init)
		defglob(name, prim, type, size, val, scls);
	Prims[y] = prim;
	Types[y] = type;
	Stcls[y] = scls;
	Sizes[y] = size;
	Vals[y] = val;
	Stext[y] = mtext;
	return y;
}

static void defloc(int prim, int type, int size, int val) {
	gendata();
	if (!(prim & STCMASK)) genlab(val);
	
	if ((prim & STCMASK) == PARRAY) {
		genbss(labname(val), objsize(prim), 1);
	} else if (prim & STCMASK) {
		genbss(labname(val), objsize(prim),1);
	}
	else if (PCHAR == prim || PSCHAR == prim) {
		gendefb(0);
		genalign(1);
	}
	else if (PINT == prim || PUNSIG == prim) {
		gendefw(0);
	}
	else if (PFLOAT == prim) {
		gendefw(0);
	}
	else {
		gendefp(0);
	}
}

int addloc(char *name, int prim, int type, int scls, int size, int val,
		int init)
{
	int	y;

	if (name[0] && (y = findlocrd(name))) {
		if (Stcls[y] != CCURRENT) {
 			y = newloc();
			error("redefinition of: %s", name);
		}
	} else {
 		y = newloc();
	}
	if (CLSTATC == scls && 0 == init) {
		if (val == 0) 
			val = label();
		defloc(prim, type, size, val);
	}
	Names[y] = locname(name);
	Prims[y] = prim;
	Types[y] = type;
	Stcls[y] = scls; /* storage class */
	Sizes[y] = size; /* size in bytes for PSTRUCT or 
			    number of elements for PARRAY */
	Vals[y] = val; /* address on stack for CAUTO or label for CSTATIC */ 
	Stext[y] = NULL;
	return y;
}

void clrlocs(void) {
	Ntop = POOLSIZE;
	Locs = NSYMBOLS;
	Locsbb = NSYMBOLS;
}

int objsize(int prim) {
	int	k = 0, sp;

	sp = prim & STCMASK;
	if (PINT == prim || PUNSIG == prim)
		k = INTSIZE;
	else if (PCHAR == prim || PSCHAR == prim)
		k = CHARSIZE;
	else if (PFLOAT == prim)
		k = INTSIZE;
	else if (VOIDPTR == prim)
		k = PTRSIZE;
	else if (prim & PPTR)
		k = PTRSIZE;
	else if (PARRAY == sp) {
		k = Sizes[prim & ~PRIMASK];
		if (k < 1) k = 1;
       		k *= objsize(Prims[prim & ~PRIMASK]);
	} else if (PSTRUCT == sp) {
		prim = Types[prim & ~PRIMASK] == TPROXY? 
			Prims[prim & ~PRIMASK] : prim;
		k = Sizes[prim & ~PRIMASK];
	} else if (FUNPTR == prim) 
		k = PTRSIZE;
	else if (PFUN == prim) 
		k = PTRSIZE;
	else {
		fatal("unknown object size");
		return -1;
	}
	return k;
}

int rootprim(int p)
{
	if (p & PPTR) {
		return rootprim(Prims[p & ~(STCMASK|PPTR)]);
	}
	switch (p) {
	case VOIDPTR: 	return PVOID;
	case FUNPTR: 	return FUNPTR;
	}
	return p;
}

static int base(int p, char *stars)
{
	if (p & PPTR) {
		strncat(stars, "*", NAMELEN - strlen(stars));	
		return base(Prims[p & ~(STCMASK|PPTR)], stars);
	}
	return p;
}

static char *typename(int p) {
	static char buf[NAMELEN+1];
	char stars[NAMELEN+1];
	
	switch (p & STCMASK) {
	case PSTRUCT:	return Types[p & ~PRIMASK] == TSTRUCT? 
			"STRUCT" : "UNION";
	case PARRAY:	
			buf[0] = '\0';
			strcat(buf, "[]");
			strncat(buf, typename(Prims[p & ~PRIMASK]), 
					NAMELEN - strlen(buf));	
			return buf;
	}
	if (p & PPTR) {
		stars[0] = '\0';
		buf[0] = '\0';
		p = base(p, stars);
		strncat(buf, typename(p), NAMELEN);
		strncat(buf, stars, NAMELEN - strlen(buf));	
		return buf;
	}
	return	PINT    == p? "INT":
		PUNSIG   == p? "UNSIGNED":
		PCHAR   == p? "CHAR":
		PSCHAR   == p? "SCHAR":
		PFLOAT   == p? "FLOAT":
		(PPTR & p) ? "*":
		FUNPTR  == p? "FUN*":
		PANY  == p? "ANY":
		PVOID   == p? "VOID": "n/a";
}

void dumpsyms(char *title, char *sub, int from, int to) {
	int	i;
	int	*p;

	printf("\n===== %s%s =====\n", title, sub);
	printf(	"PRIM    TYPE  STCLS   SIZE  VALUE  NAME [MVAL]/(SIG)\n"
		"------  ----  -----  -----  -----  -----------------\n");
	for (i = from; i < to; i++) {
		printf("%-6s  %s  %s  %5d  %5d  %s",
			typename(Prims[i]),
			TVARIABLE == Types[i]? "VAR ":
				TTARRAY == Types[i]? "ARRY":
				TFUNCTION == Types[i]? "FUN ":
				TCONSTANT == Types[i]? "CNST":
				TUNION == Types[i]? "UNIO":
				TMACRO != Types[i]? "MAC ":
				TSTRUCT == Types[i]? "STCT": "n/a",
			CPUBLIC == Stcls[i]? "PUBLC":
				CEXTERN == Stcls[i]? "EXTRN":
				CSTATIC == Stcls[i]? "STATC":
				CSPROTO == Stcls[i]? "STATP":
				CLSTATC == Stcls[i]? "LSTAT":
				CAUTO   == Stcls[i]? "AUTO ":
				CLABEL  == Stcls[i]? "LABEL":
				CMEMBER == Stcls[i]? "MEMBR":
				CTYPE   == Stcls[i]? "TYPE ": "n/a  ",
			Sizes[i],
			Vals[i],
			Names[i]);
		if (TMACRO == Types[i] && NULL != Stext[i])
			printf(" [\"%s\"]", Stext[i]);
		if (TFUNCTION == Types[i]) {
			printf(" (");
			for (p = (int *)Stext[i]; *p; p++) {
				printf("%s", typename(*p));
				if (p[1]) printf(", ");
			}
			putchar(')');
		}
		putchar('\n');
	}
}
