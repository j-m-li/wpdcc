

//**********************
// Main program for XMAC
//**********************
// Written by John Goltz
//**********************

//++++
// This software is in the public domain.  It may be freely copied and used
// for whatever purpose you see fit, including commerical uses.  Anyone
// modifying this software may claim ownership of the modifications, but not
// the complete derived code.  It would be appreciated if the authors were
// told what this software is being used for, but this is not a requirement.

//   THIS SOFTWARE IS PROVIDED BY THE AUTHORS "AS IS" AND ANY EXPRESS OR
//   IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
//   OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
//   IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY DIRECT, INDIRECT,
//   INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
//   BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
//   OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
//   ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
//   TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
//   USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//----

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <xos.h>
#include <xcmalloc.h>
#include <xcstring.h>
#include <xossvc.h>
#include <progarg.h>
///#include <progasst.h>
#include <proghelp.h>
#include "xmac.h"

// Edit history
 
// 1.31 - 4-Oct-91
//	Minor text changes to get rid of "address space" references, fixed
//	.SEG, .MSECT, and .PSECT handling to allow for trailing spaces
// 1.32 - 17-Nov-91
//	Fixed srchtbl to return null if 0 length table specified - this
//	could cause page fault if certain names where searched for in a
//	0 length table
// 1.33 - 13-Dec-91
//	Fixed getopr to give proper error when #reg specified
// 1.34 - 9-May-92
//	Added new global symbol request section (type 9), added support for
//	the .LNKSEG pseudo-op for linked segments, changed address parser to
//	consider general byte offsets illegal (were considered to be word
//	offsets), changed to not use "ADDL EAX, #n.B" special form when it
//	would be longer
// 2.0 13-May-92
//	Changed to use CLIBTV (XOS only)
// 2.1  1-Jul-92
//	Changed ENTER and LEAVE to require 80186 or higher processor
// 2.2  18-Jul-92
//	Changed to allow [DI+BX] as well as [BX+DI], etc., changed to generate
//	the segment override prefix first (386 gives the wrong address on
//	traps if size prefixes preceed a segment override prefix!), added %
//	feature for auto-size selection, added .EXPB, .EXPW, and .EXPL pseudo-
//	ops (same as .BYTE, .WORD, and .LONG)
// 2.3  16-Oct-92
//	Added FPU instructions, changed to use heapsort, changed to use procarg
// 2.5  2-Jan-92
//	Fixed op-codes for FSAVE, FNSAVE, and FRSTOR, fixed problem with /OUT
//	and /NOOUT switches
// 2.6  6-Feb-93
//	Several minor code generation changes to match MASM 6.0
// 2.7  29-Mar-93
//	Started to add support for floating point constants; added some more
//	floating point opcode to the opcode table
// 2.8  2-Apr-93
//	Added .SEG16 and .SEG32 pseudo-ops
// 2.9  3-Apr-93
//	Added some error checks to .SEG16 and .SEG32; changed to not report
//	address error on undefined relative addresses; added /COMPAND and
//	/EXPANDED options to control error message formats; some general
//	clean-up
// 2.10 5-Apr-93
//	Fixed bug in code added for 2.6 (was not checking immediate operand
//	size correctly for EAX case)
// 2.11 20-May-93
//	Fixed bug in .IF DEF and .IF NDF (was not marking symbol tested as being
//	used)
// 2.12 7-Jul-93
//	Fixed bug with >xxxx in macro arguments when symbol value was negative
// 2.13 6-Nov-93
//	Added RETW, RETL, RETFW, RETFL.
// 2.14 10-Nov-93
//	Added IRETW and IRETL
// 2.15 5-Dec-93
//	Changed to use symbol numbers in object files; changed default object
//	file extension to OBJ.
// 2.16 7-Dec-93
//	Several bug fixes for symbol number support.
// 2.17 8-Dec-93
//	Changed to suppress psect byte for symbol definitions with non-address
//	values; added W (binary) as number radix specifier.
// 2.18 20-Dec-93
//	Fixed problem which caused page fault when referencing psect whose
//	msect referenced an undefined segment.
// 3.0  28-Dec-93
//	Changed to 32-bit environment.
// 3.1  16-Feb-94
//	Corrected problem with floating divide instructions in the opcode table.
// 3.2	25-Feb-94
//	Added line continuation.
// 3.3  15-Mar-94
//	Fixed handling of the FSTSW AX instruction.
// 3.4	8-May-94
//	Added support for .IMPORT and .EXPORT
// 3.5  21-May-94
//	Added .ASCIL and .ASCIU pseudo-ops
// 3.6  5-Jul-94
//	Fixed problem with continued lines messing up line number in error
//	messages; fixed problem in pmod which caused page fault if had .MOD
//	without a current psect.
// 3.7	9-Aug-94
//	Fixed op-code table entry for FNCLEX.
// 3.8  29-Nov-94
//	Fixed bug in repeat handling, was not reporting unterminated repeats
// 3.9  20-Dec-94
//	Fixed several errors in opcode table for floating point instructions.
// 3.10	25-Sep-98
//	Fixed problem with listing negative 16-bit address offsets.
// 3.11 21-Nov-00
//	Increased maximum symbol length to 64 characters
// 3.12 6-Apr-02
//  Fixed problem with order in pseudo-op table (.IFTF)

