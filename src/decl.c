/*
 *	NMH's Simple C Compiler, 2011--2022
 *	Declaration parser
 */

#include "defs.h"
#include "data.h"
#include "decl.h"

static char *gettmp(void);
static void flushtmp(void);

static int declarator(int arg, int scls, char *name,  int utype,
			int *pprim, int *psize,
			int *pval, int *pinit, int *paddr);

void decl(int clss, int prim, int utype);

/*
 * enumdecl := { enumlist } ;
 *
 * enumlist :=
 *	  enumerator
 *	| enumerator , enumlist
 *
 * enumerator :=
 *	  IDENT
 *	| IDENT = constexpr
 */

static void enumdecl(int clss) {
	int	v = 0;
	char	name[NAMELEN+1];

	Token = scant();
	if (IDENT == Token)
		Token = scant();
	lbrace();
	while (RBRACE != Token) {
		copyname(name, Text);
		ident();
		if (ASSIGN == Token) {
			Token = scant();
			v = constexpr0();
		}
		if (CAUTO != clss)
			addglob(name, PINT, TCONSTANT, 0, 0, v++, NULL, 0);
		else
			addloc(name, PINT, TCONSTANT, 0, 0, v++, 0);
		if (Token != COMMA)
			break;
		Token = scant();
		if (eofcheck()) return;
	}
	rbrace();
	if (SEMI != Token) 
		decl(clss, PINT,  0);
	else 
		semi();
}

static void initastr(int utype, int prim, int *psize, int *paddr, int lab)
{
	char	*p;
	int	i, d, s, a, j;
	//a = *paddr - s;
	a = 0;
	p = Text + 1;
	i = Value - 1;
	while (i > 0) {
		d = 0;
		j = 0;
		while (j < INTSIZE) {
			i--;
			if (i > 0) {
				d |= (*p) << (8 * j);
				p++;
			}
			j++;
		}
		genlit(d);
		genmovw(lab, a);
		a += INTSIZE;	
	}
	s = Value -1;
	if (0 == *psize) {
		*psize = s;
	} else {
		if (s > *psize) {
			error("String initializer too long", NULL);
		} else {
			s = *psize;
		}
	}
	s = (s + INTSIZE-1) / INTSIZE * INTSIZE;
	*paddr = *paddr - s;
	Token = scant();
}

static void initaarray(int utype, int prim, int *psize, int *paddr, int lab)
{
	char	*p;
	int	i, d, s, a, j;
	s = 0;
	while (Token != RBRACE) {
		singleexpr(prim);
		//fprintf(stderr, "JML iii %s\n", Text);
		if (PCHAR == prim || PSCHAR == prim) {
			genmovb(lab, s + *paddr);
			s += 1;
		} else {
			genmovw(lab, s + *paddr);
			s += BPW;
		}
		if (COMMA == Token)
			Token = scant();
		else
			break;
		if (eofcheck()) {
			*psize = 0;
			return;
		}
	}
	if (*psize == 0) {
		*psize = s;
	} else {
		if (s > *psize) {
			error("Too many initializers", NULL);
		} else {
			s = *psize;
		}
	}
	s = (s + INTSIZE-1) / INTSIZE * INTSIZE;
	*paddr = *paddr - s;
	Token = scant();
}

/*
 * initlist :=
 *	  { const_list }
 *	| STRLIT
 *
 * const_list :=
 *	  constexpr
 *	| constexpr , const_list
 */

