/*
 *	NMH's Simple C Compiler, 2011--2021
 *	Definitions
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "cg.h"
#include "sys.h"

#define VERSION		"2023-02-24"

#ifndef SCCDIR
 #define SCCDIR		"."
#endif

#ifndef AOUTNAME
 #define AOUTNAME	"a.out"
#endif

#define SCCLIBC		"%s/lib/libscc.a"

#define PREFIX		'C'
#define LPREFIX		'L'

#define INTSIZE		BPW
#define PTRSIZE		INTSIZE
#define CHARSIZE	1

#define TEXTLEN		2048
#define NAMELEN		50

#define MAXFILES	32
#define MAXRECURS	8

#define MAXIFDEF	16
#define MAXNMAC		32
#define MAXCASE		256
#define MAXBREAK	16
#define MAXLOCINIT	32
#define MAXFNARGS	32

/* assert(NSYMBOLS < PSTRUCT) */
#define NSYMBOLS	10240
#define POOLSIZE	163840
#define NODEPOOLSZ	4096	/* ints */

/* types */
enum {
	TVARIABLE = 1,
	TFUNCTION,
	TCONSTANT,
	TMACRO,
	TPTR,
	TSTRUCT,
	TUNION,
	TPROXY,
	TLABEL,
	TTARRAY,
};

/* primitive types */
enum {
	PCHAR = 1,
	PSCHAR,
	PINT,
	PUNSIG,
	PFLOAT,
	PVOID,
	VOIDPTR,
	PFUN,
	FUNPTR, /* must be the last */
	P__FREE = 0x200000,
	PSTRUCT = 0x400000,
	PARRAY  = 0x600000,
	PPTR 	= 0x800000,
	STCMASK = 0x600000,
	PRIMASK	= 0xE00000
};

/* storage classes */
enum {
	CPUBLIC = 1,
	CEXTERN,
	CSTATIC,
	CLSTATC,
	CAUTO,
	CSPROTO,
	CMEMBER,
	CTYPE,
	CLABEL,
	CCURRENT,
};

/* lvalue structure */
enum {
	LVSYM,
	LVPRIM,
	LVADDR,
	LV
};

/* debug options */
enum {
	D_LSYM = 1,
	D_GSYM = 2,
	D_STAT = 4,
	D_DEFI = 8,

};

/* addressing modes */
enum {
	empty,
	addr_auto,
	array_auto,
	addr_static,
	addr_globl,
	addr_label,
	literal,
	auto_byte,
	auto_char,
	auto_word,
	static_byte,
	static_char,
	static_word,
	globl_byte,
	globl_char,
	globl_word
};

/* compare instructions */
enum {
	cnone,
	equal,
	not_equal,
	less,
	greater,
	less_equal,
	greater_equal,
	below,
	above,
	below_equal,
	above_equal
};

/* boolean instructions */
enum {
	bnone,
	lognot,
	normalize
};

/* AST node */
struct node_stc {
	int		op;
	struct node_stc	*left, *right;
	int		args[1];
};

#define node	struct node_stc

/* tokens */
enum {
	SLASH, STAR, MOD, PLUS, MINUS, LSHIFT, RSHIFT, /*7*/
	GREATER, GTEQ, LESS, LTEQ, EQUAL, NOTEQ, AMPER, 
	CARET, PIPE, LOGAND, LOGOR,

	ARROW, ASAND, ASXOR, ASLSHIFT, ASMINUS, ASMOD, ASOR, ASPLUS,
	ASRSHIFT, ASDIV, ASMUL, ASSIGN, AUTO, BREAK, CASE, CHAR, COLON,
	COMMA, CONST, CONTINUE, DECR, DEFAULT, DO, DOT, DOUBLE, 
	ELLIPSIS, ELSE, ENUM, EXTERN, FLOAT, FLOATLIT, FOR, GOTO, IDENT, IF, 
	INCR, INT, INTLIT, LBRACE, LBRACK, LONG, LPAREN, NOT, QMARK, 
	RBRACE, RBRACK, REGISTER, RETURN, RPAREN, SCHAR, SEMI, SHORT, SIGNED, 
	SIZEOF, STATIC, STRLIT, STRUCT, SWITCH, TILDE, TYPEDEF,
	UNION, UNSIGLIT, UNSIGNED, VOID, VOLATILE, WHILE, XEOF, XMARK, XEOL,

	P_DEFINE, P_DONE,  P_ELSE, P_ELIF, P_ELIFNOT, P_ELSENOT, P_ENDIF, 
	P_ERROR, P_IF, P_IFDEF,
	P_IFNDEF, P_INCLUDE, P_LINE, P_PRAGMA, P_UNDEF 
};

/* AST operators */
enum {
	OP_GLUE, OP_ADD, OP_ADDR, OP_ARRAY,  OP_ASSIGN, OP_BINAND, OP_BINIOR,
	OP_BINXOR, OP_BOOL, OP_BRFALSE, OP_BRTRUE, OP_CALL, OP_CALR,
	OP_COMMA, OP_DEC, OP_DIV, OP_EQUAL, OP_GREATER, OP_GTEQ,
	OP_IDENT, OP_IFELSE, OP_LAB, OP_LDLAB, OP_LESS, OP_LIT,
	OP_LOGNOT, OP_LSHIFT, OP_LTEQ, OP_MOD, OP_MUL, OP_NEG,
	OP_NOT, OP_NOTEQ, OP_PLUS, OP_PREDEC, OP_PREINC, OP_POSTDEC,
	OP_POSTINC, OP_RSHIFT, OP_RVAL, OP_SCALE, OP_SCALEBY, OP_SUB,
	OP_POINTER, OP_FUN, OP_PAREN, OP_ARG
};