// 4.4 15-Aug-04
//	Fixed problem with ENTER instruction with only 1 operand
// 4.5 31-Aug-04
//	Added MFENCE, SFENCE, and LFENCE op-codes
// 4.6 5-Dec-05
//  Changed .IF to give X error on bad value, also removed support for
//  single line conditionals (it didn't work right and is not really
//  needed). Fixed so .IF xx lines list most of the time.
// 4.7  3-Feb-06
//  Fixed problem with adjusting offsets past assignments to $
// 4.8  2-May-06
//  Fixed problem with adjusting offsets past volatile assignments to $
// 4.9  8-Jul-06
//  Added FISTTPx instruction.
// 4.10 14-Jul-06
//  Fixed problem with expanding branches to different PSECTS (for now force
//  them to be 16/32-bit)
// 4.11 22-Sep-06
//  Fixed several problems with not reporting truncation errors; Fixed
//    problem with not clearing adjust value when assigning value to $.
// 4.12 3-Apr-07
//  Fixed some more problems with truncation reporting.
// 4.13 ?????
//  Sources lost!!!!
// 4.14 31-Jan-08
//  Added CPUID instruction
// 4.15  5-Jun-08
//	Added table entry for CR4
// 4.16  9-Jul-09
//  Fixed problem with generating bad code for scaled index without a base
//    register. Added automatic insertion or removal of 0 offset for indexed
//    addressing modes.
// 4.17  10-Jul-09
//  Fixed bug introduced in 4.16 - was discarding offset if forward reference.

// Allocate global data

#define VERSION 4
#define EDITNO  18

int     version = VERSION;		// XMAC version number 
int     editnum = EDITNO;		// XMAC edit number

Prog_Info pib;

OP  opdd;						// Data for destination operand
OP  opds;						// Data for source operand
OP  opdi;						// Data for immediate operand

struct  sd *sghead;				// Pointer to head of segment data block list
struct  sd *sgtail;				// Pointer to tail of segment data block list
struct  sd *cursgd;				// Pointer to current segment data block
struct  md *mshead;				// Pointer to head of msect data block list
struct  md *mstail;				// Pointer to tail of msect data block list
struct  md *curmsd;				// Pointer to current msect data block
struct  pd *pshead;				// Pointer to head of psect data block list
struct  pd *pstail;				// Pointer to tail of psect data block list
struct  pd *curpsd;				// Pointer to current psect data block
struct  sb *firstsrc;			// Pointer to first source block
struct  sb *thissrc;			// Pointer to current source block
struct  sb *lastsrc = (struct sb *)(&firstsrc);
								// Pointer to last source block
