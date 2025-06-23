/*
 *	NMH's Simple C Compiler, 2011--2022
 *	Expression parser
 */

#include "defs.h"
#include "data.h"
#include "decl.h"
#include "prec.h"

static node *asgmnt(int *lv);
static node *cast(int *lv);
static node *cond3(int *lv);
static node *exprlist(int *lv, int ckvoid);
static void initaarray(int prim, int *psize, int *paddr, int lab);

static node *rvalue(node *n, int *lv) {
	if (lv[LVADDR]) {
		lv[LVADDR] = 0;
		return mkunop2(OP_RVAL, lv[LVPRIM], lv[LVSYM], n);
	}
	else {
		return n;
	}
}

/*
 * primary :=
 *	  IDENT
 *	| INTLIT
 *	| string
 *	| ARGC
 *	| ( expr )
 *
 * string :=
 *	  STRLIT
 *	| STRLIT string
 */

static node *primary(int *lv) {
	node	*n = NULL;
	int	lv2[LV];
	int	y, lab, k, addr, size, prim;
	char	name[NAMELEN+1];

	lv2[LVPRIM] = lv[LVPRIM];
	lv2[LVSYM] = lv[LVSYM];
	lv2[LVADDR] = lv[LVADDR];
	lv[LVPRIM] = lv[LVSYM] = lv[LVADDR] = 0;
	switch (Token) {
	case IDENT:
		y = findsym(Text);
		copyname(name, Text);
		Token = scant();
		if (!y) {
			if (LPAREN == Token) {
				y = addglob(name, PINT, TFUNCTION, CEXTERN,
					-1, 0, NULL, 0);
			}
			else {
				error("undeclared variable: %s", name);
				y = addloc(name, PINT, TVARIABLE, CAUTO,
					0, 0, 0);
			}
		}
		lv[LVSYM] = y;
		lv[LVPRIM] = Prims[y];
		if (TFUNCTION == Types[y]) {
			if (LPAREN != Token) {
				lv[LVPRIM] = FUNPTR;
				n = mkleaf(OP_ADDR, y);
			}
			return n;
		}
		if (TCONSTANT == Types[y]) {
			return mkleaf(OP_LIT, Vals[y]);
		}
		if ((Prims[y] & STCMASK) == PARRAY) {
			n = mkleaf(OP_ADDR, y);
			lv[LVPRIM] = pointerto(Prims[Prims[y] & ~PRIMASK]);
			return n;
		}
		if (CTYPE == Stcls[y]) {
			error("invalid use of typedef type", Text);
		}
		if (comptype(Prims[y])) {
			n = mkleaf(OP_ADDR, y);
			lv[LVSYM] = 0;
			return n;
		}
		n = mkleaf(OP_IDENT, y);
		lv[LVADDR] = 1;
		return n;
	case INTLIT:
		n = mkleaf(OP_LIT, Value);
		lv[LVPRIM] = PINT;
		Token = scant();
		return n;
	case UNSIGLIT:
		n = mkleaf(OP_LIT, Value);
		lv[LVPRIM] = PUNSIG;
		Token = scant();
		return n;

	case STRLIT:
	//	gentmp(1);
		gentext();
		gendata();
		lab = label();
		genlab(lab);
		k = 0;
		while (STRLIT == Token) {
			gendefs(Text, Value);
			k += Value-2;
			Token = scant();
		}
		gendefb(0);
		genalign(k+1);
	//	gentmp(0);
		n = mkleaf(OP_LDLAB, lab);
		lv[LVPRIM] = Charptr;
		return n;
	case LPAREN:
		Token = scant();
		n = exprlist(lv, 0);
		rparen();
		return n;
	case LBRACE:
	//	gentmp(1);
		gentext();
		lab = label();
		genlab(lab);
		lbrace();
		size = -1;
		prim = lv2[LVPRIM];
		if ((prim & PRIMASK) ==  PARRAY) {
			size = Sizes[prim & ~PRIMASK];
			prim = Prims[prim & ~PRIMASK];	
		}
		initaarray(prim, &size, &Thislsize, lab);
		genautosize(lab, Thislsize);
		if (Thismsize > Thislsize) { /* Thislsize is negative */
			Thismsize = Thislsize;
		}
		sprintf(name, "*L%d", lab);
		lv2[LVSYM] = addloc(name, prim, TVARIABLE, CAUTO, 0, Thislsize, 0);
		if ((lv2[LVPRIM] & PRIMASK) ==  PARRAY) {
			n = mkunop1(OP_ADDR, lv2[LVSYM], n);
		} else { 
			n = mkunop2(OP_RVAL, lv2[LVPRIM], lv2[LVSYM], n);
		}
		lv[LVPRIM] = lv2[LVPRIM];
		return n;
	default:
		error("syntax error at: %s", Text);
		Token = synch(SEMI);
		return NULL;
	}
}

