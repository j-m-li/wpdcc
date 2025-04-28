//***********************
// Main program for XLINK
//***********************
// Written by John Goltz
//***********************

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
#include <errno.h>
#include <string.h>
#include <xos.h>
#include <xcmalloc.h>
#include <xcstring.h>
#include <xossvc.h>
#include <xoserrmsg.h>
#include <xosstr.h>
#include <procarg.h>
#include "xlink.h"


// v 1.15 - 2-May-92
//	Changed command processor to accept any characters in file names
// v 1.16 - 8-May-92
//	Fixed some bugs in library handling, converted to use procarg
// v 2.0 - 13-May-92
//	Changed to use CLIBTV
// v 2.1 - 19-Jul-92
//	Fixed bug in filetwo which caused failure if unneeded library was
//	specified
// v 2.2 - 17-Oct-92
//	Added help text
// v 2.3 - 30-Aug-93
//	Fixed bug in handling absolute addresses in pass 2
// v 2.4 - 5-Dec-93
//	Added support for symbol numbers; changed default extension to .OBJ.
// v 2.5 - 7-Dec-93
//	Several bug fixes for symbol number support.
// v 2.6 - 8-Dec-93
//	Changed to handle symbol definitions without extra byte for absolute
//	non-address symbols
// v 2.7 - 23-Dec-93
//	Added support for MS OBJ files; major clean-up and fixes otherwise.
// v 3.0 - 27-Dec-93
//	Converted to 32-bit.
// v 3.1 - 29-Dec-93
//	Added support for line number symbols for MS OBJ files.
// v 3.2 - 11-Jan-94
//	Changed to use heapsort for sorting symbols in the map; changed to
//	start each module's part of each psect according to the psect's modulus.
// v 3.3 - 9-May-94
//	Added support for version 2 RUN files, added support for imported
//	and exported symbols.
// v 3.4 - ????
// v 3.5 - 8-Jul-94
//	Added support for MS communal items; fixed several bugs relating to
//	relocation.
// v 3.6 - 12-Jul-94
//	Fixed problem with generating relocation fixup stuff introduced in 3.5;
//	fixed problem with fixup handling for MS COMDEF records.
// v 3.7 - 16-Jul-94
//	Added reasonable error message for unsupported relative reference to
//	absolute symbol (did cause page fault).
// v 3.8 - 4-Oct-94
//	Fixed problem with processing libraries in pass one, was not saving
//	value of objamnt.
// v 3.9 - 30-Nov-94 (FPJ)
//	Added prototype for nullfunc().
// v 3.10 - 28-Sep-95 (JRG)
//	Merged dirvergent versions, no know changes!
// v 3.11 - 22-Dec-95 (JRG)
//	Fixed problem with not initializing the msbuffer pointer if loading an
//	MS module from an XOS library when no other MS modules had been
//	loaded.
// v 3.12 - 29-Jun-98 (JRG)
//	Fixed problem with calculating size of exported symbol table
//	incorrectly.  (Changed to always output 4 byte values of exported
//	symbols since need length of value before value is relocated!)
// v 3.13 - 26-Sep-98 (JRG)
//	Fixed minor problem with displaying selector values in the map.
// v 4.3  - 22-Sep-06
//  Added CHECK and NOCHECK options
// v 4.6  - 29-Jun-09
//  Fixed problem with some fully defined symbols being stored in the
//  the symbol file as relocatable.
// v 4.7  - ???
// v 4.8  - 20-Sep-09
//  Fixed problem with MS fixup for absolute symbols. Value was off by 4.
// v 4.9.0 - 1-Nov-09
//	Added version options to specify version numbers to be included in the
//  The RUN file generated.
// v 4.9.2 - 15-Nov-09
//  Enhanced error reporting for segment type conflicts.
// v.4.9.3 - 15-Dec-09
//	Added summary listing to load map, removed check for MS group conflicts,
//  changed to ignore empty MS groups.
// v.4.9.4 - 21-Dec-09
//  Added code to junk contents of $$SYMBOLS and $$TYPES MS segments (temp
//  until we fix owcx).