struct  ic *inspnt;				// Pointer to current ICB
struct  ic *lstinp;				// Listing copy of pointer to current ICB
struct  ab *alspnt;				// Pointer to first argument while defining
								//   macro
struct  reqblk *reqhead;		// Head pointer for .REQUIR list
struct  reqblk *reqtail;		// Tail pointer for .REQUIR list
int     srclvl;					// Source level
FILE   *srcfp[INCLMAX+1];		// Source file pointers
char   *srcfsp[INCLMAX+1];		// Pointers to source file specifications
long    savlineno[INCLMAX+1];	// Saved line number during .INCLUD
long    savlinesz[INCLMAX+1];	// Saved line size during .INCLUD
long    errflag;				// Error flag bits
int     condlvl;				// Conditional level
int     falselvl;				// False conditional level
char    condtbl[CONDMAX];		// Conditional table
int     totalerr;				// Total error count
int     mltcnt;					// Number of multiply defined symbols
int     undcnt;					// Number of undefined symbols
long    binofs;					// Binary output file address offset
int     binpsn;					// Psect number of binofs
VL      strval;
CL     *strclp;
char    strexp[SPCEXPSZ];
VL      dbgval;
CL     *dbgclp;
char    dbgexp[SPCEXPSZ];
VL      stkval;
CL     *stkclp;
char    stkexp[SPCEXPSZ];
long    iradix;					// Default input radix
long    lineno;					// Line number in input file
long    linesz;					// Line size (number of continuation lines + 1)
long    lstseq;					// Listing sequence number
int     pagnum;					// Listing page number
int     pagcnm;					// Listing page continuation number
int     lincnt;					// Listing line count
long    lsbnum;					// Local symbol block number
int     gnacnt;					// Generated argument number
char   *albpnt;					// Alpha listing buffer pointer
int     albcnt;					// Alpha listing buffer count
int     symsize;				// Size of symbol in symbuf
int     digcnt;					// Number of digit values in digbuf
int     symnum;
struct  fb *freelist;			// Memory free list head pointer
SY     *hashtbl[128];			// The hash table
SY     *symhead;				// Pointer to first symbol table entry
SY     *machead;				// Pointer to first macro table entry
char   *outfsp;					// Address of output file specification
FILE   *outfp;					// File pointer for output file

///char   *errtbl[ETBLSIZE];		// Array of pointers to error messages
///int     errcnt;					// Number of entries used in errtbl
///char  **errpnt;					// Pointer to current entry in errtbl

long    sectsize;				// Position in binary output file for size of
								//   current section
int     symnum;					// Number of symbol table entries
int     dbkcnt;					// Data block byte count
char   *dbkpnt;					// Data block pointer
char    dbkbuf[128];			// Data block buffer
int     nlscnt;					// .NLIST count
int     crfcnt;					// .NCREF count
int     lstpos;					// Position in listing line
char   *lstfsp;					// Address of list file specification
FILE   *lstfp;					// File pointer for list file

long    savesymflg;				// TRUE if should save compiler generated local
								//   symbols

int     prmcnt;					// Parameter file count

char    errltrs[8];				// Error letters string
char    timebfr[30];			// ASCII string of current date and time
union   sybf symbuf;			// Symbol name buffer
char    digbuf[DIGMAXSZ];		// Digit buffer
///char    blbbuf[BLBSIZE];		// Binary listing buffer
char    albbuf[ALBSIZE];		// Alpha listing buffer
char    hldchar;				// Held source character
char    stopper;				// Stopper character
char    catchr;					// Concatination character for macbody
char    lstmode;				// Listing mode
char    ptype;					// Processor type
char    pbits = B_8086;			// Processor bits
char    listtype;				// Listing type
char    segatrb;				// Current segment attributes
char    lsymflg = TRUE;			// TRUE if should include local symbols in
								//   OBJ file
char    gsymflg = TRUE;			// TRUE if should include global symbols in
								//   OBJ file