int typematch(int p1, int p2) {
	int pp1, pp2;
	p1 = realtype(p1);
	p2 = realtype(p2);
	if ((p1 & ~PRIMASK) == PANY || (p2 & ~PRIMASK) == PANY) {
		return 1;
	}
	if (p1 == p2) return 1;
	if (inttype(p1) && inttype(p2)) return 1;
	if (!inttype(p1) && VOIDPTR == p2) return 1;
	if (VOIDPTR == p1 && !inttype(p2)) return 1;
	if ((p2 & PRIMASK) == PPTR && (p1 & PRIMASK) == PPTR) {
		pp1 = Prims[p1 & ~PRIMASK];
		pp2 = Prims[p1 & ~PRIMASK];
		if (pp1 == pp2) return 1;
	/*	
		if ((pp1 & PRIMASK) == PARRAY && (pp2 & PRIMASK) == PPTR &&
			(pp1 & ~PRIMASK) == (pp2 & ~PRIMASK)) return 1;
		if ((pp2 & PRIMASK) == PARRAY && (pp1 & PRIMASK) == PPTR &&
			(pp1 & ~PRIMASK) == (pp2 & ~PRIMASK)) return 1;
			*/
	//fprintf(stderr, "JML %x %x error %x %x\n", pp1 , pp2, 
	//	pp1 & ~PRIMASK, pp2 & ~PRIMASK);

	}
	if ((p2 & PRIMASK) == PPTR && ((p1 & PRIMASK) == PARRAY || 
		(p1 & PRIMASK) == PSTRUCT) && 
		Prims[p1 & ~PRIMASK] == Prims[p2 & ~PRIMASK]) return 1;
	if ((p1 & PRIMASK) == PPTR && ((p2 & PRIMASK) == PARRAY || 
		 (p2 & PRIMASK) == PSTRUCT) && 
		Prims[p2 & ~PRIMASK] == Prims[p1 & ~PRIMASK]) return 1;
/*	
	if ((p2 & PRIMASK) == PPTR && ((p1 & PRIMASK) == PARRAY || 
		(p1 & PRIMASK) == PSTRUCT) && 
		p1 == Prims[p2 & ~PRIMASK]) return 1;
	if ((p1 & PRIMASK) == PPTR && ((p2 & PRIMASK) == PARRAY || 
		 (p2 & PRIMASK) == PSTRUCT) && 
		p2 == Prims[p1 & ~PRIMASK]) return 1;*/
//	fprintf(stderr, "JML %x %x Typematch error %x %x %d %d\n", p1 &PRIMASK, p2 &PRIMASK, p1, p2, Prims[p1 & ~PRIMASK], Prims[p2 & ~PRIMASK]);
	dumpsyms("hello", "world", p1 & ~PRIMASK, (p1 & ~PRIMASK) + 1);
	dumpsyms("hello", "world2", p2 & ~PRIMASK, (p2 & ~PRIMASK) + 1);
	return 0;
}

static node *funorarr(int size)
{
	node *n = NULL;
	node *n2 = NULL;
	int p;
	
	if (LPAREN == Token) {
		n = mkleaf(OP_CALL, 0);
                p = 1;
                Token = scant();
                while (XEOF != Token && p > 0) {
                        if (LPAREN == Token) {
                                p++;
                        } else if (RPAREN == Token) {
                                p--;
                        }               
                        Token = scant();
		}
	} else if (LBRACK == Token) {
       		Token = scant();
                if (RBRACK == Token) {
			if (size < 0) {
                       		error("[][] is illegal %s", Text);
			}
			size = -1;
			Token = scant();
			n = mkleaf(OP_ARRAY, 0);
                } else {
                	size = constexpr0();
                        if (size < 1) {
                       		error("invalid array size %s", Text);
                                size = 0;
                        }
			n = mkleaf(OP_ARRAY, size);
                        rbrack();
                }
                if (LBRACK == Token) {
			n2 = funorarr(size); 
			if (n) {
				if (n2) {
					n = mkbinop(OP_GLUE, n2, n);
				}
			} else {
				n = n2;
			}
		}
        }
	return n;
}

static node *pardecl(char *name, int cast, int *eat)
{
	node *n = NULL;
	node *n2 = NULL;
	int pointer = 0;
	int dummy;

        while (STAR == Token) {
                Token = scant();
                pointer++;
        }
	if (LPAREN == Token) {
		match(LPAREN, " '(' expecting ");
		n = pardecl(name, cast, &dummy);
		//fprintf(stderr, "JML IO %s\n", Text);
	} else if (cast) {
		match(RPAREN, "cast ");
		if (LPAREN == Token || LBRACK == Token) {
			n = funorarr(0);
		}
	} else {
		if (IDENT == Token && usertype(Text) == 0) {
                	copyname(name, Text);
                	Token = scant();
			if (LPAREN == Token || LBRACK == Token) {
				n = funorarr(0);
			}
			match(RPAREN, " ')' after identifier ");
		} else {
			error("expected '(' or identifier at :%s", Text);
		}
	}
        if (pointer > 0) {
		n2 = mkleaf(OP_POINTER, pointer);
		if (n) {
			if (n2) {
				n = mkbinop(OP_GLUE, n2, n);
			}
		} else {
			n = n2;
		}
        }
	
	if (LPAREN == Token || LBRACK == Token) {
		*eat = 0;
		n2 = funorarr(0);
		if (n) {
			if (n2) {
				n = mkbinop(OP_GLUE, n2, n);
			}
		} else {
			n = n2;
		}
	}
	
	return n;
}

static void parwalk(node *n, int *pprim, int level)
{
	int p = 0;
	if (NULL == n) return;
	level++;
	//	fprintf(stderr, "JML N IO %x %d", n, level);
	//	fprintf(stderr, " %d\n", n->op);
	switch (n->op) {
	case OP_GLUE:
		parwalk(n->left, pprim, level);
		parwalk(n->right, pprim, level);
		break;
	case OP_POINTER:
		p = n->args[0];
	//	fprintf(stderr, "JML OP_POINTER %d", p);
		while (p > 0) {
                	*pprim = pointerto(*pprim);
			p--;
		}
		break;
	case OP_ARRAY:
	//	fprintf(stderr, "JML OP_ARRAY %d", n->args[0]);
                *pprim = findarray(*pprim, n->args[0]);
		break;
	case OP_CALL:
//		fprintf(stderr, "JML OP_CALL ()");
               *pprim = FUNPTR;
	       break;
	default:
//		fprintf(stderr, "JML OP_ UNKNOWN %d", n->op);
		break;
	}
		//fprintf(stderr, " \n");
               *pprim = FUNPTR;
}