// XLINK will accept either XOS or MS (OMF) obj files as input. The two
//   types may be mixed to produce a RUN file.  This requires that we map
//   between the XOS and MS memory structures. XOS strucrtures memory as a
//   collection of segments, (SEGs) each of which has a unique selector
//   value. Within each segment is a collection of MSECTS each of which is
//   a contiguous area of memory.  Each MSECT is made up of a collection
//   of PSECTs, each of which is also a contiguous area of memory.  The
//   PSECTs in an MSECT are tightly packed.  The MS memory structure consists
//   of segments and groups. These DO NOT directly map to the corresponding
//   named XOS elements. An MS segment which is not a member of a group is
//   treated as an XOS PSECT. The name of the PSECT is formed by adding _p
//   after the MS segment name. If this psect is already defined it is used.
//   If it not defined, it is created as a member of the MSECT whose name
//   is formed by adding _m to the end of the class name of the MS segment.
//   This MSECT must have been previously defined by an XOS OBJ file.
//   If an MS segment is a member of a group, it is again treated as a PSECT
//   as described above. If the PSECT is already defined, the MS group
//   membership is ignored. If it is not defined, the name of its MSECT is
//   the group name followed by _m. This msect most also be previously defined.
//   This handling of MS groups is not perfect, but it does allow the MS group
//   definitions to be used in simple cases or easily overridden. This is
//   sometimes necessary because most C compiliers do not provide a way to
//   override group definitions.

// Unlike the MS linker, XLINK has no built in assumtions about segment,
//   MSECT, or PSECT names or order. It is a simple mater to fully specify
//   all memory elements using the .SEG, .MSECT, and .OPCODE pseudo=ops
//   in assembly code. Normally a small OBJ file is created which specifies
//   all of the memory elements for a program and is loaded first.

// XLINK will accept XOS format library files which contain either XOS or MS
//   (OMF) or a mixture of XOS and MS (OMF) object files. This results in a
//   ambiguity with the first section as the normal XOS module header may
//   start with 0x80 which is the normal initial section type for an OMF file.
//   This is resolved by inserting a special two-byte header (0xC1, 0) which
//   indicates that an OMF object module follows. This is similar to the two-
//   byte module header used before all XOS modules. The index entry for an
//   OMF module points to this extra header.

// Allocate global data

//int    version = CFG_XLINK_VER_MAJ;	// LINK version number
//int    editnum = CFG_XLINK_VER_MIN;	// LINK edit number

extern void nullfunc(void);

long   offset;				// Current offset in RUN file
long   rdoffset;			// Offset in RUN file for start of relocation
							//   data
long   symtblsize;			// Number of symbol table entries
long   stbtotal;			// Number of in-memory debugger symbols
long   outbuftop;
long   outbufsize;
int    modulenum;			// Module number
int    seggbn;				// Current global segment number
int    mscgbn;				// Number of msects
int    pscgbn;				// Current global psect number
int    symsize;				// Length of symbol in symbuf
int    mltcnt;				// Number of multiply defined symbols
int    undefcnt;			// Number of undefined symbols seen
long   outdd;				// Device descriptor for output file
long   symdd;				// Device descriptor for symbol table file
long   stradr;				// Starting address for program
PD    *strpsd;				// Address of psect data block for starting
							//   address
long   stkadr;				// Initial stack address
PD    *stkpsd;				// Address of psect data block for initial
							//   stack address
long   dbgadr;				// Debugger address for program
PD    *dbgpsd;				// Address of psect data block for debugger
							//   address
PSTK  *polspnt;				// Polish expression stack pointer
PSTK   polstk[POLSSIZE];	// The polish expression stack
PD     abspsd;				// Psect data block used to link absolute
							//   symbols
PD    *curpsd;				// Pointer to psect data block for psect
							//   being loaded
MD    *curmsd;
SD    *cursgd;
PD    *stbpsd;				// Pointer to psect data block for psect
							//   containing debugger symbol table
MD    *stbmsd;				// Pointer to msect data block for msect
							//   containing debugger symbol table
MSGB  *msgbhead;			// First MS group MSGB
MSGB  *msgbtail;			// Last MS group MSGB
PD   **psnumpnt;
int    psnumcnt;
MD   **msnumpnt;
int    msnumcnt;
SD   **segnumpnt;
int    segnumcnt;

uchar *msbuffer;
int    mscount;
ulong  msoffset;
void (*msdispatch)(void) = (void (*)(void))nullfunc;

ulong  stbsize;				// Space needed for debugger symbol names
ulong  stbnum;				// Number of entries in the debugger symbol
							//   table
ulong  stbxsp;				// Number of extra entries in debugger symbol
							//   table
int    seccnt;				// Byte count for section
int    sectype;				// Section type
int    sectypx;
long   objdd;				// Device descriptor for OBJ file
int    objcnt;				// Bytes left in current OBJ file block
uchar *objpnt;				// Pointer to OBJ file buffer
int    objamnt;