char    nopar;					// TRUE if N option seen
char    sleflg;					// TRUE if listing single letter error codes
char    test;					// TRUE if NOOUT option seen
char    lststd;					// TRUE if listing output is to stderr
char    eofflg;					// TRUE if EOF seen for last input
char    ttlflg;					// TRUE if .TITLE seen
char    bolflg;					// TRUE if at beginning of line
char    eolflg;					// TRUE if at end of line
char    eofreq;					// TRUE if should force end of file
char    ffflg;					// TRUE if have seen form-feed
char    notnull;				// TRUE if atom is not null
uchar   prnflg;					// TRUE if register names begin with %
uchar   dtxflg;					// TRUE if should default symbols to external
char    fhdflg;					// TRUE if have output first listing header
char    tocflg;					// TRUE if should list table of contents
char    tchflg;					// TRUE if have output header for table of
								//   contents
char    bnoflg;					// TRUE if binary output has been done
char    bexflg;					// TRUE if should not list binary data beyond
								//   first line
char    stack32;				// TRUE if generating code for 32 bit stack
uchar   gccflg;					// TRUE if using GCC mode
uchar   adjusted;				// TRUE if adjustment done by clfixvalues
uchar   havesection;			// TRUE if have seen a .SECTION pseudo-op
char    litchr = '#';
char    inxbgn = '[';			// Index field beginning character
char    inxend = ']';			// Index field ending character
char    inxsep1 = '+';
char    inxsep2 = '*';
char    copymsg[] =				// Our Copyright notice
         "Copyright (c) 1988-1992, Saguaro Software, Ltd.";
char    envname[] = "^XOS^XMAC^OPT";
								// The environment option name
char    prgname[] = "XMAC";		// Name of this program
char    hdrbfr[TITLESZ+1] = ".main.      ";
				// Text for title line
char    hdrstb[SBTTLSZ+1];		// Text for subtitle line

// Error message strings for listings

char *errmsgtbl[] =
{	"QSyntax error",							// ERR_Q  = 0x80000000
	"VIllegal value used in conditional or to specify size of block",
												// ERR_V  = 0x40000000
	"UUndefined symbol",						// ERR_U  = 0x20000000
	"UUndefined symbol",						// ERR_U2 = 0x10000000
	"AIllegal address mode",					// ERR_A  = 0x08000000
	"TValue truncated",							// ERR_T  = 0x04000000
	"RRelative value out of range",				// ERR_R  = 0x02000000
	"DMultiple definition of label",			// ERR_M  = 0x01000000
	"OOpcode field does not contain valid opcode, pseudo-op,"
                  " or macro",					// ERR_O  = 0x00800000
	"FConflicting attributes specified",		// ERR_C  = 0x00400000
	"XExpression error",						// ERR_X  = 0x00200000
	"MReference to multiplely defined label",	// ERR_D  = 0x00100000
	"KUnbalanced brackets",						// ERR_K  = 0x00080000
	"SSize of opcode does not match size of operand",
												// ERR_S  = 0x00040000
	"E.ERROR specified",						// ERR_E  = 0x00020000
	"NInvalid segment or msect name",			// ERR_I  = 0x00010000
	"NDifferent segment or msect name",			// ERR_DN = 0x00008000
	"CPseudo-op must be inside conditional",	// ERR_CE = 0x00004000
	"CConditional nesting is too deep",			// ERR_CX = 0x00002000
	"CUnterminated conditional",				// ERR_CO = 0x00001000
	"WUnterminated macro or repeat definition",	// ERR_UM = 0x00000800
	"AValue is not an address",					// ERR_NA = 0x00000400
	"ZNo psect specified for label, code, or data",
												// ERR_NP = 0x00000200
	"IIllegal .INCLUD or .REQUIR file specification"
};												// ERR_IN = 0x00000100

// Profarg option table

char *optfile(arg_data *arg, char *dftext);
///void  opthelp(void);
int   optlist(arg_data *arg);
int   optnoparm(void);
int   optout(arg_data *arg);
int   optsle(arg_data *arg);