void dirdecl(int *pprim, char *name, int cast, int pmtr)
{
	node *n;
	int eat;

	eat = 1;

	match(LPAREN, " '(' expecting ");
	n = pardecl(name, cast, &eat);
	if (n) {
		parwalk(n, pprim, 0);
	}
	if (eat) {
		rparen();
	}
	// fprintf(stderr, "JMLi  IO %s %d %d\n",  Text, cast, pmtr);
}

/*
 * fnargs :=
 *	  asgmnt
 *	| asgmnt , fnargs
 */

static node *fnargs(int fn, int *na) {
	int	lv[LV];
	int	*types;
	char	msg[100];
	int	sgn[MAXFNARGS+1];
	node	*n = NULL, *n2;

	types = (int *) (fn? Stext[fn]: NULL);
	*na = 0;
	while (RPAREN != Token) {
		n2 = asgmnt(lv);
		n2 = rvalue(n2, lv);
		n = mkbinop(OP_GLUE, n, n2);
		if (comptype(lv[LVPRIM])) {
			error("struct/union passed by value", NULL);
			lv[LVPRIM] = pointerto(lv[LVPRIM]);
		}
		if (types && *types) {
			if (!typematch(*types, lv[LVPRIM])) {
				sprintf(msg, "wrong type in argument %d"
					" of call to: %%s",
					*na+1);
				error(msg, Names[fn]);
			}
			types++;
		}
		if (*na < MAXFNARGS) sgn[*na] = lv[LVPRIM], sgn[*na+1] = 0;
		(*na)++;
		if (COMMA == Token) {
			Token = scant();
			if (RPAREN == Token)
				error("trailing ',' in function call", NULL);
		}
		else
			break;
	}
	if (fn && TFUNCTION == Types[fn] && !Stext[fn]) {
		Stext[fn] = galloc((*na+1) * sizeof(int), 1);
		memcpy(Stext[fn], sgn, (*na+1) * sizeof(int));
	}
	rparen();
	return n;
}

int deref(int p) {
	int	y;

	if (p & PPTR) {
		return Prims[p & ~PRIMASK];
	}
	if ((p & PRIMASK) == PARRAY) {
		return Prims[p & ~PRIMASK];
	}	
	switch (p) {
	case VOIDPTR:	return PCHAR;
	case FUNPTR:	return PCHAR;
	}
	return -1;
}

static node *indirection(node *n, int *lv) {
	int	p;

	n = rvalue(n, lv);
	if (VOIDPTR == lv[LVPRIM])
		error("dereferencing void pointer", NULL);
	if ((p = deref(lv[LVPRIM])) < 0) {
		if (lv[LVSYM])
			error("indirection through non-pointer: %s",
				Names[lv[LVSYM]]);
		else
			error("indirection through non-pointer", NULL);
		p = lv[LVPRIM];
	}
	lv[LVPRIM] = p;
	lv[LVSYM] = 0;
	return n;
}

static void badcall(int *lv) {
	if (lv[LVSYM])
		error("call of non-function: %s",
			Names[lv[LVSYM]]);
	else
		error("call of non-function", NULL);
}

static int argsok(int na, int nf) {
	return na == nf || (nf < 0 && na >= -nf-1);
}

static node *stc_access(node *n, int *lv, int ptr) {
	int	y, p, q;
	node	*n2;

	n2 = n;
	p = rootprim(lv[LVPRIM]);
	lv[LVADDR] = 1;
	if (IDENT != Token) {
		Token = scant();
		error("struct/union member name expected after '%s'",
			ptr? "->": ".");
		return NULL;
	}
	y = findmem(p & ~PRIMASK, Text);
	if (0 == y) {
		error("struct/union has no such member: %s", Text);
	}

	if (lv[LVPRIM] & PPTR) {
		q = deref(lv[LVPRIM]) & PRIMASK;
	} else {
		q = 0;
	}
	if ((PSTRUCT == (p & PRIMASK) || PSTRUCT == (q & PRIMASK)) 
			&& Vals[y]) 
	{
		n2 = mkleaf(OP_LIT, Vals[y]);
		n2 = mkbinop(OP_ADD, n, n2);
	}
	Token = scant();
	p = Prims[y];
	if ((Prims[y] & STCMASK) == PARRAY) {
		p = pointerto(Prims[p & ~PRIMASK]);
		lv[LVADDR] = 0;
	}
	lv[LVPRIM] = p;
	return n2;
}

static node *brackexpr(node *n, int *lv) {
	node *n2, *fn;
	int	lv2[LV], p, na;
	int	q;
	Token = scant();
	if (LBRACK == Token) n = brackexpr(n, lv);
	n = indirection(n, lv);
	n2 = exprlist(lv2, 1);
	n2 = rvalue(n2, lv2);
	p = lv[LVPRIM];
	if (PINT != lv2[LVPRIM] && PCHAR != lv2[LVPRIM] && 
		PSCHAR != lv2[LVPRIM] && PUNSIG != lv2[LVPRIM])
	{
		error("non-integer subscript", NULL);
	}
	q = deref(p);
	if (    PINT == p ||
		PUNSIG == p ||
		PANY == p ||
		PFLOAT == p || 
		(p & PPTR) ||
		VOIDPTR == p ||
		PSTRUCT == (q & PRIMASK)
		) 
	{
		n2 = mkunop(OP_SCALE, n2);
	} else if ((p & PRIMASK) == PARRAY) {
		n2 = mkunop1(OP_SCALEBY,
				objsize(p), n2);
	} else if (comptype(p)) {
		n2 = mkunop1(OP_SCALEBY,
				objsize(p), n2);
	}
	n = mkbinop(OP_ADD, n, n2);
	rbrack();
	lv[LVSYM] = 0;
	lv[LVADDR] = 1;
	if ((p & PRIMASK) == PARRAY) {
		lv[LVADDR] = 0;
	}
	return n;
}