static void initlist(char *name, int utype, int oprim, int type, 
		int clss, int *psize, int *pinit, int *paddr) 
{
	int	n = 0;
	char	*p;
	int	i, d, s, a, j;
	int	prim;

	switch (oprim & PRIMASK) {
	case PPTR:
	case PARRAY:
		prim = Prims[oprim & ~PRIMASK];	
	}
	*pinit = label();
	if (CAUTO == clss) {
		if (STRLIT == Token) {
			gentext();
			genlab(*pinit);
			if (PCHAR != prim)
				error("initializer type mismatch!: %s", name);
			s = *psize;
			a = *paddr;
			initastr(utype, prim, &s, &a, *pinit);
			*psize = s;
			genautosize(*pinit, a);
			//*paddr -= s; JML ?
			return;
		}
		gentext();
		genlab(*pinit);
		lbrace();
		s = *psize;
		a = *paddr;
		initaarray(utype, prim, &s, &a, *pinit);
		*psize = s;
		genautosize(*pinit, a);
	//	*paddr -= s; JML FIXME
		return;
	}
	gendata();
	genlab(*pinit);
	genname(name);
	if (STRLIT == Token) {
		if (PCHAR != prim)
			error("initializer type mismatch: %s", name);
		gendefs(Text, Value);
		gendefb(0);
		genalign(Value-1);
		*psize = Value-1;
		Token = scant();
		return;
	}
	lbrace();
	while (Token != RBRACE) {
		n += constinit(prim); /* JML FIXME check type of prim */
		if (COMMA == Token)
			Token = scant();
		else
			break;
		if (eofcheck()) {
			*psize = 0;
			return;
		}
	}
	if (PCHAR == prim || PSCHAR == prim) genalign(n);
	Token = scant();
	if (!n) error("too few initializers", NULL);
	*psize = n;
	return;
}

int primtype(int t, char *s) {
	int	p, y;
	char	sname[NAMELEN+1];

	p = t == CHAR? PCHAR:
		t == SCHAR? PSCHAR:
		t == INT? PINT:
		t == ENUM? PINT:
		t == UNSIGNED? PUNSIG:
		t == FLOAT? PFLOAT:
		t == UNION? PSTRUCT:
		t == STRUCT? PSTRUCT:
		PVOID;
	if (ENUM == t) {
		Token = scant();
		if (IDENT != Token) {
			error("enum name expected: %s", Text);
		}
		return p;
	}
	if (PSTRUCT == p) {
		if (!s) {
			Token = scant();
			copyname(sname, Text);
			s = sname;
			if (IDENT != Token) {
				error("struct/union name expected: %s", Text);
				return p;
			}
		}
		if ((y = findstruct(s)) == 0) 
			y = 0; /* undeclared struct */
		else if (Prims[y] != p)
			error("no such struct/union: %s", s);
		p |= y;
	}
	return p;
}

int usertype(char *s) {
	int	y;

	if ((y = findsym(s)) == 0) return 0;
	return CTYPE == Stcls[y]? y: 0;
}

/*
 * pmtrdecl :=
 *	  ( )
 *	| ( pmtrlist )
 *	| ( pmtrlist , ... )
 *
 * pmtrlist :=
 *	  primtype declarator
 *	| primtype declarator , pmtrlist
 *	| usertype declarator
 *	| usertype declarator , pmtrlist
 *
 * usertype :=
 *	  TYPEDEF_NAME
 */

static int pmtrdecls(void) {
	char	name[NAMELEN+1];
	int	utype, prim, type, size, na, addr;
	int	dummy;

	if (RPAREN == Token)
		return 0;
	na = 0;
	prim = 0;
	addr = 2*BPW;
	for (;;) {
		utype = 0;
		if (na > 0 && ELLIPSIS == Token) {
			Token = scant();
			na = -(na + 1);
			break;
		}
		else if (IDENT == Token &&
			 (utype = usertype(Text)) == 0)
		{
			prim = PINT;
		}
		else {
			if (	CHAR == Token || INT == Token ||
				VOID == Token || UNSIGNED == Token ||
				FLOAT == Token || SCHAR == Token ||
				STRUCT == Token || UNION == Token ||
				ENUM == Token ||
				(IDENT == Token && utype != 0)
			) {
				name[0] = 0;
				prim = utype? Prims[utype]:
					primtype(Token, NULL);
				Token = scant();
				if (RPAREN == Token && prim == PVOID && !na)
					return 0;
			}
			else {
				error("type specifier expected at(1): %s", Text);
				Token = synch(RPAREN);
				return na;
			}
		}
		size = 1;
		type = declarator(1, CAUTO, name, utype, &prim, &size, &dummy,
				&dummy, &addr);
		if ((prim & PRIMASK) == PARRAY) {
			prim = pointerto(prim);
			type = TVARIABLE;
		}
		addloc(name, prim, type, CAUTO, size, addr, 0);
		addr += BPW;
		na++;
		if (COMMA == Token)
			Token = scant();
		else
			break;
	}
	return na;
}