long   objblk;				// Current block number for OBJ file

OB    *firstobj;			// Pointer to first OBJ data block
OB    *curobj;				// Address of OBJ file block for current file

OB   **prevobj = &firstobj;	// Address of previous OBJ file block next
							//   pointer
MB    *curmod;
char  *firstname;			// Pointer to copy of name of first OBJ file
char  *firstext;
SD    *sgdhead = 0;				// Segment data block list head pointer
SD    *sgdtail;				// Segment data block list tail pointer
SY   **symnumbase;			// Current symbol number table base
SY    *hashtbl[HASHSIZE];	// The hash table

SY    *importhead;
SY   **importlast = &importhead;
long   importnum;			// Number of imported symbols
long   importnumoffset;		// Offset in file to store header pointers
long   importos;			// Offset in file to store imported symbol data
SY    *exporthead;
SY    *exporttail;
long   exportnum;
long   exporttotal;

struct _mab symnummem = {SYMNUMBASE, SYMNUMBASE, SYMNUMBASE};
struct _mab segnummem = {SEGNUMBASE, SEGNUMBASE, SEGNUMBASE};
struct _mab msectnummem = {MSECTNUMBASE, MSECTNUMBASE, MSECTNUMBASE};
struct _mab psectnummem = {PSECTNUMBASE, PSECTNUMBASE, PSECTNUMBASE};
struct _mab lnamesmem = {LNAMESBASE, LNAMESBASE, LNAMESBASE};

uchar *auxloc;					// Address of auxillary buffer
uchar *objloc;					// Address of OBJ file buffer
uchar *objbuf;					// Address of current input buffer
char  *symbufr;					// Address of symbol table file output buffer
char  *outfsp;					// Pointer to output file specification
char  *mapfsp;					// Pointer to map file specification
char  *symfsp;					// Pointer to symbol file specification
FILE  *mapfp;					// File pointer for map file
long   debgaddr;				// Address where debugger is loaded
int    opsymsz;					// Size of symbol table entry excluding name
								//   field, 0 if not loading symbol table
int    majvrsn;					// Major version number of program being linked
int    minvrsn;					// Minor version number of program being linked
int    editnum;					// Edit number of program being linked
int    vendorlen;
char   vendor[64];				// Vendor name for program being linked
int    fheadsz;
uchar  ptype;					// Processor type
char   itype = I_XOSUSER;		// Image type
char   opsymwrd;				// TRUE if symbol table entries must be word
								//   aligned
char   opsymord;				// TRUE if symbol table stored low order byte
								//   first
char   copymsg[] = "";
char   prgname[] = "XLINK";		// Our name
char   envname[] = "^XOS^XLINK^OPT"; // The environment option name
char   inpext[] = ".OBJ";		// Default extension for input files
char   outext[] = ".RUN";		// Default extension for output image file
char   symext[] = ".SYM";		// Default extension for symbol table file
char   mapext[] = ".MAP";		// Default extension for map file
char   cmdext[] = ".LNK";		// Default extension for command file
char   objtype;					// Type of current OBJ file
char   modtype;					// Type of current module
char   needrun = FALSE;			// TRUE if generating RUN file
char   needexe = FALSE;
char   needmap = FALSE;					// TRUE if generating MAP file
char   needsym = FALSE;					// TRUE if generating SYM file
char   lowfirst;				// TRUE if byte order is low order byte first
char   libflag;					// TRUE if loading from library file
char   mapflag;					// TRUE if map output is not to the console
char   sumflag;					// TRUE if want final summary displayed
char   debugflag;				// TRUE if want debug output to console
uchar  multstr;					// TRUE if more than one start address seen
uchar  multdbg;					// TRUE if more than one debugger address seen
uchar  multstk;					// TRUE if more than one stack address seen
uchar  checkflg = TRUE;			// TRUE if should check for address conflict
char   done;					// Done flag
char   errorflg;				// TRUE if have fatal error
char   symblk;					// True if more than 1 block in symbol table
								//   file
char   modname[SYMMAXSZ+3];		// Name of current module
char   symbuf[SYMMAXSZ+3];		// Symbol name buffer
char   haveweak = FALSE;		// TRUE if have weak symbols
char   havelazy = FALSE;		// TRUE if have lazy symbols

// Procarg option table

#define AF(func) (int (*)(arg_data *))func