/*
 * postfix :=
 *	  primary
 *	| postfix [ expr ]
 *	| postfix ( )
 *	| postfix ( fnargs )
 *	| postfix ++
 *	| postfix --
 *	| postfix . identifier
 *	| postfix -> identifier
 */

static node *postfix(int *lv) {
	node	*n = NULL, *n2, *fn;
	int	lv2[LV], p, na;
	int	q;
	n = primary(lv);
	for (;;) {
		switch (Token) {
		case LBRACK:
			n = brackexpr(n, lv);
			break;
		case LPAREN:
			Token = scant();
			fn = n;
			n = fnargs(lv[LVSYM], &na);
			if (lv[LVSYM] && TFUNCTION == Types[lv[LVSYM]]) {
				if (!argsok(na, Sizes[lv[LVSYM]]))
					error("wrong number of arguments: %s",
						Names[lv[LVSYM]]);
				n = mkunop2(OP_CALL, lv[LVSYM], na, n);
			}
			else {
				if (lv[LVPRIM] != FUNPTR) badcall(lv);

				n = mkbinop(OP_GLUE, n, fn);
				n = mkunop2(OP_CALR, lv[LVSYM], na, n);
				lv[LVPRIM] = PANY;
			}
			lv[LVADDR] = 0;
			break;
		case INCR:
		case DECR: 
			if (lv[LVADDR]) {
				if (INCR == Token)
					n = mkunop2(OP_POSTINC, lv[LVPRIM],
						lv[LVSYM], n);
				else
					n = mkunop2(OP_POSTDEC, lv[LVPRIM],
						lv[LVSYM], n);
			}
			else
				error("lvalue required before '%s'", Text);
			Token = scant();
			lv[LVADDR] = 0;
			break;
		case DOT:
			Token = scant();
			if (comptype(lv[LVPRIM]))
				n = stc_access(n, lv, 0);
			else
				error("struct/union expected before '.'",
					NULL);
			break;
		case ARROW:
			Token = scant();
			q = deref(lv[LVPRIM]) & (STCMASK|PPTR);
			if (PSTRUCT == q) {
				n = rvalue(n, lv);
				n = stc_access(n, lv, 1);
			}
			else
				error(
				 "struct/union pointer expected before '->'",
				 NULL);
			lv[LVSYM] = 0;
			break;
		default:
			return n;
		}
	}
}

static node *prefix(int *lv);

static node *comp_size(void) {
	int	utype, k = 0, y, lv[LV];

	utype = 0;
	if (	CHAR == Token || INT == Token || VOID == Token ||
		UNSIGNED == Token || STRUCT == Token || UNION == Token ||
		FLOAT == Token || SCHAR == Token ||
                (IDENT == Token && (utype = usertype(Text)) != 0)
	) {
		if (utype) {
			k = objsize(Prims[utype]);
		}
		else {
			switch (Token) {
			case SCHAR:
			case CHAR:	k = CHARSIZE; break;
			case FLOAT:
			case UNSIGNED:
			case INT:	k = INTSIZE; break;
			case STRUCT:
			case UNION:	k = primtype(Token, NULL);
					k = objsize(k);
					break;
			}
		}
		Token = scant();
		if (STAR == Token) {
			k = PTRSIZE;
			Token = scant();
			if (STAR == Token) Token = scant();
		}
		if (0 == k)
			error("sizeof(void) is unknown", NULL);
	}
	else {
		prefix(lv);
		y = lv[LVSYM]? lv[LVSYM]: 0;
		k = y? objsize(Prims[y]):
			objsize(lv[LVPRIM]);
		if (0 == k)
			error("cannot compute sizeof object", NULL);
	}
	return mkleaf(OP_LIT, k);
}

/*
 * prefix :=
 *	  postfix
 *	| ++ prefix
 *	| -- prefix
 *	| & cast
 *	| * cast
 *	| + cast
 *	| - cast
 *	| ~ cast
 *	| ! cast
 *	| SIZEOF ( type )
 *	| SIZEOF ( type * )
 *	| SIZEOF ( type * * )
 *	| SIZEOF ( IDENT )
 *
 * type :=
 *	  INT
 *	| CHAR
 *	| VOID
 *	| STRUCT IDENT
 *	| UNION IDENT
 */

