/*
 *	NMH's Simple C Compiler, 2011,2014
 *	Miscellanea
 */

#include "defs.h"
#include "data.h"
#include "decl.h"

void init(void) {
	char *defines;
	char buf[10];
	Tmpnbuf = 0;
	Line = 1;
	Putback = '\n';
	Rejected = -1;
	Errors = 0;
	Mp = 0;
	Mpbase = 0;
	Expandmac = 1;
	Syntoken = 0;
	Isp = 0;
	Inclev = 0;
	Globs = 0;
	Locs = NSYMBOLS;
	Locsbb = NSYMBOLS;
	Nbot = 0;
	Ntop = POOLSIZE;
	Ndmax = 0;
	Bsp = 0;
	Csp = 0;
	Q_type = empty;
	Q_cmp = cnone;
	Q_bool = bnone;
	addglob("", 0, 0, 0, 0, 0, NULL, 0);
	/* must match primitive type in defs.h */
	addglob("char", 0, 0, 0, 0, 1, NULL, 0);
	addglob("signed char", 0, 0, 0, 0, 1, NULL, 0);
	addglob("int", 0, 0, 0, 0, 4, NULL, 0);
	addglob("unsigned", 0, 0, 0, 0, 4, NULL, 0);
	addglob("float", 0, 0, 0, 0, 4, NULL, 0);
	addglob("void", 0, 0, 0, 0, 0, NULL, 0);
	addglob("void*", 0, 0, 0, 0, 4, NULL, 0);
	addglob("()", 0, 0, 0, 0, 4, NULL, 0);
	addglob("(any)", 0, 0, 0, 0, 4, NULL, 0);
	addglob("(*)", 0, 0, 0, 0, 4, NULL, 0);
	if (Globs != FUNPTR + 1) {
		fprintf(stderr, "Panic!");
		exit(-1);
	}
	defines = DEFINES;
	while (defines[0]) {
		defarg(defines);
		defines += strlen(defines) + 1;
	}
	defarg("__LINE__=-1");
	defarg("__FILE__=\"not set\"");
	defarg("__DATE__=\"not set\"");
	defarg("__TIME__=\"not set\"");
	Funpp = findptr(FUNPTR);
	Intptr = findptr(PINT);
	Unsigptr = findptr(PUNSIG);
	Floatptr = findptr(PFLOAT);
	Charptr = findptr(PCHAR);
	Scharptr = findptr(PSCHAR);
	Infile = stdin;
	File = "(stdin)";
	Basefile = NULL;
	Outfile = stdout;
	opt_init();
	label();
}

int chrpos(char *s, int c) {
	char	*p;

	p = strchr(s, c);
	return p? p-s: -1;
}

void copyname(char *name, char *s) {
	strncpy(name, s, NAMELEN);
	name[NAMELEN] = 0;
}

void match(int t, char *what) {
	if (Token == t) {
		Token = scant();
	}
	else {
		error("%s expected", what);
	}
}

void lparen(void) {
	match(LPAREN, "'('");
}

void rparen(void) {
	match(RPAREN, "')'");
}

void lbrace(void) {
	match(LBRACE, "'{'");
}

void rbrace(void) {
	match(RBRACE, "'}'");
}

void rbrack(void) {
	match(RBRACK, "']'");
}

void semi(void) {
	match(SEMI, "';'");
}

void colon(void) {
	match(COLON, "':'");
}

void ident(void) {
	match(IDENT, "identifier");
}

int eofcheck(void) {
	if (XEOF == Token) {
		error("missing '}' at end of file", NULL);
		return 1;
	}
	return 0;
}

int realtype(int p) {
	int u;
	u = p & ~PRIMASK;
	if (u > FUNPTR && u < NSYMBOLS && TPROXY == Types[u]) {
		/* late declared struct */
		p = Prims[u];
		p = realtype(p);
	}
	return p;
}

int inttype(int p) {
	p = realtype(p);
	return PANY == p || PINT == p || PCHAR == p || PUNSIG == p || PSCHAR == p;
}

int comptype(int p) {
	p = PRIMASK & realtype(p);
	return p == PSTRUCT;
}

void notvoid(int p) {
	p = realtype(p);
	if (PVOID == p)
		error("void value in expression", NULL);
}