int pointerto(int prim) {
	return PVOID == prim? VOIDPTR:
			PFUN == prim? FUNPTR:
			findptr(prim);
}

static void arraydecl(int *pprim, int *psize)
{
	int prim;
	int size;
	int dummy;

	prim = *pprim;
	size = constexpr0();
	if (size < 1) {
		error("invalid array size %s", Text);
		size = 0;
	}
	rbrack();
	if (LBRACK == Token) {
		Token = scant();
		arraydecl(&prim, &dummy);
	}
	*pprim = findarray(prim, size);
	*psize = size;
}

static node *dcl(int pmtr, char *name, int *pprim);
static void walkdcl(node *n, int pmtr, char *name, int *pprim);

/* arrdcl : ('[' constexpr? ']')*
 */
static node *arrdcl(int *pprim)
{
	int size = 0;
	node *n = NULL;

	Token = scant();
	if (RBRACK == Token) {
		Token = scant();
	} else {
		size = constexpr0();
		if (size < 1) {
			error("invalid array size %s", Text);
		}
		rbrack();
	}
	if (LBRACK == Token) {
		n = arrdcl(pprim);
	}
	n = mkunop1(OP_ARRAY, size, n);
	return n;
}

node *pardecl(void) {
	node 	*n = NULL;
	node	*nd;
	char	name[NAMELEN+1];
	int	utype, prim, type, size, addr, val, ini;
	int	stat, extn;
	int	pbase, rsize;
	int	i;
	int	scls;

	Token = scant();

	if (RPAREN == Token) {
		Token = scant();
		return NULL;
	}

	utype = 0;
	while (INT == Token || UNSIGNED == Token || 
		CHAR == Token || VOID == Token || FLOAT == Token ||
		ENUM == Token || SCHAR == Token ||
		STRUCT == Token || UNION == Token ||
		(IDENT == Token && (utype = usertype(Text)) != 0)
	) {
		if (utype) {
			prim = Prims[utype];
			Token = scant();
		} else {
			prim = primtype(Token, NULL);
			Token = scant();
		}
		size = 1;
		ini = 0;
		val = 0;
		type = TVARIABLE;

		nd = dcl(1, name, &prim); 
		walkdcl(nd, 1, name, &prim);
	
		n = mkunop1(OP_ARG, prim, n);
		if (COMMA == Token)
			Token = scant();
		else
			break;
		utype = 0;
	}
	if (RPAREN != Token) {
		error("')' expected found '%s'", Text);
	}
	Token = scant();
	return n;
}

/* dirdcl: name | 
 * 	('(' dcl ')') | 
 * 	(dirdcl '(' ')') | 
 * 	(dirdcl '[' constexpr? ']')
 */
static node *dirdcl(int pmtr, char *name, int *pprim)
{
	int i;
	int size;
	node *n = NULL;

	if (LPAREN == Token) {
		Token = scant();
		n = dcl(pmtr, name, pprim);
		if (RPAREN != Token) {
			error("missing ')' at : %s", Text);
		}
		n = mkunop(OP_PAREN, n);
		Token = scant();
	} else if (IDENT != Token) {
		if (!pmtr) {
			error("expected name or '(' at : %s", Text);
		}
		name[0] = 0;
		n = mkunop(OP_IDENT, NULL);
	} else {
		copyname(name, Text);
		n = mkunop(OP_IDENT, NULL);
		Token = scant();
	}
	if (LPAREN == Token) {
	n = mkbinop(OP_FUN, n, pardecl());
	} else if (LBRACK == Token) {
		n = mkbinop(OP_GLUE, n, arrdcl(pprim));
	}
	return n;
}

/* dcl: '*'* dirdcl
 */
static node *dcl(int pmtr, char *name, int *pprim)
{
	int stars = 0;
	node *n = NULL;

	while (STAR == Token) {
		Token = scant();
		stars++;
	}
	n = dirdcl(pmtr, name, pprim);
	if (stars > 0) {
		n = mkunop1(OP_POINTER, stars, n);
	}	
	return n;
}