static node *prefix(int *lv) {
	node	*n;
	int	t;

	switch (Token) {
	case INCR:
	case DECR:
		t = Token;
		Token = scant();
		n = prefix(lv);
		if (lv[LVADDR]) {
			if (INCR == t)
				n = mkunop2(OP_PREINC, lv[LVPRIM],
					lv[LVSYM], n);
			else
				n = mkunop2(OP_PREDEC, lv[LVPRIM],
					lv[LVSYM], n);
		}
		else {
			error("lvalue expected after '%s'",
				t == INCR? "++": "--");
		}
		lv[LVADDR] = 0;
		return n;
	case STAR:
		Token = scant();
		n = cast(lv);
		n = indirection(n, lv);
		lv[LVADDR] = 1;
		return n;
	case PLUS:
		Token = scant();
		n = cast(lv);
		n = rvalue(n, lv); /* XXX really? */
		if (!inttype(lv[LVPRIM]))
			error("bad operand to unary '+'", NULL);
		lv[LVADDR] = 0;
		return n;
	case MINUS:
		Token = scant();
		n = cast(lv);
		n = rvalue(n, lv);
		if (!inttype(lv[LVPRIM]))
			error("bad operand to unary '-'", NULL);
		n = mkunop(OP_NEG, n);
		lv[LVADDR] = 0;
		return n;
	case TILDE:
		Token = scant();
		n = cast(lv);
		n = rvalue(n, lv);
		if (!inttype(lv[LVPRIM]))
			error("bad operand to '~'", NULL);
		n = mkunop(OP_NOT, n);
		lv[LVADDR] = 0;
		return n;
	case XMARK:
		Token = scant();
		n = cast(lv);
		n = rvalue(n, lv);
		n = mkunop(OP_LOGNOT, n);
		lv[LVPRIM] = PINT;
		lv[LVADDR] = 0;
		return n;
	case AMPER:
		Token = scant();
		n = cast(lv);
		if (lv[LVADDR]) {
			if (lv[LVSYM]) n = mkunop1(OP_ADDR, lv[LVSYM], n);
		}
		else if ((0 == lv[LVSYM] || 
			(Prims[lv[LVSYM] & ~PRIMASK] & STCMASK) != PARRAY) &&
			 !comptype(lv[LVPRIM])
		) {
			error("lvalue expected after unary '&'", NULL);
		}
		lv[LVPRIM] = pointerto(lv[LVPRIM]);
		lv[LVADDR] = 0;
		return n;
	case SIZEOF:
		Token = scant();
		lparen();
		n = comp_size();
		rparen();
		lv[LVPRIM] = PINT;
		lv[LVADDR] = 0;
		return n;
	default:
		return postfix(lv);
	}
}

/*
 * cast :=
 *	  prefix
 *	| ( type ) prefix
 *	| ( type * ) prefix
 *	| ( type * * ) prefix
 *	| ( INT ( * ) ( ) ) prefix
 */

static node *cast(int *lv) {
	int	t;
	node	*n;
	int 	utype = 0;

	if (LPAREN == Token) {
		Token = scant();
		if (INT == Token || CHAR == Token || VOID == Token ||
			UNSIGNED == Token || FLOAT == Token || SCHAR == Token ||
			STRUCT == Token || UNION == Token || ENUM == Token ||
                	(IDENT == Token && (utype = usertype(Text)) != 0)
		) {
			if (utype) {
				t = Prims[utype];
			} else {
				t = primtype(Token, NULL);
			}
			Token = scant();
		}
		else {
			reject();
			Token = LPAREN;
			strcpy(Text, "(");
			return prefix(lv);
		}
       		while (STAR == Token) {
                	Token = scant();
                	t = pointerto(t);
        	}

		if (LPAREN == Token) {
			Token = scant();
			match(STAR, "int(*)()");
			rparen();
			lparen();
			rparen();
			t = FUNPTR;
			/*
			dirdecl(&t, NULL, 1, 1);
			if ((t & (PRIMASK|PPTR)) == PARRAY) {
				error("cast to array", NULL);
			} else if (t == FUNPTR) {
				//fprintf(stderr, "JML FIXME\n");
				//error("cast to function", NULL);
			}	*/
		}
		if (LBRACK == Token) {
			Token = scant();
			if (RBRACK == Token) {
				t = findarray(t, 0);
			} else {
				t = findarray(t, constexpr0());
			}
			rbrack();
		}	
		rparen();
		lv[LVPRIM] = t;
		if (LPAREN == Token) {
			n = cast(lv); 
		} else {
			n = prefix(lv);
		}
		lv[LVPRIM] = t;
		return n;
	}
	else {
		return prefix(lv);
	}
}

int binop(int tok) {
	switch(tok) {
	case AMPER:	return OP_BINAND;
	case CARET:	return OP_BINXOR;
	case EQUAL:	return OP_EQUAL;
	case GREATER:	return OP_GREATER;
	case GTEQ:	return OP_GTEQ;
	case LESS:	return OP_LESS;
	case LSHIFT:	return OP_LSHIFT;
	case LTEQ:	return OP_LTEQ;
	case MINUS:	return OP_SUB;
	case MOD:	return OP_MOD;
	case NOTEQ:	return OP_NOTEQ;
	case PIPE:	return OP_BINIOR;
	case PLUS:	return OP_PLUS;
	case RSHIFT:	return OP_RSHIFT;
	case SLASH:	return OP_DIV;
	case STAR:	return OP_MUL;
	default:	fatal("internal: unknown binop");
			return 0; /* notreached */
	}
}

node *mkop(int op, int p1, int p2, node *l, node *r) {
	if (PLUS == op || MINUS == op || 
		RSHIFT == op || SLASH == op || MOD == op) 
	{
		return mkbinop2(binop(op), p1, p2, l, r);
	}
	else if (EQUAL == op || NOTEQ == op || LESS == op ||
		 GREATER == op || LTEQ == op || GTEQ == op)
	{
		return mkbinop1(binop(op), p1, l, r);
	}
	else {
		return mkbinop(binop(op), l, r);
	}
}

