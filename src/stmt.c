/*
 *	NMH's Simple C Compiler, 2011--2016
 *	Statement parser
 */

#include "defs.h"
#include "data.h"
#include "decl.h"

static void stmt(void);

static char Savetxt[TEXTLEN+1];
static int Saveval;
static int Savetk;

void save(int tk)
{
	Savetk = tk;
	Saveval = Value;
	strcpy(Savetxt, Text);
}

int restore()
{
	Value = Saveval;
	strcpy(Text, Savetxt);
	return Savetk;
}

int scant(void)
{
	int t, nt;

	t = scan();
	switch (t) {
	case VOLATILE:
		t = scant();
		break;
	case DOUBLE:
	case FLOAT:
		/*t = FLOAT;*/
		t = INT;
		break;
	case CONST:
		t = scant();
		break;
	case UNSIGNED:
	case SIGNED:
		save(-1);
		nt = scan();
		if (INT != nt) {
			if (SHORT == nt || LONG == nt) {
				save(-1);
				nt = scan();
				if (LONG == nt) {
					nt = scan(); /* unsigned long long int*/
				}
				if (INT != nt) {
					Token = nt;
					reject();
					restore();
				}
				if (SIGNED == t) {
					t = INT;
				}
			} else if (CHAR == nt) {
				if (SIGNED == t) {
					t = SCHAR;
				} else {
					t = CHAR;
				}
			} else {
				Token = nt;
				reject();
				restore();
			}
		} else if (SIGNED == t) {
			t = INT;
		}
		break;
	case LONG:
	case SHORT:
		save(-1);
		nt = scan();
		if (DOUBLE == nt && LONG == t) {
			t = FLOAT;
			error("Floating point not supported", NULL);
		} else if (LONG == nt) {
			nt = scan(); /* long long int */
			if (INT != nt) {
				Token = nt;
				reject();
			}
			t = INT;
		} else if (INT != nt) {
			t = INT;
			Token = nt;
			reject();
			restore();
		} else {
			t = INT;
		}
		break;
	}
	return t;
}


/*
 * compound :=
 *	  { stmt_list }
 *	| { }
 *
 * stmt_list:
 *	  stmt
 *	| stmt stmt_list
 */

void compound(int lbr) {
	int lsize = 0;
	int locs;
	int locsbb;

	locs = Locs;
	locsbb = Locsbb;
	Locsbb = Locs;

	if (lbr) Token = scant();
	lsize = localdecls();  /* lsize is negative */
        Thislsize += lsize;
        if (Thismsize > Thislsize) {
                Thismsize = Thislsize;
        }

	while (RBRACE != Token) {
		if (eofcheck()) return;
		stmt();
		Ndtop = 1;
	}

	Thislsize -= lsize;
	if (lbr) Token = scant();
	Locs = locs;
	Locsbb = locsbb;
}

static void pushbrk(int id) {
	if (Bsp >= MAXBREAK)
		fatal("too many nested loops/switches");
	Breakstk[Bsp++] = id;
}

static void pushcont(int id) {
	if (Csp >= MAXBREAK)
		fatal("too many nested loops/switches");
	Contstk[Csp++] = id;
}

/*
 * break_stmt := BREAK ;
 */

static void break_stmt(void) {
	Token = scant();
	if (!Bsp) error("'break' not in loop/switch context", NULL);
	genjump(Breakstk[Bsp-1]);
	semi();
}

/*
 * continue_stmt := CONTINUE ;
 */

static void continue_stmt(void) {
	Token = scant();
	if (!Csp) error("'continue' not in loop context", NULL);
	genjump(Contstk[Csp-1]);
	semi();
}

/*
 * do_stmt := DO stmt WHILE ( expr ) ;
 */

static void do_stmt(void) {
	int	ls, lb, lc;

	Token = scant();
	gentext();
	ls = label();
	pushbrk(lb = label());
	pushcont(lc = label());
	genlab(ls);
	gendata();
	stmt();
	match(WHILE, "'while'");
	lparen();
	genlab(lc);
	rexpr();
	genbrtrue(ls);
	clear(1);
	genlab(lb);
	rparen();
	semi();
	Bsp--;
	Csp--;
}

/*
 * for_stmt :=
 *	FOR ( opt_expr ; opt_expr ; opt_expr ) stmt
 *
 * opt_expr :=
 *	| expr
 */

static void for_stmt(void) {
	int	ls, lbody, lb, lc;

	Token = scant();
	ls = label();
	lbody = label();
	pushbrk(lb = label());
	pushcont(lc = label());
	lparen();
	if (Token != SEMI) {
		rexpr();
		clear(1);
	}
	semi();
	genlab(ls);
	if (Token != SEMI) {
		rexpr();
		genbrfalse(lb);
		clear(1);
	}
	genjump(lbody);
	semi();
	genlab(lc);
	if (Token != RPAREN)
		rexpr();
	clear(1);
	genjump(ls);
	rparen();
	genlab(lbody);
	stmt();
	genjump(lc);
	genlab(lb);
	Bsp--;
	Csp--;
}

/*
 * if_stmt :=
 *	  IF ( expr ) stmt
 *	| IF ( expr ) stmt ELSE stmt
 */