arg_spec options[] =
{   {"?"        , 0        , NULL, AF(opthelp) , 0},
    {"ALO"      , 0        , NULL, AF(optalone), 0},
    {"ALONE"    , 0        , NULL, AF(optalone), 0},
    {"H"        , 0        , NULL, AF(opthelp) , 0},
    {"HEL"      , 0        , NULL, AF(opthelp) , 0},
    {"HELP"     , 0        , NULL, AF(opthelp) , 0},
    {"LKE"      , 0        , NULL, AF(optlke)  , 0},
    {"O"        , ASF_LSVAL, NULL,    optout   , TRUE},
   {"OUT"      , ASF_LSVAL, NULL,    optout   , TRUE},
   {"OUTPUT"   , ASF_LSVAL, NULL,    optout   , TRUE}, 
   {"EXE"   , ASF_LSVAL, NULL,    optexe   , TRUE}, 

    {"M"        , ASF_LSVAL, NULL,    optmap   , 0},
    {"MAP"      , ASF_LSVAL, NULL,    optmap   , 0},
    {"NOO"      , 0        , NULL,    optout   , FALSE},
    {"NOOUT"    , 0        , NULL,    optout   , FALSE},
    {"NOOUTPUT" , 0        , NULL,    optout   , FALSE},
    {"SYM"      , ASF_LSVAL, NULL,    optsym   , 0},
    {"SYM86"    , ASF_NVAL , NULL,    opts86   , 0},
    {"SYM386"   , ASF_NVAL , NULL,    opts386  , 0},
    {"SYM68K"   , ASF_NVAL , NULL,    opts68k  , 0},
    {"SUM"      , 0        , NULL,    optsum   , TRUE},
    {"SUMMARY"  , 0        , NULL,    optsum   , TRUE},
    {"NOSUM"    , 0        , NULL,    optsum   , FALSE},
    {"NOSUMMARY", 0        , NULL,    optsum   , FALSE},
	{"CHECK"    , 0        , NULL,    optcheck , TRUE},
	{"CHE"      , 0        , NULL,    optcheck , TRUE},
	{"NOCHECK"  , 0        , NULL,    optcheck , FALSE},
	{"NOCHE"    , 0        , NULL,    optcheck , FALSE},
    {"DEBUG"    , 0        , NULL,    optdebug , TRUE},
    {"NODEBUG"  , 0        , NULL,    optdebug , FALSE},
    {"MAJ"      , ASF_NVAL , NULL,    optvrsn  , (long)&majvrsn},
    {"MAJVRSN"  , ASF_NVAL , NULL,    optvrsn  , (long)&majvrsn},
    {"MIN"      , ASF_NVAL , NULL,    optvrsn  , (long)&minvrsn},
    {"MINVRSN"  , ASF_NVAL , NULL,    optvrsn  , (long)&minvrsn},
    {"EDI"      , ASF_NVAL , NULL,    optvrsn  , (long)&editnum},
    {"EDITNUM"  , ASF_NVAL , NULL,    optvrsn  , (long)&editnum},
    {"VEN"      , ASF_LSVAL, NULL,    optvendor, 0},
    {"VENDOR"   , ASF_LSVAL, NULL,    optvendor, 0},
    {NULL       , 0        , NULL,    NULL     , 0}
};


#if CFG_XLINK_MULTIPASS
int		g_cUndefPrev = 0;
int		g_nPass1Lev	 = 0;
#endif


//****************************************
// Function: main - Main program for XLINK
// Returned: 0 if normal, 1 if error
//****************************************