/*
 * term :=
 *	  cast
 *	| term * cast
 *	| term / cast
 *	| term % cast
 *
 * sum :=
 *	  term
 *	| sum + term
 *	| sum - term
 *
 * shift :=
 *	  sum
 *	| shift << sum
 *	| shift >> sum
 *
 * relation :=
 *	  shift
 *	| relation < shift
 *	| relation > shift
 *	| relation <= shift
 *	| relation >= shift
 *
 * equation :=
 *	  relation
 *	| equation == relation
 *	| equation != relation
 *
 * binand :=
 *	  equation
 *	| binand & equation
 *
 * binxor :=
 *	  binand
 *	| binxor ^ binand
 *
 * binor :=
 *	  binxor
 *	| binor '|' binxor
 *
 * binexpr :=
 *	  binor
 */

static node *binexpr(int *lv) {
	int	ops[9];
	int	prims[10];
	int	sp = 0;
	int	lv2[LV], a;
	node	*tree[10];

	tree[0] = cast(lv);
	a = lv[LVADDR];
	prims[0] = lv[LVPRIM];
	while (SLASH == Token || STAR == Token || MOD == Token ||
		PLUS == Token || MINUS == Token || LSHIFT == Token ||
		RSHIFT == Token || GREATER == Token || GTEQ == Token ||
		LESS == Token || LTEQ == Token || EQUAL == Token ||
		NOTEQ == Token || AMPER == Token || CARET == Token ||
		PIPE == Token
	) {
		tree[0] = rvalue(tree[0], lv);
		while (sp > 0 && Prec[Token] <= Prec[ops[sp-1]]) {
			tree[sp-1] = mkop(ops[sp-1], prims[sp-1], prims[sp],
					tree[sp-1], tree[sp]);
			prims[sp-1] = binoptype(ops[sp-1], prims[sp-1],
					prims[sp]);
			sp--;
		}
		ops[sp++] = Token;
		Token = scant();
		tree[sp] = cast(lv2);
		tree[sp] = rvalue(tree[sp], lv2);
		prims[sp] = lv2[LVPRIM];
		a = 0;
	}
	while (sp > 0) {
		tree[sp-1] = mkop(ops[sp-1], prims[sp-1], prims[sp],
				tree[sp-1], tree[sp]);
		prims[sp-1] = binoptype(ops[sp-1], prims[sp-1], prims[sp]);
		sp--;
	}
	lv[LVPRIM] = prims[0];
	lv[LVADDR] = a;
	return tree[0];
}

/*
 * logand :=
 *	  binexpr
 *	| logand && binexpr
 *
 * logor :=
 *	  logand
 *	| logor '||' logand
 */

static node *cond2(int *lv, int op) {
	int	lv2[LV];
	int	lab = 0;
	node	*n, *n2 = NULL;
	int	tv = 1;

	n = op == LOGOR? cond2(lv, LOGAND): binexpr(lv);
	while (Token == op) {
		if (!lab) lab = label();
		if (tv) notvoid(lv[LVPRIM]), tv = 0;
		n = rvalue(n, lv);
		Token = scant();
		n2 = op == LOGOR? cond2(lv2, LOGAND): binexpr(lv2);
		n2 = rvalue(n2, lv2);
		if (op == LOGOR)
			n = mkbinop1(OP_BRTRUE, lab, n, n2);
		else
			n = mkbinop1(OP_BRFALSE, lab, n, n2);
	}
	if (lab) {
		n = mkunop1(OP_LAB, lab, n);
		n = mkunop(OP_BOOL, n);
		lv[LVPRIM] = PINT;
		lv[LVADDR] = 0;
	}
	return n;
}

/*
 * condexpr :=
 *	  logor
 *	| logor ? expr : condexpr
 */

static node *cond3(int *lv) {
	node	*n, *n2;
	int	lv2[LV], p;
	int	l1 = 0, l2 = 0, tv = 1;

	n = cond2(lv, LOGOR);
	p = 0;
	while (QMARK == Token) {
		n = rvalue(n, lv);
		if (tv) notvoid(lv[LVPRIM]), tv = 0;
		l1 = label();
		if (!l2) l2 = label();
		Token = scant();
		n2 = exprlist(lv2, 0);
		n2 = rvalue(n2, lv2);
		n = mkbinop1(OP_BRFALSE, l1, n, n2);
		if (!p) p = lv2[LVPRIM];
		if (!typematch(p, lv2[LVPRIM]))
			error("incompatible types in '?:'", NULL);
		colon();
		n2 = cond2(lv2, LOGOR);
		n2 = rvalue(n2, lv2);
		n = mkbinop(OP_GLUE, n, n2);
		if (QMARK != Token)
			if (!typematch(p, lv2[LVPRIM]))
				error("incompatible types in '?:'", NULL);
	}
	if (l2) {
		n = mkunop1(OP_IFELSE, l2, n);
		lv[LVPRIM] = p;
		lv[LVADDR] = 0;
	}
	return n;
}

int arithop(int tok) {
	switch(tok) {
	case ASPLUS:	return PLUS;
	case ASMINUS:	return MINUS;
	case ASAND:	return AMPER;
	case ASOR:	return PIPE;
	case ASXOR:	return CARET;
	case ASMUL:	return STAR;
	case ASMOD:	return MOD;
	case ASDIV:	return SLASH;
	case ASLSHIFT:	return LSHIFT;
	case ASRSHIFT:	return RSHIFT;
	default:	fatal("internal: unknown assignment operator");
			return 0; /* notreached */
	}
}

/*
 * asgmnt :=
 *	  condexpr
 *	| condexpr = asgmnt
 *	| condexpr *= asgmnt
 *	| condexpr /= asgmnt
 *	| condexpr %= asgmnt
 *	| condexpr += asgmnt
 *	| condexpr -= asgmnt
 *	| condexpr <<= asgmnt
 *	| condexpr >>= asgmnt
 *	| condexpr &= asgmnt
 *	| condexpr ^= asgmnt
 *	| condexpr |= asgmnt
 */