long mskbits[4] = {1, 0, 0, 1};

#define OPT(name) ((int (*)(arg_data *))name)

arg_spec options[] =
{   {"?"        , 0                 , NULL   , OPT(opthelp)  , 0,
			"Display this text"},
    {"H*ELP"    , 0                 , NULL   , OPT(opthelp)  , 0,
			"Display this text"},

    {"CRE*F"    , ASF_LSVAL         , NULL   ,     optlist   , LT_CREF,
			"Specity cross-reference listing"},

    {"L*IST"    , ASF_LSVAL         , NULL   ,     optlist   , LT_NORMAL,
			"Specity normal listing"},

    {"O*UTPUT"  , ASF_LSVAL         , NULL   ,     optout    , TRUE,
			"Specity object file"},

    {"P*ARM"    , ASF_FLAG|ASF_STORE, (SUBOPTS *)mskbits, OPT(optnoparm), 0,
			"Use the .PARM pseudo-op"},

    {"DEBUG"    , ASF_LSVAL         , NULL   ,     optlist   , LT_DEBUG,
			"Specify debug listing"},

    {"COM*PACT" , ASF_FLAG|ASF_STORE, (SUBOPTS *)mskbits,     optsle    , FALSE,
			"Use compact format in listing (error letters only, no "
			"error descriptions"},
    {"SAVESYM"  , ASF_FLAG|ASF_STORE, (SUBOPTS *)mskbits,  OPT(&savesymflg), TRUE,
			"Save local compiler generated symbols"},

    {NULL       , 0        , NULL   ,     NULL      , 0}
};

char  example[] = "{/options} name1, name2, ...";
char  description[] = "This command invokes the GNU compilers.  Currently "
		"only the C compiler (CC1) is supported. Any options not specified "
		"here are passed to the compiler. This command can be used to "
		"compile, assemble, and link simple programs in one step. It can "
		"also be used (usually with XMAKE) to compile single program "
		"modules. It should be noted that while CC1 can be run directly, "
		"it's command syntax is quite different from other XOS commands. "
		"Some CC1 options also have a non-standard format which is not "
		"translated by XCC (other than converthing leading /C to -. "
		"Compiler options which use a space to seperate an argument "
		"must use an underscore instead.";


//***************************************
// Function: main - Main program for XMAC
// Returned: 0 if normal or 1 if error
//***************************************

int main(argc, argv)
int  argc;
char **argv;

{
    char *apnt[2];

    fprintf(stderr, "XMAC - version %d.%d\n", version, editnum);
	reg_pib(&pib);
    pib.opttbl = options; 				// Load the option table
    pib.kwdtbl = NULL;
    pib.build = __DATE__;
    pib.majedt = VERSION; 				// Major edit number
    pib.minedt = EDITNO; 				// Minor edit number
    pib.copymsg = copymsg;
    pib.prgname = prgname;
    pib.desc = description;
    pib.example = example;
    pib.errno = 0;
    getTrmParms();
    getHelpClr();
    if (argc >= 2)
    {
        if(svcSysGetEnv(envname, (cchar **)&apnt[0]) > 0)
        {
            apnt[1] = NULL;
            progarg(apnt, PAF_INDIRECT, options, NULL, (int (*)(char *))NULL,
                    (void (*)(char *, char*))NULL, (int (*)(void))NULL, ".DFT");
        }
        ++argv;
        progarg(argv, PAF_INDIRECT|PAF_ECHOINDIR, options, NULL, havearg,
                (void (*)(char *, char *))NULL, (int (*)(void))NULL, ".CMD");
    }
    if (firstsrc == NULL)				// Did we get any files to assemble?
    {
        fputs("? XMAC: No input files specified\n", stderr);
        exit(1);
    }
    assemble();							// Do the work
    return (((totalerr + undcnt + mltcnt) != 0) ? 1 : 0);
}


//*********************************************
// Function: havearg - Process normal arguments
// Returned: TRUE if OK, FALSE if error
//*********************************************

