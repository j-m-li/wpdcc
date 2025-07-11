/*
 *	NMH's Simple C Compiler, 2011--2014
 *	Global variables
 */

#ifndef extern_
 #define extern_ extern
#endif

extern_ FILE	*Infile;
extern_ FILE	*Outfile;
extern_ FILE	*Outfile1;
extern_ FILE	*Tmpfile;
extern_ char 	*Tmpbuf[MAXRECURS];
extern_ int	Tmpnbuf;
extern_ char	Includes[TEXTLEN+2];
extern_ int	Token;
extern_ char	Text[TEXTLEN+1];
extern_ int	Value;
extern_ int	Line;
extern_ int	Newline;
extern_ int	Spaces;
extern_ int	Errors;
extern_ int	Syntoken;
extern_ int	Putback;
extern_ int	Rejected;
extern_ int	Rejval;
extern_ char	Rejtext[TEXTLEN+1];
extern_ char	*File;
extern_ char	*Basefile;
extern_ char	*Macp[MAXNMAC];
extern_ int	Macc[MAXNMAC];
extern_ int	Macy[MAXNMAC];
extern_ char	Macbuf[MAXNMAC * (TEXTLEN+1)];
extern_ int	Mp;
extern_ int	Mpbase;
extern_ int	Expandmac;
extern_ int	Ifdefstk[MAXIFDEF], Isp;
extern_ int	Inclev;
extern_ int	Textseg;
extern_ int	Nodes[NODEPOOLSZ];
extern_ int	Ndtop;
extern_ int	Ndmax;


extern_ int	Intptr;
extern_ int	Charptr;
extern_ int	Scharptr;
extern_ int	Unsigptr;
extern_ int	Floatptr;
extern_ int	Funpp;

/* symbol tabel structure */
extern_ char	*Names[NSYMBOLS];
extern_ int	Prims[NSYMBOLS];
extern_ char	Types[NSYMBOLS];
extern_ char	Stcls[NSYMBOLS];
extern_ int	Sizes[NSYMBOLS];
extern_ int	Vals[NSYMBOLS];
extern_ char	*Stext[NSYMBOLS];
extern_ int	Globs;
extern_ int	Locs;
extern_ int	Locsbb;

extern_ int	Thisfn;
extern_ int	Thislsize;
extern_ int	Thismsize;

/* name list */
extern_ char	Nlist[POOLSIZE];
extern_ int	Nbot;
extern_ int	Ntop;

/* label stacks */
extern_ int	Breakstk[MAXBREAK], Bsp;
extern_ int	Contstk[MAXBREAK], Csp;
extern_ int	Retlab;

/* synthesizer operand queue */
extern_ int	Q_type;
extern_ int	Q_val;
extern_ char	Q_name[NAMELEN+1];
extern_ int	Q_cmp;
extern_ int	Q_bool;

/* file collector */
extern_ char	*Files[MAXFILES];
extern_ char	Temp[MAXFILES];
extern_ int	Nf;

/* options */
extern_ int	O_verbose;
extern_ int	O_componly;
extern_ int	O_asmonly;
extern_ int	O_preponly;
extern_ int	O_testonly;
extern_ int	O_stdio;
extern_ char	*O_outfile;
extern_ int	O_debug;