static node *asgmnt(int *lv) {
	node	*n, *n2, *src;
	int	lv2[LV], lvs[LV], op;
	int 	lp;
	n = cond3(lv);
	if (ASSIGN == Token || ASDIV == Token || ASMUL == Token ||
		ASMOD == Token || ASPLUS == Token || ASMINUS == Token ||
		ASLSHIFT == Token || ASRSHIFT == Token || ASAND == Token ||
		ASXOR == Token || ASOR == Token
	) {
		op = Token;
		Token = scant();
		lv2[LVPRIM] = lv[LVPRIM];
		n2 = asgmnt(lv2);
		n2 = rvalue(n2, lv2);
		if (ASSIGN == op) {
			if (!typematch(lv[LVPRIM], lv2[LVPRIM])) {
				//fprintf(stderr, "JML %d::: %d %d\n",OP_LIT, n->op, n2->op);
				error("assignment from incompatible type",
						NULL);
			} else if (lv[LVPRIM] & PARRAY) {
				error("assignment to expression with array type"
						, NULL);
			}
			n = mkbinop2(OP_ASSIGN, lv[LVPRIM], lv[LVSYM], n, n2);
		}
		else {
			memcpy(lvs, lv, sizeof(lvs));
			src = rvalue(n, lvs);
			n2 = mkop(arithop(op), lv[LVPRIM], lv2[LVPRIM],
				src, n2);
			n = mkbinop2(OP_ASSIGN, lv[LVPRIM], lv[LVSYM], n, n2);
		}
		if (!lv[LVADDR])
			error("lvalue expected in assignment", Text);
		lv[LVADDR] = 0;
	}
	return n;
}

/*
 * expr :=
 *	  asgmnt
 *	| asgmnt , expr
 */

static node *exprlist(int *lv, int ckvoid) {
	node	*n, *n2 = NULL;
	int	p;

	n = asgmnt(lv);
	p = lv[LVPRIM];
	if (COMMA == Token) n = rvalue(n, lv);
	while (COMMA == Token) {
		Token = scant();
		n2 = asgmnt(lv);
		n2 = rvalue(n2, lv);
		p = lv[LVPRIM];
		n = mkbinop(OP_COMMA, n, n2);
	}
	if (ckvoid) notvoid(p);
	return n;
}

void expr(int *lv, int ckvoid) {
	node	*n;

	n = exprlist(lv, ckvoid);
	n = rvalue(n, lv);
	emittree(n);
}

void singleexpr(int prim) {
	int	lv[LV];
	node	*n;
	int	p;
	lv[LVPRIM] = prim;
	n = asgmnt(lv);
	p = lv[LVPRIM];
	n = rvalue(n, lv);
	notvoid(p);
	emittree(n);
}

void rexpr(void) {
	int	lv[LV];

	expr(lv, 1);
}

int constexpr0(void) {
	node	*n;
	int	lv[LV];

	n = cond3(lv);
	notvoid(lv[LVPRIM]);
	n = fold_reduce(n);
	if (NULL == n || OP_LIT != n->op) {
		//fprintf(stderr, "JML %d prim %d\n", n ? 0 : n->op, lv[LVPRIM]);
		error("constant expression expected", NULL);
		return 0;
	}
	return n->args[0];
}

static void initastr(int prim, int *psize, int *paddr, int lab)
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
		}
	}
	*paddr = *paddr - a;
	Token = scant();
}

static void initaarray(int prim, int *psize, int *paddr, int lab)
{
	char	*p;
	int	os, i, d, s, a, j, nbe;
	s = 0;
	nbe = 0;
	os = objsize(prim);
	while (Token != RBRACE) {
		singleexpr(prim);
		if (*psize < 0 && nbe > 0) {
			warning("excess elements in scalar initializer", NULL);
			clear(1);
		} else if (*psize > 0 && nbe >= *psize) {
			warning("excess elements in initializer", NULL);
			clear(1);
		} else if (os == 1) {
			genmovb(lab, s);
			s += os;
		} else {
			genmovw(lab, s);
			s += os;
		}
		nbe++;
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
		*psize = nbe;
	} else if (*psize < 0) {
		*psize = os;
	} else {
		if (nbe > *psize) {
			error("Too many initializers", NULL);
		} else {
			nbe = *psize;
		}
	}
	s = nbe * os;
	s = (s + INTSIZE-1) / INTSIZE * INTSIZE;
	*paddr = *paddr - s;
	Token = scant();
}