static void walkdcl(node *n, int pmtr, char *name, int *pprim)
{
	int s;
	int p;

	p =  (O_debug & D_DEFI) && !pmtr;
	if (NULL == n) return;
	switch (n->op) {
	case OP_POINTER:
		s = n->args[0];
		while (s > 0) {
			if (p)	fprintf(stderr, "*");	
			*pprim = pointerto(*pprim);
			s--;
		}
		walkdcl(n->left, pmtr, name, pprim);
		break;
	case OP_ARRAY:
		walkdcl(n->left, pmtr, name, pprim);
		s = n->args[0];
		if (p) fprintf(stderr, "[%d]",s);	
		*pprim = findarray(*pprim, s);
		break;
	case OP_FUN:
		if (p) fprintf(stderr, "(");	
		*pprim = PFUN;
		//*ptype = TFUNCTION;
		walkdcl(n->right, pmtr, name, pprim);
		if (p) fprintf(stderr, ")");	
		walkdcl(n->left, pmtr, name, pprim);
		break;
	case OP_ARG:
		s = n->args[0];
		walkdcl(n->left, pmtr, name, pprim);
		if (p) fprintf(stderr, " %d, ",s);	
		break;
	case OP_PAREN:
		if (p) fprintf(stderr, " ");
		walkdcl(n->left, pmtr, name, pprim);
		if (p) fprintf(stderr, " ");
		break;	
	case OP_IDENT:
		if (p) fprintf(stderr, " %s", name);	
		break;
	case OP_GLUE:
		if (p) fprintf(stderr, " ");	
		walkdcl(n->right, pmtr, name, pprim);
		if (p) fprintf(stderr, " ");	
		walkdcl(n->left, pmtr, name, pprim);
	
		if (p) fprintf(stderr, " ");	
		break;
	default:
		error("unknown op in declaration in %s", name);
	}
}

static void dcla(int pmtr, char *name, int *pprim)
{
	int ntop;
	node *n;

	ntop = Ndtop;
	n = dcl(pmtr, name, pprim);
	walkdcl(n, pmtr, name, pprim);
	
	if (O_debug & D_DEFI)	
		fprintf(stderr, "\n");	
	Ndtop = ntop;
}

/*
 * declarator :=
 *	  IDENT
 *	| * IDENT
 *	| * * IDENT
 *	| * IDENT [ constexpr ]
 *	| IDENT [ constexpr ]
 *	| IDENT = constexpr
 *	| IDENT [ ] = initlist
 *	| IDENT pmtrdecl
 *	| IDENT [ ]
 *	| * IDENT [ ]
 *	| ( * IDENT ) ( )
 */


static int declarator(int pmtr, int scls, char *name, int utype, int *pprim, int *psize,
			int *pval, int *pinit, int *paddr)
{
	int	type = TVARIABLE;
	int	ptrptr = 0;
	char	*unsupp;
	int	par;
	int	fundecl = 0;
	int	prim;

	prim = *pprim;

	unsupp = "unsupported typedef syntax";
	while (STAR == Token) {
		Token = scant();
		*pprim = pointerto(*pprim);
	}
	
	if (LPAREN == Token) {
		name[0] = 0;
		dirdecl(pprim, name, 0, pmtr);
		if (!pmtr && RPAREN == Token) {
			//Token = scant();
		}
	} else if (IDENT != Token) {
		if (!pmtr) {
			error("missing identifier at (1): %s", Text);
		}
		name[0] = 0;
	}
	else {
		copyname(name, Text);
		Token = scant();
	}
	
	if (CMEMBER == scls && COLON == Token) {
		/* bit field */
		Token = scant();
		constexpr0();
	}
	

	if (!pmtr && ASSIGN == Token) {
		if (CTYPE == scls)
			error(unsupp, NULL);
		Token = scant();
		addloc(name, *pprim, type, CCURRENT, 1, 0, 0);
		constexpr2(name, utype, *pprim, type, scls, psize, pval, pinit, paddr);
	}
	else if (!pmtr && LPAREN == Token) {
		if (CTYPE == scls)
			error(unsupp, NULL);
		Token = scant();
		clrlocs();
		*psize = pmtrdecls();
		rparen();
		return TFUNCTION;
	}
	else if (LBRACK == Token) {
		Token = scant();
		if (RBRACK == Token) {
			*psize = 0;
			if (CTYPE == scls)
				error(unsupp, NULL);
			Token = scant();
			if (LBRACK == Token) {
				Token = scant();
				arraydecl(pprim, psize);
			}
			if (pmtr) {
				*pprim = pointerto(*pprim);
				*psize = 0;
			}
			else {
				type = TVARIABLE;
				*psize = 1;
				prim = *pprim;
				*pprim = findarray(*pprim, 0);
				if (ASSIGN == Token) {
					addloc(name, *pprim, type, CCURRENT, *psize, 0, 0);
					*psize = 0;
					Token = scant();
					initlist(name, utype, *pprim, type, 
							scls, psize, 
							pinit, paddr);
					*pprim = findarray(prim, *psize);
				}
				else if (CEXTERN != scls) {
					error("automatically-sized array"
						" lacking initialization: %s",
						name);
				}
			}
		}
		else {
			*psize = 1;
			arraydecl(pprim, psize);
			type = TVARIABLE;
			if (ASSIGN == Token) {
				Token = scant();
				initlist(name, utype, *pprim, type, 
					scls, psize, pinit, paddr);
				*pval = *pinit;
			} else {
				*pinit = 0;
			}
		}
	} else if (IDENT == Token) {
		if (scls == CTYPE && PVOID == *pprim) {
			*pprim = PINT; /* enum ? */
			copyname(name, Text);
			Token = scant();
		}
	}
	if (PVOID == *pprim)
		error("'void' is not a valid type: %s", name);
	return type;
}