int main(
    int  argc,
    char *argv[])
{
    char *apnt[2];

	fprintf(stderr, "XLINK - version %d.%d.%d\n", MV, NV, EN);
	if (argc >= 2)
	{
		if(svcSysGetEnv(envname, (const char **)&apnt[0]) > 0)
		{
			apnt[1] = NULL;
			procarg((cchar **)apnt, PAF_INDIRECT, options, NULL,
					(int (*)(char *))NULL, (void (*)(char *, char*))NULL,
					(int (*)(void))NULL, ".DFT");
		}
		++argv;
		procarg((cchar **)argv, PAF_INDIRECT|PAF_ECHOINDIR, options, NULL,
				havearg, (void (*)(char *, char *))NULL, (int (*)(void))NULL,
				cmdext);
	}
	if (firstobj == NULL)				// Did we get any files to load?
	{
		fputs("? XLINK: No input files specified\n", stderr);
		exit(1);
	}
	objloc = (uchar *)getspace(512);	// Allocate memory for input buffer

	if (needmap)
		mapbegin();

#if CFG_XLINK_MULTIPASS

	undefcnt	= 0;

	do
	{
		g_cUndefPrev	= undefcnt;		// Previous undefined count

//@@@	undefcnt		= 0;

    	passone();						// Do first pass

		fprintf( stderr, "%% XLINK:passone[%d] undefcnt=%d\n"
			, g_nPass1Lev, undefcnt );

		g_nPass1Lev++;

	} while( undefcnt && undefcnt != g_cUndefPrev && g_nPass1Lev < 7 );

#else
    passone();							// Do first pass
#endif

	if (needmap)						// If generating a map
		mapout();						// Generate the map
    passtwo();							// Do second pass
	if (sumflag)						// Want summary?
		dosummary();					// Yes
    if (mapfp)							// If generating a map
    {
        if (fclose(mapfp) < 0)			// Close map output
			femsg(prgname, -errno, mapfsp);
    }
	dofilesymtbl();						// Generate symbol file if its needed
    return (((undefcnt+mltcnt) == 0) ? errorflg : 1);
}


//*************************************************
// Function: havearg - Process non-option arguments
// Returned: TRUE if normal, FALSE if error
//*************************************************

int havearg(
	char *arg)
{
    curobj = (OB *)getfsp(offsetof(OB, obj_fsp), arg, inpext);
										// Get file specification
	curobj->obj_magic = 'JBO*';
    curobj->obj_next = NULL;			// Initialize the OBJ block
    curobj->obj_mdl = 0;
    *prevobj = curobj;					// Link to previous OBJ block
    prevobj = &curobj->obj_next;
    return (TRUE);
}


//*******************************************
// Function: optout - Process /OUTPUT options
// Returned: TRUE if normal, FALSE if error
//*******************************************

int optout(
    arg_data *arg)
{
    needrun = (char)(arg->data);
    outfsp = optfile(arg, outext);
    return (TRUE);
}

int optexe(
    arg_data *arg)
{
    needexe = (char)(arg->data);
    outfsp = optfile(arg, ".exe");
    return (TRUE);
}


//*****************************************
// Function: optmap - Process /MAP option
// Returned: TRUE if normal, FALSE if error
//*****************************************

int optmap(
    arg_data *arg)
{
    needmap = TRUE;
    mapfsp = (arg->flags & ADF_NULL)? "#": optfile(arg, mapext);
    return (TRUE);
}


//*****************************************
// Function: optsym - Process /SYM option
// Returned: TRUE if normal, FALSE if error
//*****************************************

int optsym(
    arg_data *arg)
{
    needsym = TRUE;
    symfsp = optfile(arg, symext);
    return (TRUE);
}


//*******************************************
// Function: optalone - Process /ALONE option
// Returned: TRUE if normal, FALSE if error
//*******************************************

int optalone(void)
{
    itype = I_ALONE;
    return (TRUE);
}


//*****************************************
// Function: optlke - Process /LKE option
// Returned: TRUE if normal, FALSE if error
//*****************************************

int optlke(void)
{
    itype = I_XOSLKE;
    return (TRUE);
}


//*****************************************
// Function: opts86 - Process /SYM86 option
// Returned: TRUE if normal, FALSE if error
//*****************************************

int opts86(
    arg_data *arg)
{
    stbxsp = arg->val.n;				// Get size of extra space for symbol
    opsymsz = 6;
    opsymord = TRUE;
    opsymwrd = FALSE;
    return (TRUE);
}


//*******************************************
// Function: opts386 - Process /SYM386 option
// Returned: TRUE if normal, FALSE if error
//*******************************************

int opts386(
    arg_data *arg)
{
    stbxsp = arg->val.n;				// Get size of extra space for symbol
    opsymsz = 8;
    opsymord = TRUE;
    opsymwrd = FALSE;
    return (TRUE);
}


//*******************************************
// Function: opts68k - Process /SYM68K option
// Returned: TRUE if normal, FALSE if error
//*******************************************

int opts68k(
    arg_data *arg)
{
    stbxsp = arg->val.n;				// Get size of extra space for symbol
    opsymsz = 8;
    opsymord = FALSE;
    opsymwrd = TRUE;
    return (TRUE);
}


//*********************************************************************
// Function: optvrsn - Process /MAJVRSN, /MINVRSH, and /EDITNUM options
// Returned: TRUE if normal, FALSE if error
//*********************************************************************