static void initlist(char *name, int utype, int oprim, int type, 
		int clss, int *psize, int *pinit, int *paddr) 
{
	int	n = 0;
	char	*p;
	int	i, d, s, a, j;
	int	prim;
	int	arr = 0;

	//fprintf(stderr, "JML initli %s\n", name);
	prim = oprim;
	*psize = -1;
	switch (oprim & PRIMASK) {
	case PARRAY:
		*psize = Sizes[oprim & ~PRIMASK];
		prim = Prims[oprim & ~PRIMASK];	
		arr = 1;
		break;
	case PPTR:
		arr = 1;
		*psize = 0;
		prim = Prims[oprim & ~PRIMASK];	
		break;
	}
	*pinit = label();
	if (CAUTO == clss) {
		if (STRLIT == Token) {
			gentext();
			genlab(*pinit);
			if (PCHAR != prim)
				error("initializer type mismatch!: %s", name);
			initastr(prim, psize, paddr, *pinit);
			genautosize(*pinit, *paddr);
			return;
		}
		gentext();
		genlab(*pinit);
		lbrace();
		initaarray(prim, psize, paddr, *pinit);
		genautosize(*pinit, *paddr);
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

int constexpr3(char *name, int prim, int scls, int *psize, int *pinit, int *paddr) {

	int 	r = BPW;
	int 	s, a, t;
	int 	lv[LV];
	node	*n;


	if (PCHAR == prim || PSCHAR == prim) r = 1;
	//if ((prim & STCMASK) == PARRAY) {
	if (LBRACE == Token || STRLIT == Token) {
		//if (CSTATIC == scls || CPUBLIC == scls || CLSTATC == scls) {
			initlist(name, 0, prim, 0, 
					scls, psize, pinit, paddr);
		//	return r;
		//}
		return r;
	//	error("unsupported array init.", NULL);
	}
	if ((prim & STCMASK) == PARRAY) {
		//fprintf(stderr, "JML gen Xi %s cls %d %s\n", name, scls, Text);
	}
	if (CEXTERN == scls) {
		error("Initialization of extern", NULL);
		return r;
	} else if (CSTATIC == scls || CPUBLIC == scls || CLSTATC == scls) {
		gendata();
                *pinit = label();
		genlab(*pinit);
		if (CSTATIC == scls || CPUBLIC == scls) {
			if (CPUBLIC == scls) genpublic(name); 
                        genname(name);

		} else {
		}
		r = constinit(prim);
		return r;
	} else if (CAUTO == scls) {
                *pinit = label();
		genlab(*pinit);
		//fprintf(stderr, "JML gen i %s\n", name);
		singleexpr(prim);
		if (PCHAR == prim || PSCHAR == prim) {
			genmovb(*pinit, 0);
		} else {
			genmovw(*pinit, 0);
		}
		*paddr -= BPW;
		genautosize(*pinit, *paddr);
	} else {
		error("Unknown storage class", NULL);
	}
	return r;
}



int constexpr2(char *name, int utype, int prim, int type, int scls, int *psize, int *pval, int *pinit, int *paddr) {

	int 	r = BPW;
	int 	s, a, t;
	int 	lv[LV];
	node	*n;

		//fprintf(stderr, "JML gen Xi %s cls %d\n", name, scls);

	if (PCHAR == prim || PSCHAR == prim) r = 1;
	if ((prim & STCMASK) == PARRAY) {
		//if (CSTATIC == scls || CPUBLIC == scls || CLSTATC == scls) {
			initlist(name, utype, prim, type, 
					scls, psize, pinit, paddr);
	        	*pval = *pinit;	
		//	return r;
		//}
		return r;
	//	error("unsupported array init.", NULL);
	}
	if (CEXTERN == scls) {
		error("Initialization of extern", NULL);
		return r;
	} else if (CSTATIC == scls || CPUBLIC == scls || CLSTATC == scls) {
		gendata();
                *pinit = label();
		genlab(*pinit);
		*pval = *pinit;
		if (CSTATIC == scls || CPUBLIC == scls) {
			if (CPUBLIC == scls) genpublic(name); 
                        genname(name);

		} else {
		}
		r = constinit(prim);
		return r;
	} else if (CAUTO == scls) {
                *pinit = label();
		genlab(*pinit);
		//fprintf(stderr, "JML gen i %s\n", name);
		singleexpr(prim);
		if (PCHAR == prim || PSCHAR == prim) {
			genmovb(*pinit, 0);
		} else {
			genmovw(*pinit, 0);
		}
		*paddr -= BPW;
		genautosize(*pinit, *paddr);
	} else {
		error("Unknown storage class", NULL);
	}
	return r;
}


int constinit(int prim)
{
	int 	r = BPW;
	node	*n;
	int	lv[LV];
	int	v;
	char 	buf[30];

	if (PCHAR == prim || PSCHAR == prim) r = 1;

	n = binexpr(lv);
	notvoid(lv[LVPRIM]);
	n = fold_reduce(n);

/*	commit(); JML */
	if (NULL == n) {
		error("missing constant expression", NULL);
	} else if (OP_LIT == n->op) {
		v = n->args[0];
		if (PCHAR == prim) {
                        if (v < 0 || v > 255) {
                                sprintf(buf, "%d", v);
                                error("initializer out of range: %s", buf);
                        }
                        gendefb(v);
		} else if (PSCHAR == prim) {
                        if (v < -128 || v > 127) {
                                sprintf(buf, "%d", v);
                                error("initializer out of range: %s", buf);
                        }
                        gendefb(v);
                }
                else {
                        gendefw(v);
                }
	} else if (PCHAR == prim || PSCHAR == prim) {
		error("invalid initialization of char", NULL);
	} else if (OP_ADDR == n->op) {
		gendefa(n->args[0], 0);	
		//fprintf(stderr, "JML ADDR L%d\n", n->args[0]);
	} else if (OP_LDLAB == n->op) { 
		//fprintf(stderr, "JML LDLAB L%d\n", n->args[0]);
		gendefl(n->args[0]);	
	} else if (OP_IDENT == n->op) { 
		//fprintf(stderr, "JML IDENT %s\n", Names[n->args[0]]);
		gendefa(n->args[0], 0);	
	} else if (OP_ADD == n->op) { 
		//fprintf(stderr, "JML %d prim %d left %d right %d\n", n->op, prim, n->left->op, n->right->op);
		if (OP_ADDR == n->left->op && OP_LIT == n->right->op) {
			gendefa(n->left->args[0], n->right->args[0]);	
		} else {
			error("constant expression expected!", NULL);
		}
	} else {
		//fprintf(stderr, "JML %d prim %d\n", n->op, prim);
		error("constant expression expected!", NULL);
	}
	commit();
	clear(0);
	return r;
}