/*
 * localdecls :=
 *        ldecl
 *      | ldecl localdecls
 *
 * ldecl :=
 *	  primtype ldecl_list ;
 *	| usertype ldecl_list ;
 *	| lclass primtype ldecl_list ;
 *	| lclass ldecl_list ;
 *	| enum_decl
 *	| struct_decl
 *
 * lclass :=
 *	| AUTO
 *	| EXTERN
 *	| REGISTER
 *	| STATIC
 *	| VOLATILE
 *
 * ldecl_list :=
 *	  declarator
 *	| declarator , ldecl_list
 */
int localdecls(void) {
	char	name[NAMELEN+1];
	int	utype, prim, type, size, addr, ini;
	int	stat, extn;
	int	pbase, rsize;
	int	i;
	int	scls;

	addr = Thislsize;

	utype = 0;
	while ( AUTO == Token || EXTERN == Token || REGISTER == Token ||
		STATIC == Token || VOLATILE == Token ||
		INT == Token || UNSIGNED == Token || 
		CHAR == Token || VOID == Token || FLOAT == Token ||
		ENUM == Token || SCHAR == Token ||
		STRUCT == Token || UNION == Token ||
		(IDENT == Token && (utype = usertype(Text)) != 0)
	) {
		scls = CAUTO;
		if (ENUM == Token) {
			enumdecl(CAUTO);
			continue;
		}
		extn = stat = 0;
		if (AUTO == Token || REGISTER == Token || STATIC == Token ||
			VOLATILE == Token || EXTERN == Token
		) {
			stat = STATIC == Token;
			extn = EXTERN == Token;
			Token = scant();
			if (	INT == Token || CHAR == Token ||
				VOID == Token || UNSIGNED == Token ||
				FLOAT == Token || SCHAR == Token ||
				STRUCT == Token || UNION == Token 
			) {
				prim = primtype(Token, NULL);
				Token = scant();
			}
			else if (IDENT == Token && 
					(utype = usertype(Text)) != 0)
			{
				prim = Prims[utype];
				Token = scant();
			}
			else
				prim = PINT;
		}
		else if (utype) {
			prim = Prims[utype];
			Token = scant();
		}
		else {
			prim = primtype(Token, NULL);
			Token = scant();
		}
		pbase = prim;
		if (stat) {
			scls = CLSTATC;
		} else if (extn) {
			scls = CEXTERN;
		}
		for (;;) {
			prim = pbase;
			if (eofcheck()) {
			       	return addr - Thislsize;
			}
			size = 1;
			ini = 0;
			type = TVARIABLE;
			dcla(0, name, &prim);
			if (ASSIGN == Token) {
				Token = scant();
		//fprintf(stderr, "JML assiiii %s\n", name);
				addloc(name, prim, type, CCURRENT, 1, 0, 0);
				constexpr3(name, prim, scls, 
						&size, &ini, &addr);
			}

			if (stat) {
				addloc(name, prim, type, CLSTATC, size,
					ini, ini);
			}
			else if (extn) {
				addloc(name, prim, type, CEXTERN, size, 0, 0);
			}
			else {
				if (ini == 0) {
					
					if ((prim & PRIMASK) == PARRAY &&
						Sizes[prim & ~PRIMASK] < 1)
					{
						error("unknown array size", 
								NULL);
					}
					rsize = objsize(prim);
					rsize = (rsize + INTSIZE-1) / INTSIZE * INTSIZE;
					addr -= rsize;
				}
				addloc(name, prim, type, CAUTO, size, addr, 0);
			}
			if (COMMA == Token)
				Token = scant();
			else
				break;
		}
		semi();
		utype = 0;
	}
	return addr - Thislsize;
}