int optvrsn(
    arg_data *arg)
{
	*(int *)(arg->data) = arg->val.n;
    return (TRUE);
}


//**********************************************
// Function: optvendor - Process /VENDOR options
// Returned: TRUE if normal, FALSE if error
//**********************************************

int optvendor(
    arg_data *arg)
{
	if ((vendorlen = arg->length) > 63)
		vendorlen = 63;
	memcpy(vendor, arg->val.s, vendorlen);
	vendor[vendorlen] = 0;
	return (TRUE);
}


//***********************************************************
// Function: optsum - Process /SUMMARY and /NOSUMMARY options
// Returned: TRUE if normal, FALSE if error
//***********************************************************

int optsum(
    arg_data *arg)
{
    sumflag = arg->data;
    return (TRUE);
}


//*********************************************************
// Function: optcheck - Process /CHECK and /NOCHECK options
// Returned: TRUE if normal, FALSE if error
//*********************************************************

int optcheck(
    arg_data *arg)
{
    checkflg = arg->data;
    return (TRUE);
}


//*********************************************************
// Function: optdebug - Process /DEBUG and /NODEBUG options
// Returned: TRUE if normal, FALSE if error
//*********************************************************

int optdebug(
    arg_data *arg)
{
    debugflag = arg->data;
    return (TRUE);
}


//*****************************************
// Function: opthelp - Process /HELP option
// Returned: TRUE if normal, FALSE if error
//*****************************************

void opthelp(void)
{
    fprintf(stderr, "\nXLINK version %d.%d.%d(%s) %s\n\n", MV, NV, EN,
			__DATE__, copymsg);
    fputs("XLINK {{-}{/}option{=val}} {file_specs} {{-}{/}option{=val} ...\n"
            " Options:\n", stderr);
    helpprint(" ?        - Displays this list", FALSE, TRUE);
    helpprint(" ALONE    - Specifies standalone image", (itype==I_ALONE), TRUE);
    helpprint(" HELP     - Displays this list", FALSE, TRUE);
    helpprint(" LKE      - Specifies LKE image", (itype==I_XOSLKE), TRUE);
    helpprint(" NOOUTPUT - Indicates no executable file output", FALSE, TRUE);
    helpprint(" OUTPUT   - Requests executable output file, specifies file",
            !needrun, TRUE);
    helpprint(" SYM      - Requests symbol table file, specifies file",
            (symfsp != NULL), TRUE);
    helpprint(" SYM86    - Requests in-memory 8086 format symbol table",
            (opsymsz == 6), TRUE);
    helpprint(" SYM386   - Requests in-memory 80386 format symbol table",
            (opsymsz == 8 && opsymord == TRUE), TRUE);
    helpprint(" SYM68K   - Requests in-memory 68000 format symbol table",
            (opsymsz == 8 && opsymord == FALSE), TRUE);
    helpprint(" NOCHECK  - Do not check for msect address conflicts",
			checkflg, TRUE);
    helpprint(" CHECK    - Check for msect address conflicts", checkflg, TRUE);
    fputs("All options except SYM86, SYM386, and SYM68K can be abbreviated to"
            " 3 letters.\n  HELP, MAP, and OUTPUT can be abbreviated to 1"
            " letter.\n", stderr);
    fputs("Use the DEFAULT command to change any default option. (* indicates"
            " current\n  default)\n", stderr);
    exit (0);
}


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
// Returned: Pointer to file spec or NULL if none
//******************************************************

char *optfile(
    arg_data *arg,
    char     *dftext)			// Default extension

{
    if (arg->flags & ADF_LSVAL)			// Was a file spec given?
		return (getfsp(0, arg->val.s, dftext)); // Yes - return it
    else
		return (NULL);					// No - return NULL
}


//******************************************
// Function: getfsp - Get file specification
// Returned: Pointer to OBJ block
//******************************************

char *getfsp(
    int   offset,		// Offset from start of block for name
    char *fsp,
    char *dftext)		// Default extension

{
    char *pnt1;
    char *pnt2;
    char *blk;
    long  size;
    char  haveext;

    haveext = (strchr(fsp, '.') != NULL);
    size = offset + strlen(fsp) + ((haveext)? 1: 5);
    blk = getspace(size);		// Get memory for OBJ block
    pnt1 = blk + offset;		// Point to place to put file spec
    pnt2 = strmov(pnt1, fsp);
    if (!haveext)
        strmov(pnt2, dftext);
    return (blk);
}