static void if_stmt(void) {
	int	l1, l2;

	Token = scant();
	lparen();
	rexpr();
	l1 = label();
	genbrfalse(l1);
	clear(1);
	rparen();
	stmt();
	if (ELSE == Token) {
		l2 = label();
		genjump(l2);
		genlab(l1);
		l1 = l2;
		Token = scant();
		stmt();
	}
	genlab(l1);
}

/*
 * return_stmt :=
 *	  RETURN ;
 *	| RETURN expr ;
 */

static void return_stmt(void) {
	int	lv[LV];

	Token = scant();
	if (Token != SEMI) {
		expr(lv, 1);
		if (!typematch(lv[LVPRIM], Prims[Thisfn]))
			error("incompatible type in 'return'", NULL);
	}
	else {
		if (Prims[Thisfn] != PVOID)
			error("missing value after 'return'", NULL);
	}
	genjump(Retlab);
	semi();
}

static int lab_stmt(int g) {
	int l;
	int y;

	y = findloc(Text);
	if (!y) {
		l = label();
		y = addloc(Text, 0, TLABEL, CLABEL, 0, l, 0);
	} else if (TLABEL == Types[y]) {
		l = Vals[y];
	} else {
		error("Label %s already defined", Text);
	}
	if (g) genlab(l);
	Token = scant();
	return l;
}

static void goto_stmt(void) {

	int l;
	Token = scant();
	if (IDENT != Token) {
		error("Label identifier expected", NULL);
		return;
	}
	l = lab_stmt(0);
	genjump(l);
	commit();
	semi();
}

/*
 * switch_stmt :=
 *	  SWITCH ( expr ) { switch_block }
 *
 * switch_block :=
 *	  switch_block_stmt
 *	| switch_block_stmt switch_block
 *
 * switch_block_stmt :=
 *	  CASE constexpr :
 *	| DEFAULT :
 *	| stmt
 */

static void switch_block(void) {
	int	lb, ls, ldflt = 0;
	int	cval[MAXCASE];
	int	clab[MAXCASE];
	int	nc = 0;

	Token = scant();
	pushbrk(lb = label());
	ls = label();
	genjump(ls);
	while (RBRACE != Token) {
		if (eofcheck()) return;
		if ((CASE == Token || DEFAULT == Token) && nc >= MAXCASE) {
			error("too many 'case's in 'switch'", NULL);
			nc = 0;
		}
		if (CASE == Token) {
			Token = scant();
			cval[nc] = constexpr0();
			genlab(clab[nc++] = label());
			colon();
		}
		else if (DEFAULT == Token) {
			Token = scant();
			ldflt = label();
			genlab(ldflt);
			colon();
		}
		else
			stmt();
	}
	if (!nc) {
		if (ldflt) {
			cval[nc] = 0;
			clab[nc++] = ldflt;
		}
		else
			error("empty switch", NULL);
	}
	genjump(lb);
	genlab(ls);
	genswitch(cval, clab, nc, ldflt? ldflt: lb);
	gentext();
	genlab(lb);
	Token = scant();
	Bsp--;
}

static void switch_stmt(void) {
	Token = scant();
	lparen();
	rexpr();
	commit();
	clear(0);
	rparen();
	if (Token != LBRACE)
		error("'{' expected after 'switch'", NULL);
	switch_block();
}

/*
 * while_stmt := WHILE ( expr ) stmt
 */

static void while_stmt(void) {
	int	lb, lc;

	Token = scant();
	pushbrk(lb = label());
	pushcont(lc = label());
	genlab(lc);
	lparen();
	rexpr();
	genbrfalse(lb);
	clear(1);
	rparen();
	stmt();
	genjump(lc);
	genlab(lb);
	Bsp--;
	Csp--;
}

static void wrong_ctx(int t) {
	if (DEFAULT == t) {
		error("'default' not in 'switch' context", NULL);
		Token = scant();
		colon();
	}
	else {
		error("'case' not in 'switch' context", NULL);
		Token = scant();
		constexpr0();
		colon();
	}
}

/*
 * stmt :=
 *	  break_stmt
 *	| continue_stmt
 *	| do_stmt
 *	| for_stmt
 *	| if_stmt
 *	| return_stmt
 *	| switch_stmt
 *	| while_stmt
 *	| compound
 *	| ;
 *	| expr ;
 */

static void stmt(void) {
	int	lv[LV];

	switch (Token) {
	case BREAK:	break_stmt(); break;
	case CONTINUE:	continue_stmt(); break;
	case DO:	do_stmt(); break;
	case FOR:	for_stmt(); break;
	case IF:	if_stmt(); break;
	case RETURN:	return_stmt(); break;
	case SWITCH:	switch_stmt(); break;
	case WHILE:	while_stmt(); break;
	case LBRACE:	compound(1); break;
	case SEMI:	Token = scant(); break;
	case DEFAULT:	wrong_ctx(DEFAULT); break;
	case CASE:	wrong_ctx(CASE); break;
	case GOTO:	goto_stmt(); break;
	default:	
		if (IDENT == Token) {
			save(Token);
			Token = scant();
			if (COLON == Token) {
				Token = restore();
				lab_stmt(1);
				commit();
				break;
			} else {
				reject();
				Token = restore();
			}
		}
		expr(lv, 0); semi(); commit(); break;
	}
	clear(1);
}