static int intcmp(int *x1, int *x2) {
	while (*x1 && realtype(*x1) == realtype(*x2)) {
		x1++, x2++;
	}
	return *x1 - *x2;
}

static void signature(int fn, int from, int to) {
	int	types[MAXFNARGS+1], i;
	if (to - from > MAXFNARGS)
		error("too many function parameters", Names[fn]);
	for (i=0; i<MAXFNARGS && from < to; i++) {
		types[i] = Prims[--to];
	}
	types[i] = 0;
	if (NULL == Stext[fn]) {
		Stext[fn] = galloc((i+1) * sizeof(int), 1);
		memcpy(Stext[fn], types, (i+1) * sizeof(int));
	}
	else if (intcmp((int *) Stext[fn], types))
		error("declaration does not match prior prototype: %s",
			Names[fn]);
}

/*
 * decl :=
 *	  declarator { localdecls stmt_list }
 *	| decl_list ;
 *
 * decl_list :=
 *	  declarator
 *	| declarator , decl_list
 */

void decl(int clss, int prim, int utype) {
	char	name[NAMELEN+1];
	int	pbase, type, size = 0, val, init;
	int	lsize;
	int	addr = 0;
	pbase = prim;
	for (;;) {
		prim = pbase;
		val = 0;
		init = 0;
		type = declarator(0, clss, name, utype, &prim, &size, 
				&val, &init, &addr);
		if (TFUNCTION == type) {
			clss = clss == CSTATIC? CSPROTO: CEXTERN;
			Thisfn = addglob(name, prim, type, clss, size, 0,
					NULL, 0);
			signature(Thisfn, Locs, NSYMBOLS);
			if (LBRACE == Token) {
				clss = clss == CSPROTO? CSTATIC:
					clss == CEXTERN? CPUBLIC: clss;
				Thisfn = addglob(name, prim, type, clss, size,
					0, NULL, 0);
				Token = scant();
				gentext();
				if (CPUBLIC == clss) genpublic(name);
				genaligntext();
				genentry(name);
				Retlab = label();
				Thislsize = -(BPW*2); /* is negative */
				Thismsize = -(BPW*2);
				compound(0);
				genlab(Retlab);
				genstacksize(name, Thismsize);
				genstack(-Thismsize);
		
				genexit();
				Token = scant();
				if (O_debug & D_LSYM)
					dumpsyms("LOCALS: ", name, Locs,
						NSYMBOLS);
			}
			else {
				semi();
			}
			clrlocs();
			return;
		}
		if (CEXTERN == clss && init > 0) {
			error("initialization of 'extern': %s", name);
		}
		addglob(name, prim, type, clss, size, val, NULL, init);
		if (COMMA == Token)
			Token = scant();
		else
			break;
	}
	semi();
}

static void fixtdef(int old, int ny, int m)
{
	int     i, j, t;
        for (i=0; i<Globs; i++) {
		if (CTYPE == Stcls[i] || TFUNCTION == Types[i]) {
			if ((Prims[i] & ~PRIMASK) == old) {
				Prims[i] = m | ny;
			}
        
		}
	}
	Types[old] = TPROXY; 
	Names[old] = ""; 
	Prims[old] = m | ny; 
}