int havearg(
    char *str)					// Pointer to argument string

{
    thissrc = getfsp(str, SB_fsp, ".M86"); // Get file specification
    thissrc->sb_next = 0;				// Initialize the source block
    lastsrc->sb_next = thissrc;			// Link to previous source block
    lastsrc = thissrc;
    return (TRUE);
}


//****************************************
// Function: optlist - Process LIST option
// Returned: TRUE if OK, FALSE if error
//****************************************

int optlist(
    arg_data *arg)
{
    if (lstfsp)
    {
		fputs("? XMAC: More than one list file specified\n", stderr);
		return (FALSE);
    }
    if ((lstfsp = optfile(arg, ".LST")) == 0)
        lstfsp = "";					// Get listing file specification
    listtype = arg->data;
    return (TRUE);
}


//********************************************
// Function: optnoparm - Process NOPARM option
// Returned: TRUE if OK, FALSE if error
//********************************************

int optnoparm(void)
{
    nopar = TRUE;						// Indicate should ignore .PARM
    return (TRUE);
}


//*****************************************
// Function: optout - Process OUTPUT option
// Returned: TRUE if OK, FALSE if error
//*****************************************

int optout(
    arg_data *arg)
{
    if (outfsp)
    {
		fputs("? XMAC: More than one output file specified\n", stderr);
		return (FALSE);
    }
    outfsp = optfile(arg, ".OBJ");		// Get output file specification
    test = arg->data;
    return (TRUE);
}


//********************************************************
// Function: optsle - Process COMPACT and EXPANDED options
// Returned: TRUE if OK, FALSE if error
//********************************************************

int optsle(
    arg_data *arg)
{
    sleflg = arg->data;
    return (TRUE);
}


//****************************************
// Function: opthelp - Process HELP option
// Returned: TRUE if OK, FALSE if error
//****************************************

/*
void opthelp(void)

{
    fprintf(stderr, "\nXMAC version %d.%d (%s) %s\n\n", version, editnum,
            __DATE__, copymsg);
    fputs("XMAC {{-}{/}option{=val}} {file_specs} {{-}{/}option{=val} ...\n"
            " Options:\n", stderr);
    helpprint(" ?        - Displays this list", FALSE, TRUE);
    helpprint(" CREF     - Requests CREF listing file, specifies listing file",
            (listtype == LT_CREF), TRUE);
    helpprint(" HELP     - Displays this list", FALSE, TRUE);
    helpprint(" LIST     - Requests normal listing file, specifies listing"
            " file", (listtype == LT_NORMAL), TRUE);
    helpprint(" NOOUTPUT - Indicates no binary output", test, TRUE);
    helpprint(" NOPARM   - Indicates should ignore .PARM pseudo-op", nopar,
            TRUE);
    helpprint(" OUTPUT   - Requests binary output, specifies output file",
            !test, TRUE);
    helpprint(" DEBUG    - Requests debug listing file, specifies listing file",
            (listtype == LT_DEBUG), TRUE);
    fputs("All options can be abbreviated to 3 letters. CREF, HELP, LIST, and"
            " OUTPUT can\n  be abbreviated to 1 letter.\n", stderr);
    fputs("Use the DEFAULT command to change any default option. (* indicates"
            " current\n  default)\n", stderr);
    exit (0);
}
*/

//************************************************
// Function: helpprint - Print help option entries
// Returned: Nothing
//************************************************

void helpprint(
    char *helpstring,
    int state,
    int newline)

{
    char strbuf[132];

    if (state)
    {
        strmov(strmov(strbuf, helpstring), " *");
        helpstring = strbuf;
    }
    fprintf(stderr, (newline)? "%s\n": "%-38s", helpstring);
}

//******************************************************
// Function: optfile - Get file specification for option
// Returned: Pointer to file specfication
//******************************************************

char *optfile(
    arg_data *arg,				// Pointer to progarg argument data block
    char     *dftext)			// Default extension