/*
 * structdecl :=
 *	  STRUCT IDENT { member_list } opt_decl ;
 *	| UNION IDENT { member_list } opt_decl ;
 *	| STRUCT { member_list } opt_decl ;
 *	| UNION { member_list } opt_decl ;
 *
 * opt_decl :=
 *      | decl
 *
 * member_list :=
 *	  primtype mdecl_list ;
 *	| primtype mdecl_list ; member_list
 *	| usertype mdecl_list ;
 *	| usertype mdecl_list ; member_list
 *
 * mdecl_list :=
 *	  declarator
 *	| declatator , mdecl_list
 */

void structdecl(int clss, int uniondecl) {
	int	utype, base, prim, size, dummy, type, addr = 0;
	char	name[NAMELEN+1], sname[NAMELEN+1];
	int	y, oy, usize = 0;

	Token = scant();
	if (IDENT == Token) {
		copyname(sname, Text);
		Token = scant();
	}
	else {
		sname[0] = 0;
	}
	if (sname[0]) 
		y = findstruct(sname);
	else
		y = 0;
	if (Token != LBRACE) {
		if (y == 0) {
			addglob(sname, PSTRUCT, uniondecl? TUNION: TSTRUCT,
				CMEMBER, 0, 0, NULL, 0);
		}
		prim = primtype(uniondecl? UNION: STRUCT, sname);
		decl(clss, prim, 0);
		return;
	}
	if (y != 0) {
		if (Sizes[y] != 0) {
			error("refinition of struct/union %s", sname);
			return;
		}
		oy = y;
		y = addglob(sname, PSTRUCT, uniondecl? TUNION: TSTRUCT, 
			CMEMBER, 0, 0, NULL, 0);
		fixtdef(oy, y, PSTRUCT);
	} else {
		y = addglob(sname, PSTRUCT, uniondecl? TUNION: TSTRUCT, 
			CMEMBER, 0, 0, NULL, 0);
	}
	Token = scant();
	utype = 0;
	while (	INT == Token || CHAR == Token || VOID == Token ||
		STRUCT == Token || UNION == Token || UNSIGNED == Token ||
		FLOAT == Token || SCHAR == Token || ENUM == Token ||
		(IDENT == Token && (utype = usertype(Text)) != 0)
	) {
		base = utype? Prims[utype]: primtype(Token, NULL);
		size = 0;
		Token = scant();
		for (;;) {
			if (eofcheck()) return;
			prim = base;
			type = declarator(1, CMEMBER, name, utype, &prim, &size,
						&dummy, &dummy, &dummy);
			addglob(name, prim, type, CMEMBER, size, addr,
				NULL, 0);
			size = objsize(prim);
			if (size < 1)
				error("size of struct/union member"
					" is unknown: %s",
					name);
			if (uniondecl) {
				usize = size > usize? size: usize;
			}
			else {
				addr += size;
				addr = (addr + INTSIZE-1) / INTSIZE * INTSIZE;
			}
			if (Token != COMMA) break;
			Token = scant();
		}
		semi();
		utype = 0;
	}
	rbrace();
	Sizes[y] = uniondecl? usize: addr;
	if (Token != SEMI)
		decl(clss, Prims[y] | y, y);
	else
		semi();
}

/*
 * typedecl :=
 *	  TYPEDEF primtype decl
 *	| TYPEDEF usertype decl
 *	| TYPEDEF structdecl
 */

void typedecl(void) {
	int	utype, prim;

	Token = scant();
	if (STRUCT == Token || UNION == Token) {
		structdecl(CTYPE, UNION == Token);
	}
	else if (ENUM == Token) {
		enumdecl(CTYPE);
	}
	else if ((utype = usertype(Text)) != 0) {
		Token = scant();
		decl(CTYPE, Prims[utype], utype);
	}
	else {
		prim = primtype(Token, NULL);
		Token = scant();
		decl(CTYPE, prim, 0);
	}
}

/*
 * top :=
 *	  ENUM enumdecl
 *	| decl
 *	| primtype decl
 *	| storclass decl
 *	| storclass primtype decl
 *	| typedecl
 *	| usertype decl
 *	| storclass usertype decl
 *
 * storclass :=
 *	  EXTERN
 *	| STATIC
 */

void top(void) {
	int	utype, prim, clss = CPUBLIC;

	switch (Token) {
	case EXTERN:	clss = CEXTERN; Token = scant(); break;
	case STATIC:	clss = CSTATIC; Token = scant(); break;
	case VOLATILE:	Token = scant(); break;
	}
	switch (Token) {
	case ENUM:
		enumdecl(CEXTERN);
		break;
	case TYPEDEF:
		typedecl();
		break;
	case STRUCT:
	case UNION:
		structdecl(clss, UNION == Token);
		break;
	case SCHAR:
	case CHAR:
	case INT:
	case UNSIGNED:
	case FLOAT:
	case VOID:
		prim = primtype(Token, NULL);
		Token = scant();
		decl(clss, prim, 0);
		break;
	case IDENT:
		if ((utype = usertype(Text)) != 0) {
			Token = scant();
			decl(clss, Prims[utype], utype);
		}
		else
			decl(clss, PINT, 0);
		break;
	default:
		error("type specifier expected at: %s", Text);
		Token = synch(SEMI);
		break;
	}
}

static void stats(void) {
	printf(	"Memory usage: "
		"Symbols: %5d/%5d, "
		"Names: %5d/%5d, "
		"Nodes: %5d/%5d\n",
		Globs, NSYMBOLS,
		Nbot, POOLSIZE,
		Ndmax, NODEPOOLSZ);
}

void defarg(char *s) {
	char	*p;
	char b[NAMELEN+1];
	int i;

	if (NULL == s) return;
	p = s;
	i = 0;
	while (*p && *p != '=' && i <= NAMELEN) {
		b[i] = *p;
		p++;
		i++;
	}
	b[i] = 0;
	if (*p == '=') p++;
	addglob(b, 0, TMACRO, 0, 0, 0, globname(p), 0);
}

void program(char *name, FILE *in, FILE *out, FILE *tmp, char *def) {
	
	init();
	while (def[0]) {
		defarg(def);
		def = def + strlen(def) + 1;
	}
	Infile = in;
	Outfile = out;
	Outfile1 = out;
	Tmpfile = tmp;
	File = Basefile = name;

	if (O_preponly) {
		Token = scan();
		while (XEOF != Token) {
			pretop();
		}
		flushtmp();
	} else {
		genprelude();
		Token = scant();
		while (XEOF != Token) {
			Ndtop = 1;
			top();
		}
		flushtmp();
		genpostlude();
	}
	if (O_debug & D_GSYM) dumpsyms("GLOBALS", "", 1, Globs);
	if (O_debug & D_STAT) stats();
}



static char *gettmp(void)
{
	int l;
	char *b = NULL;
	if (Outfile1 && Tmpfile && (l = ftell(Tmpfile)) > 0) {
		b = malloc(l + 1);
		rewind(Tmpfile);
		fread(b,  l, 1, Tmpfile);
		rewind(Tmpfile);
		b[l] = '\0';
	} 
	return b;
}

static void flushtmp(void)
{
	char *b;
	while (Tmpnbuf > 0) {
		Tmpnbuf--;
		poptmp();
	}
	b = gettmp();
	if (b) {
		if (Outfile1) {
			fwrite(b, strlen(b), 1, Outfile1);
		}
		free(b);
	}
}

void pushtmp(void)
{
	char *b;

	if (Tmpnbuf >= MAXRECURS-1) {
		fatal("too many recusion buffers");
	}
	b = gettmp();
	Tmpbuf[Tmpnbuf] = b;
	Tmpnbuf++;
}


void poptmp(void)
{
	char *b, *b1;
	if (Tmpnbuf < 1) {
		return;
	}
	b1 = gettmp();
	Tmpnbuf--;
	b = Tmpbuf[Tmpnbuf];
	if (b) {
		if (Tmpfile) {
			fwrite(b, strlen(b), 1, Tmpfile);
		}
		free(b);
	}
	if (b1) {
		if (Tmpfile) {
			fwrite(b1, strlen(b1), 1, Tmpfile);
		}
		free(b1);
	}
}