{
    char *temp;

    if (arg->flags & ADF_LSVAL)			// Was a file spec given?
    {					// Yes
	temp = (char *)getfsp(arg->val.s, 0, dftext);
        if (*temp != '.')
            return (temp);
    }
    return (NULL);						// No
}


//**********************************************
// Function: getfsp - Get file specification
// Returned: Pointer to file specification block
//**********************************************

struct sb *getfsp(
    char *arg,					// Pointer to file specifcation string
    int   offset,				// Offset from start of block for name
    char *dftext)				// Default extension

{
    char *pnt;
    char *blk;
    int   size;
    char  haveext;
    char  chr;

    haveext = FALSE;					// Assume not extension
    pnt = arg;							// Point to the argument value
    while ((chr = *pnt++) != '\0')		// Scan string to find periods and to
    {									//   get length
        if (chr == '.')					// Start of extension?
            haveext = TRUE;				// Yes - remember that
    }
    size = (int)(pnt - arg) + offset + 2;
    if (!haveext)
	size += 4;
    blk = (char *)getspace((long)size);	// Get memory for file name block
    pnt = strmov(blk+offset, arg);		// Copy file spec
    if (!haveext)
	strmov(pnt, dftext);
    return ((struct sb *)blk);
}


//*********************************************************************
// Function: defspec - Setup defaults for output specification. If no
//		fsp is setup at all, use string specified by "none", if
//		this is null, use device and name from last source
//		specification with extension from "ext".  If name is
//		given, use complete specification as given.  If no name
//		is given, use device if it is given, otherwise use null
//		device name; use name from last source specification
//		with extension that is given.
// Returned: Pointer to file specification string
//*********************************************************************

char *defspec(
    char *fsp,
    char *none,
    char *ext)

{
    int   size;
    char *pnt1;
    char *pnt2;
    char *pnt3;
    char *pnt4;
    char *pntm;

    if (fsp == NULL)					// Was anything at all given?
    {
	if (none)							// No - do we have default string?
	    return (none);					// Yes - use it
	else
	{
	    pnt1 = lastsrc->sb_fsp;			// No - use device and name from
	    pnt2 = strchr(pnt1, '.');		//   last source specification
	    pntm = getspace((long)(pnt2 - pnt1 + 5));
	    pnt2 = pntm;
	    while ((*pnt2++ = *pnt1++) != '.');
	    strcpy(pnt2, ext);				// Add default extension
	    return(pntm);
	}
    }
    else
    {   if ((pnt2 = strchr(fsp, ':')) != NULL)	// Have specification - have device?
	    ++pnt2;							// Yes
	else
	    pnt2 = fsp;						// No
	if (*pnt2 == '.')					// Was a name given?
	{   pnt1 = lastsrc->sb_fsp;			// No - point to last source spec
	    if ((size=(int)(pnt2 - fsp)) == 0 // Was a device given?     
			&& (pnt3=strchr(pnt1, ':')) != NULL)
		size += (int)(++pnt3 - pnt1);	// Yes - add in its length
	    else							// In any case, point to start of
		pnt3 = pnt1;					//   name

	    pnt4 = strchr(pnt3, '.');		// Find end of source name
	    size += (int)(pnt4 - pnt3);		// Add in length of name
	    pnt4 = strchr(fsp, '.');		// Find length of output extension
	    size += strlen(++pnt4);			// Add it in
	    pntm = getspace((long)(size+2)); // Get memory
	    if (pnt2 != fsp)				// Was an output device specified?
	    {
		pnt2 = pntm;					// Yes - use it
		pnt1 = fsp;
		while ((*pnt2++ = *pnt1++) != ':');
		pnt1 = pnt3;					// Skip source device
	    }
	    else
		pnt2 = pntm;

	    while ((*pnt2++ = *pnt1++) != '.');// Copy source name
	    strcpy(pnt2,pnt4);				// Copy given output extension
	    return (pntm);
	}
	else
	    return (fsp);
    }
}
