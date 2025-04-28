//******************************************************
// Routines to read single file for first pass for XLINK
//******************************************************
// Written by John Goltz
//******************************************************

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

#include <STDIO.H>
#include <STDLIB.H>
#include <STRING.H>
#include <CTYPE.H>
#include <XOS.H>
#include <PROCARG.H>
#include <XCMALLOC.H>
#include <XOSSTR.H>
#include "XLINK.H"

// Allocate local data
extern void nullfunc(void);

static MB   **modlst;		// Module list add pointer
static long   svdblk;		// Place to save objblk while reading library entry
							//   section
static int    libcnt;		// Place to save objcnt while loading library
static uchar *libpnt;		// Place to save objpnt while loading library
static int    libsec;		// Place to save seccnt while loading library
static long   libblk;		// Place to save objblk while loading library
static long   libamnt;		// Place to save objamnt while loading library
static char   liblow;		// Place to save lowfirst while loading library
static char   libdone;		// Library input done flag


static MD  *declmsect(SD *sgd, int type, int flags, int max, int priv,
		long addr);
static SD  *declseg(int type, int flags, int linked, int priv, long addr,
		int select);
static PD  *declpsect(MD *msd, int flags, long addr, int mod, long tsize,
		long lsize);
static SY  *deflocal(int header, PD *psd, ulong value, uint selector);
static void ms1mcomdat(void);
static void ms1mcomdef(void);
static void ms1mcoment(void);
static void ms1mextdef(void);
static void ms1mflcexd(void);
static void ms1mgrpdef(void);
static void ms1mledata(void);
static void ms1mlidata(void);
static void ms1mlinnum(void);
static void ms1mlnames(void);
static void ms1mlocsym(void);
static void ms1mmodend(void);
static void ms1mpubdef(void);
static void ms1msegdef(void);
static void ms1mtheadr(void);
static void ms1mtypdef(void);
static void onedata(void);
static void onedebug(void);
static void onedsvl(int header);
static void onedsvn(int header);
static void onedsvs(int header);
static void oneendmod(void);
static void oneexport(void);
static void oneextern(void);
static void oneimport(void);
static void oneintern(void);
static void onelibent(void);
static void onelocal(void);
static void onemsect(void);
static void onepsect(void);
static void onepush(int header);
static void onereq(void);
static void oneseg(void);
static void onestack(void);
static void onestart(void);
static void onestradr(void);
static void weakextern(int lazy);


// Section type dispatch table

static void (*dspone[])(void) =
{   oneendmod,			// SC_EOF     = 0  End of module
    badsec,				// SC_INDEX   = 1  Library index section
    onestart,			// SC_START   = 2  Start of module section
    oneseg,				// SC_SEG     = 3  Segment definition section
    onemsect,			// SC_MSECT   = 4  Msect definition section
    onepsect,			// SC_PSECT   = 5  Psect definition section
    onedata,			// SC_DATA    = 6  Data section
    oneintern,			// SC_INTERN  = 7  Internal symbol definition section
    onelocal,			// SC_LOCAL   = 8  Local symbol definition section
    oneexport,			// SC_EXPORT  = 9  Exported symbol definition section
    onestradr,			// SC_STRADR  = 10 Starting address section
    onedebug,			// SC_DEBUG   = 11 Debugger address section
    onestack,			// SC_STACK   = 12 Initial stack address section
    onereq,				// SC_FILEREQ = 13 File request section
    oneextern,			// SC_EXTERN  = 14 Extern symbol definition section
    badsec,				// SC_SHARED  = 15 Shared library definition section
    oneimport,			// SC_IMPORT  = 16 Imported symbol definition section
    badsec,				//            = 17 Illegal
    badsec,				//            = 18 Illegal
    badsec,				//            = 19 Illegal
    oneendmod,			// SC_ENTRY   = 20 Entry list section
    badsec,				// SC_BAD     = 21 Illegal MS section type
    ms1mtheadr,			// SC_MTHEADR = 22 (0x80) Module header
    ms1mcoment,			// SC_MCOMENT = 23 (0x88) Comment
    ms1mmodend,			// SC_MMODEND = 24 (0x8A, 0x8B) Module end
    ms1mextdef,			// SC_MEXTDEF = 25 (0x8C) External definition
    ms1mtypdef,			// SC_MTYPDEF = 26 (0x8E) Type definition
    ms1mpubdef,			// SC_MPUBDEF = 27 (0x90, 0x91) Public definition
    ms1mlocsym,			// SC_MLOCSYM = 28 (0x92, 0x93) Local symbols
    ms1mlinnum,			// SC_MLINNUM = 29 (0x94, 0x95) Source line number
    ms1mlnames,			// SC_MLNAMES = 30 (0x96) Name list
    ms1msegdef,			// SC_MSEGDEF = 31 (0x98, 0x99) Segment definition
    ms1mgrpdef,			// SC_MGRPDEF = 32 (0x9A) Group definition
    (void (*)(void))nullfunc,
						// SC_MFIXUPP = 33 (0x9C, 0x9D) Fix up previous data
						//                        image
    ms1mledata,			// SC_MLEDATA = 34 (0xA0, 0xA1) Logical data image
    ms1mlidata,			// SC_MLIDATA = 35 (0xA2, 0xA3) Logical repeated
						//		                  (iterated) data image
    ms1mcomdef,			// SC_MCOMDEF = 36 (0xB0) Communal names definition
    ms1mextdef,			// SC_MLOCEXD = 37 (0xB4) External definition visible
						//                        within module only
    ms1mflcexd,			// SC_MFLCEXD = 38 (0xB5) Func ext definition visible
						//                        within module only
    ms1mpubdef,			// SC_MLOCPUB = 39 (0xB6, 0xB7) Public definition
						//                        visible within module only
    ms1mcomdef,			// SC_MLOCCOM = 40 (0xB8) Communal name visible
						//                        within module only
    ms1mcomdat			// SC_MCOMDAT = 41 (0xC2, 0xC3) Initialized communal
};						//						  data

// Polish item type dispatch table

static void nullfunc1(int i)
{
	(void)i;
}

void (*oneddsp[])(int) =
{   (void (*)(int))nullfunc1,	//              =10000xxx - Arithmetic operators
    (void (*)(int))nullfunc1,	//              =10001xxx - Arithmetic operators
    onepush,					//              =10010xxx - Push operators
    (void (*)(int))nullfunc1,	//              =10011xxx - Store operators
    onedsvl,					// PO_PVAL      =10100xxx - Push absolute value
    onedsvl,					// PO_PVALREL   =10101xxx - Push relative
								//                          absolute value
    onedsvl,					// PO_POFSCP    =10110xxx - Push relocated value
								//                          for current psect
    onedsvl,					//              =10111xxx - Push relocated
								//		relative value for current psect
    onedsvn,					// PO_POFSAP    =11000xxx - Push relocated value
								//                          for any psect
    onedsvn,					// PO_POFSRELAP =11001xxx - Push relocated
								//		                    relative value for
								//                          any psect
    onedsvs,					//              =11010xxx - Push external value
    onedsvs,					//              =11011xxx - Push external
								//                          relative value,
								//                          symbol name follows
    onedsvl,					//              =11100xxx - Push external value,
								//                          previous symbol
    onedsvl,					//              =11101xxx - Push external
								//                          relative value,
								//                          previous symbol
    onedsvn,					// PO_POFSSYM   =11110xxx - Push external value,
								//                          symbol number
								//                          follows
    onedsvn						// PO_POFSRELSYM=11111xxx - Push external
};								//                          relative value,
								//                          symbol number
								//                          follows

// Push operators dispatch table

void (*onepdsp[])(void) =
{   (void (*)(void))onesym,		// 10010000 - Push segment selector for external
								//            symbol, symbol name follows
    (void (*)(void))nullfunc,	// 10010001 - Push segment selector for external
								//            symbol, previous symbol
    (void (*)(void))nullfunc,	// 10010010 - Push segment selector for currect
								//            psect
    (void (*)(void))getnumber,	// 10010011 - Push segment selector for any
								//            psect
    (void (*)(void))getnumber,	// 10010100 - Push segment selector for any
								//            msect
    (void (*)(void))getnumber,	// 10010101 - Push segment selector for any
								//            segment
    (void (*)(void))getnumber,	// 10010110 - Push offset for any msect
    (void (*)(void))getnumber	// 10010111 - Push segment selector for external
};								//            symbol, symbol number follows


static MD  *assocmsseg(char *npnt, int msflags);
static PD  *declpsect(MD *msd, int flags, long addr, int mod, long tsize,
		long lsize);
static SY  *getsymbymsnum();


//*******************************************************
// Function: fileone - Process single file for first pass
// Returned: Nothing
//*******************************************************

#if CFG_XLINK_MULTIPASS
 extern int	g_nPass1Lev;
#endif

void fileone(
	OB *obj)
{
	int    tcnt;
	uchar *tp1;
	uchar *tp2;

#if CFG_XLINK_MULTIPASS
	if (g_nPass1Lev > 0 &&
			strnicmp( obj->obj_fsp, "XOSLIB:\\XOS\\LIB", 15) != 0)
		return;
	fprintf(stdout, "->fileone(%3d) obj_mdl=%08X obj_fsp=[%.40s]\n",
			g_nPass1Lev, (int)obj->obj_mdl, obj->obj_fsp);

#endif
	opnobj (obj);						// Open the OBJ file
	if (!libflag)						// Library file?
	{			 						// No - just one module
		curmod = obj->obj_mdl = (MB *)getspace(sizeof(MB));
		curmod->mb_magic = 'DOM*';
		curmod->mb_obj = obj;
		curmod->mb_name = "";
		moduleone();
	}
	else
	{
		// Processing a library file

		setobj();						// Setup to read the index module
		libdone = FALSE;
		svdblk = objblk;

		// Initialize module list add pointer

		modlst = &obj->obj_mdl;

#if CFG_XLINK_MULTIPASS

		while (modlst->mb_next != 0 && modlst->mb_next != (MD *)(-1))
		{
			fprintf(stdout, " ... traverse modlst %08X <- %08X\n",
					(int)modlst, (int)modlst->mb_next);
			modlst = modlst->mb_next;
		}
#endif

		if (auxloc == 0)				// Do we have a library index buffer?
			auxloc = (uchar *)getspace((long)512); // No - get one now

		tcnt = objcnt;					// Copy remaining part of buffer
		tp1 = objpnt;
		objpnt = tp2 = auxloc + (int)(objpnt - objbuf);

		while (--tcnt >= 0)
			* tp2++ = * tp1++;

		objbuf = auxloc;				// Use library index buffer now
		do
		{
			switch (getsec())			// Get next section     
		    {
			 case SC_EOM:
				libdone = TRUE;
				break;

			 case SC_INDEX:
				onelibent();			// Process library entry section
				break;

			 default:
				badsec();
				break;
			}
		} while (!libdone);

///		if (curobj->obj_mdl == 0)		// Did we find anything to load?
///		    curobj->obj_mdl = (MB *)-1; // No - remember this!

	}
	closeobj();							// Close the input file
}


//*******************************************************************
// Function: onelibent - Process library entry section for first pass
// Returned: Nothing
//*******************************************************************

static void onelibent(void)

{
	SY 	*sym;
	long modpnt;

	modpnt = getlong();					// Get pointer to module in file
	while (seccnt != 0)
    {
		getsym();						// Get symbol
		symbuf[0] = SYM_SYM;

		if ((sym=looksym()) != NULL && (sym->sy_flag & SYF_UNDEF))
		{
			*modlst = curmod = (MB *)getspace(sizeof(MB));
										// Add to used module list
			curmod->mb_magic = 'DOM*';
			curmod->mb_obj = curobj;
			curmod->mb_next = NULL;
			curmod->mb_mod = modpnt;
			curmod->mb_name = "";
			curmod->mb_next = 0;
			modlst = &curmod->mb_next;
		    libpnt = objpnt;			// Save our state
		    libcnt = objcnt;
		    libblk = objblk;
			libamnt = objamnt;
		    libsec = seccnt;
		    liblow = lowfirst;
		    objblk = svdblk;
		    objbuf = objloc;
			setblk(modpnt);				// Setup to read module
		    moduleone();				// Load the module
		    objpnt = libpnt;			// Restore input state
		    objcnt = libcnt;
		    objblk = libblk;
			objamnt = libamnt;
		    objbuf = auxloc;
			modtype = OT_XOS;
			seccnt = libsec;
			lowfirst = liblow;
		    break;
		}
	}
}


//*******************************************************************
// Function: moduleone - Process single program module for first pass
// Returned: Nothing
//*******************************************************************

void moduleone(void)

{
	PD **pnt;
	PD  *psd;

    setobj();
    if (symnummem.mab_mempnt == SYMNUMBASE) {
	sbrkx(0, &symnummem);
    }
    if (lnamesmem.mab_mempnt == LNAMESBASE) {
	sbrkx(0, &lnamesmem);
    }
    if (segnummem.mab_mempnt == SEGNUMBASE) {
	sbrkx(0, &segnummem);
    }
    if (msectnummem.mab_mempnt == MSECTNUMBASE) {
	sbrkx(0, &msectnummem);
    }
    if (psectnummem.mab_mempnt == PSECTNUMBASE) {
	sbrkx(0, &psectnummem);
    }
    curmod->mb_symnumtbl = symnumbase = (SY **)symnummem.mab_mempnt;
    curmod->mb_symnummax = 0;
    curmod->mb_lnamestbl = (char **)lnamesmem.mab_mempnt;
    curmod->mb_lnamesmax = 0;
    curmod->mb_segnumtbl = (SD **)segnummem.mab_mempnt;
    curmod->mb_segnummax = 0;
    curmod->mb_msectnumtbl = (MD **)msectnummem.mab_mempnt;
    curmod->mb_msectnummax = 0;
    curmod->mb_psectnumtbl = (PD **)psectnummem.mab_mempnt;

	// Create the abs PD for this module

	psd = (PD *)getspace(sizeof(PD));
	memset(psd, 0, sizeof(PD));
	psd->pd_magic = 'DSP*';
	psd->pd_modblk = curmod;
	psd->pd_loclast = &psd->pd_lochead;
	if (abspsd.pd_filetail)			// Link into list for psect
		abspsd.pd_filetail->pd_next = psd;
	else
		abspsd.pd_filehead = psd;
	abspsd.pd_filetail = psd;
	if ((pnt = (PD **)sbrkx(sizeof(PD *), &psectnummem)) == NULL)
		fail("Cannot allocate memory for psect number table");
	*pnt = psd;						// Store pointer in local table
	(curmod->mb_psectnummax)++;
    curmod->mb_psectnummax = 0;

	// Process the module

    done = FALSE;
    do
    {
        getsec();

///		printf("### type: %02X size:%d pos %X\n", sectype, seccnt,
///				((long)objblk) * 512 + (long)(objpnt-objbuf));

        if( debugflag )
            printf("p1: sec=0x%02.2X, size=%d\n", sectypx, seccnt);
        (*dspone[sectype])();			// Dispatch on section type
    } while (!done);
	modulenum++;
}


//**********************************************************
// Function: oneendmod - Update offset for all psects at end
//	of OBJ module during pass one
// Returned: Nothing
//**********************************************************

static void oneendmod(void)
{
    SD *sgd;
    SD *lnksgd;

	if( (sgd = sgdhead) != NULL )		// Scan all segments (if any)
	{
        do
        {
            if (sgd->sd_lnknum != 0)
            {
                if (sgd->sd_lnknum > curmod->mb_segnummax)
                {
                    char buf[80];

                    sprintf(buf, "Illegal segment number %d for linked segment",
                            sgd->sd_lnknum);
                    fail(buf);
                }
                lnksgd = curmod->mb_segnumtbl[(int)sgd->sd_lnknum-1];
                if (sgd->sd_linked != NULL &&
                        sgd->sd_linked != lnksgd)
                    confllink(sgd->sd_sym);
                else
                    sgd->sd_linked = lnksgd;
                sgd->sd_lnknum = 0;
            }
        } while ((sgd=sgd->sd_next) != NULL);
    }
    modname[0] = 0;
    done = TRUE;						// Indicate finished with file
}


//**************************************************
// Function: onestart - Process module start section
// Returned: Nothing
//**************************************************

static void onestart(void)

{
	int len;

    getsym();							// Get module name

    len = strmov(modname, symbuf + 1) - modname; // Save module name
	curmod->mb_name = getspace(len + 1);
	strcpy(curmod->mb_name, modname);
    if (debugflag)
        printf("    mod=%s\n", modname);
    if (opsymsz)						// Loading debugger symbols?
        resvsym();						// Reserve symbol table space
}


//******************************************************
// Function: oneseg - Process segment definition section
// Returned: Nothing
//******************************************************

static void oneseg(void)
{
    long  addr;
    ulong linked;
    int   select;
	int   flags;
    uchar flag1;
    uchar flag2;
    uchar type;
    uchar priv;

    while (seccnt != 0)
    {
		flag1 = getbyte();				// Get first flag byte
        select = getitem(flag1);		// Get segment selector
        flag2 = getbyte();				// Get second flag byte
        addr = getitem(flag2);			// Get address or modulus
        linked = (flag2 & SF2_LINKED)? getnumber(): 0;
										// Get number of linked segment
        type = getbyte();				// Get segment type
        if (type > 4)					// Check for valid type
        {
            char buf[50];

            sprintf(buf, "Illegal segment type %d", type);
            fail(buf);
        }
        priv = getbyte();				// Get privilege level
        getsym();						// Get name of segment
		flags = flag1 & (SA_LARGE|SA_32BIT|SA_CONF|SA_READ|SA_WRITE);
		if (flag2 & SF2_EXTEND)
			flags |= SA_EXTEND;
		if (flag2 & SF2_ADDR)
			flags |= SA_ADDR;
		declseg(type, flags, linked, priv, addr, select);
	}
}


//******************************************************
// Function: onemsect - Process msect definition section
// Returned: Nothing
//******************************************************

static void onemsect(void)
{
    ulong max;
    ulong addr;
    uint  segnum;
	int   flags;
    uchar flag1;
    uchar flag2;
    uchar type;
    uchar priv;

    while (seccnt != 0)
    {
		flag1 = getbyte();				// Get first flag byte
        max = getitem(flag1);			// Get maximum space value
        flag2 = getbyte();				// Get second flag byte
        addr = getitem(flag2);			// Get address or modulus
        type = getbyte();				// Get msect type
        if (type > 4)					// Check for valid type
            fail("Illegal msect type");
        priv = getbyte();				// Get privledge level

        segnum = getbyte();				// Get segment number
        getsym();						// Get name of msect
		flags = flag1 & (MA_CONF|MA_READ|MA_WRITE);
		if (flag2 & MF2_ADDR)
			flags |= MA_ADDR;
		declmsect(getsgd(segnum), 0, flags, max, priv, addr);
	}
}


//******************************************************
// Function: onepsect - Process psect definition section
// Returned: Nothing
//******************************************************

static void onepsect(void)
{
    long  tsize;
    long  lsize;
    long  addr;
    int   msn;
	int   flags;
	int   mod;
    uchar flag1;
    uchar flag3;

    while (seccnt != 0)
    {
		flag1 = getbyte();				// Get first flag byte
        tsize = getitem(flag1);			// Get total size of psect
        lsize = getitem(getbyte());		// Get loaded size of psect
        flag3 = getbyte();				// Get third byte
        addr = getitem(flag3);			// Get address or modulus
		mod = getitem(flag3 >> 4);		// Get address
		if ((flag3 & PF3_ADDR) == 0 && addr != 0) // Is the address value really
		{										  //   the address
			mod = addr;					// No - its the modulus
			addr = 0;
		}
        msn = getbyte();				// Get msect number
        if (msn == 0 || msn > curmod->mb_msectnummax) // Is it valid?
            fail("Illegal msect number"); // Yes - fail
		getsym();						// Get name of psect
		flags = 0;
		if (flag1 & PF1_OVER)
			flags |= PA_OVER;
		if (flag3 & PF3_ADDR)
			flags |= PA_ADDR;
        declpsect(curmod->mb_msectnumtbl[msn-1], flags, addr, mod, tsize,
				lsize);
	}
}


//*****************************************
// Function: onedata - Process data section
// Returned: Nothing
//*****************************************

static void onedata(void)
{
    int header;

    while (seccnt != 0)
    {
		if ((header=getbyte()) & 0x80)	// Get item header byte
			(*oneddsp[(header>>3)&0x0F])(header); // If polish item
		else
		{
			if (header >= 0x78)			// Is this an address?
                onedsvn(header);		// Yes - skip the address value
            else
                skpbyte(header);		// No - skip the data
		}
    }
}


//*******************************************
// Function: onepush - Process push operators
// Returned: Nothing
//*******************************************

static void onepush(
    int header)
{
    (*onepdsp[header&7])();
}


//*****************************************
// Function: onedsvl - Skip following value
// Returned: Nothing
//*****************************************

static void onedsvl(
    int header)
{
    if ((header &= 3) != 0)
    {
        if (header == 3)
            ++header;
        skpbyte(header);
    }
}


//*********************************************************
// Function: onedsvs - Skip following value and process the
//	external symbol reference
// Returned: NOthing
//*********************************************************

static void onedsvs(
    int header)
{
    onedsvl(header);					// Skip value
    onesym();							// Process the symbol
}


//***********************************************************
// Function: onedsvn - Skip following value and the following
//	symbol or psect number
// Returned: Nothing
//***********************************************************

static void onedsvn(
    int header)
{
    onedsvl(header);				// Skip value
    getnumber();					// Skip symbol number
}


//*****************************************************
// Function: onesym - Process external symbol reference
// Returned: Pointer to symbol table entry
//*****************************************************

SY *onesym(void)
{
	SY *sym;

	getsym();						// Get symbol

	symbuf[0] = SYM_SYM;
	if ((sym=looksym()) == NULL)	// Is it in the symbol table now?
	{
		sym = entersym();			// No - put it in now
		++symtblsize;
		sym->sy_flag = SYF_UNDEF;	// Indicate undefined
		++undefcnt;					// Count it
	}
	return (sym);
}


//*********************************************************************
// Function: oneintern - Process internal symbol definition (SC_INTERN)
// Returned: Nothing
//*********************************************************************

static void oneintern(void)
{
    long value;
    int  header;
    int  psnum;
	int  select;

    while (seccnt != 0)
    {
		header = getbyte();				// Get header byte
		value = getitem(header);		// Get value of symbol
        if (header & SYF_ADDR)			// Address?
		{
			if (header & SYF_ABS)		// Yes - get psect number or selector
			{							//   value
				select = getword();
				psnum = 0;
			}
			else
			{
				select = 0;
				psnum = getnumber();
			}
		}
        else							// Not address - if not indicated as
        {								//   ABS eat the extra byte which old
            if (!(header & SYF_ABS))	//   versions of XMAC inserted!
                getbyte();
            psnum = 0;
        }
		if (psnum == 0)
                header |= SYF_ABS;
		getsym();						// Get symbol name
        definternal(TRUE, header, getpsd(psnum), value, select);
    }									// Define the internal symbol
}


//*************************************************************
// Function: onelocal - Process local symbol definition section
// Returned: Nothing
//*************************************************************

static void onelocal(void)
{
	long value;
    int  header;
	int  select;
	int  psnum;

    if (opsymsz || needsym)				// Loading debugger symbols?
    {
		while (seccnt != 0)				// Yes
		{
            header = getbyte();
            value = getitem(header);
            if (header & SYF_ADDR)		// Address?
            {
                if (header & SYF_ABS)	// Yes - get psect number or selector
				{
                    select = getword();
					psnum = 0;
				}
                else
				{
					select = 0;
					psnum = getnumber();
				}
            }
            else						 // Not address - if not indicated as
            {							 //   ABS eat the extra byte which
                if (!(header & SYF_ABS)) //   old versions of XMAC inserted!
                    getbyte();
				psnum = 0;
				select = 0;
			}
		    getsym();					// Get symbol name
			if (needsym || opsymsz)		// Loading debugger symbols?
			{
	            if (opsymsz)	
	                resvsym();			// Reserve debugger symbol space
				deflocal(header, getpsd(psnum), value, select);
			}
		}
    }
}


//*****************************************************************
// Function: oneexport - Process exported symbol definition section
// Returned: Nothing
//*****************************************************************

static void oneexport(void)
{
    SY  *sym;
    long value;
    int  header;
    int  psnum;
	int  select;

    while (seccnt != 0)
    {
		header = getbyte();				// Get header byte
		value = getitem(header);		// Get value of symbol
        if (header & SYF_ADDR)			// Address?
		{
			if (header & SYF_ABS)		// Yes - get psect number or selector
			{							//   value
				select = getword();
				psnum = 0;
			}
			else
			{
				select = 0;
				psnum = getnumber();
			}
		}
        else							// Not address - if not indicated as
        {								//   ABS eat the extra byte which old
            if (!(header & SYF_ABS))	//   versions of XMAC inserted!
                getbyte();
            psnum = 0;
        }
		if (psnum == 0)
			header |= SYF_ABS;
		getsym();						// Get symbol name
        sym = definternal(TRUE, header, getpsd(psnum), value, select);
										// Define the symbol
        sym->sy_flag |= SYF_EXPORT;		// Indicate exported
        if (exporttail == NULL)			// Put it in the exported list
            exporthead = sym;
        else
            exporttail->sy_export = sym;
        exporttail = sym;
		sym->sy_export = NULL;
        ++exportnum;
    }
}


//****************************************************
// Function: onestradr - Process start address section
// Returned: Nothing
//****************************************************

static void onestradr(void)
{
	int psnum;

    if (strpsd == NULL)
    {
        stradr = getlong();				// Get starting address
		if ((psnum = getnumber()) == 0)	// Get psect number
        {
            absaddr("starting");		// Can't be absolute
            strpsd = NULL;
        }
        strpsd = getpsd(psnum);			// Get its psect
    }
	else
		multstr = TRUE;
}


//******************************************************
// Function: onedebug - Process debugger address section
// Returned: Nothing
//******************************************************

static void onedebug(void)

{
	int psnum;

    if (dbgpsd == NULL)
    {
        dbgadr = getlong();				// Get starting address
		if ((psnum = getnumber()) == 0)	// Get psect number
        {
            absaddr("debugger");		// Can't be absolute
            strpsd = NULL;
        }
        dbgpsd = getpsd(psnum);			// Get its psect
    }
	else
		multdbg = TRUE;
}


//***********************************************************
// Function: onestack - Process initial stack address section
// Returned: Nothing
//***********************************************************

static void onestack(void)
{
	int psnum;

    if (stkpsd == NULL)
    {
        stkadr = getlong();				// Get starting address
		if ((psnum = getnumber()) == 0)	// Get psect number
        {
            absaddr("starting");		// Can't be absolute
            stkpsd = NULL;
        }
        stkpsd = getpsd(psnum);			// Get its psect
    }
	else
		multstk = TRUE;
}


//************************************************
// Function: onereq - Process file request section
// Returned: Nothing
//************************************************

static void onereq(void)
{
    OB   *file;
    char *pnt;
    char  chr;
    char  haveext;

    file = (OB *)getspace(offsetof(OB, obj_fsp) + seccnt + 6);
    pnt = file->obj_fsp;				// Get space for the name
    haveext = FALSE;
    while (seccnt > 0)					// Read the name
    {
         chr = getbyte();
         if (chr == '.')
             haveext = TRUE;
        *pnt++ = chr;
    }
    if (!haveext)						// Add default extension if none
        strcpy(pnt, inpext);			//   given
    else
        *pnt = '\0';
    *prevobj = file;					// Link in the block
    file->obj_next = NULL;
    prevobj = &file->obj_next;
}


//****************************************************************
// Function: onextern - Process external symbol definition section
// Returned: Nothing
//****************************************************************

static void oneextern(void)
{
    SY  *sym;
    SY **pnt;

    while (seccnt > 0)
    {
        sym = onesym();
        if ((pnt = (SY **)sbrkx(sizeof(SY *), &symnummem))
                == NULL)
            fail("Cannot allocate memory for symbol number table");
        *pnt = sym;
        (curmod->mb_symnummax)++;
    }
}


//****************************************************************
// Function: onimport - Process imported symbol definition section
// Returned: Nothing
//****************************************************************

static void oneimport(void)
{
    SY  *sym;
    SY **pnt;

    while (seccnt > 0)
    {
        getbyte();						// Discard the flag byte
        sym = onesym();
        if (sym->sy_flag & SYF_UNDEF)	// Was it already in the table?
        {								// No
            sym->sy_flag |= SYF_IMPORT;
            sym->sy_flag &= ~SYF_UNDEF;
			--undefcnt;					// Count it
            *importlast = sym;
            sym->sy_next = NULL;
            importlast = &sym->sy_next;
            sym->sy_val.v = 0;
        }
        else if (!(sym->sy_flag & SYF_IMPORT))
			impaldef(sym);
        if ((pnt = (SY **)sbrkx(sizeof(SY *), &symnummem)) == NULL)
            fail("Cannot allocate memory for symbol number table");
        *pnt = sym;
        (curmod->mb_symnummax)++;
    }
}


//**********************************************************
// Function: ms1mtheadr - Process MS module header (MTHEADR)
// Returned: Nothing
//**********************************************************

static void ms1mtheadr(void)
{
	int len;

    getmssym();							// Get module name
	if (modname[0] == 0)
	{
	    len = strmov(modname, symbuf + 1) - modname; // Save module name
		curmod->mb_name = getspace(len + 1);
		strcpy(curmod->mb_name, modname);
    	if (debugflag)
        	printf("    mod=%s\n", modname);
    	if (opsymsz)					// Loading debugger symbols?
        	resvsym();					// Reserve symbol table space
	}
}


//****************************************************
// Function: ms1mcoment - Process MS comment (MCOMENT)
// Returned: Nothing
//****************************************************

static void ms1mcoment(void)
{
	getbyte();							// Discard the comment type
	switch (getbyte())					// Dispatch on the comment class
	{
	 case 0xA0:							// OMF extensions
		break;

	 case 0xA8:							// Weak extern record
		weakextern(FALSE);
		haveweak = TRUE;
		break;

	 case 0xA9:							// Laxy Extern record
		weakextern(TRUE);
		havelazy = TRUE;
		break;
	}
}


static void weakextern(
	int lazy)
{
	SY *sym;

	sym = getsymbymsnum();

///	printf("### %s sym: %s\n", (lazy) ? "lazy" : "weak", sym->sy_name + 1);

	sym->sy_weak = getsymbymsnum();

///	printf("### %s ref: %s\n", (lazy) ? "lazy" : "weak",
///			sym->sy_weak->sy_name + 1);

	if (lazy)
		sym->sy_flag |= SYF_LAZY;
}


static SY *getsymbymsnum()

{
	int symn;

    symn = getmsnum();
    if (symn > curmod->mb_symnummax)
    {
        char buf[60];

        sprintf(buf, "Illegal symbol number %d", symn);
        fail(buf);
    }
    return (curmod->mb_symnumtbl[symn-1]);
}


//******************************************************************
// Function: ms1mmodend - Process MS module end (MMODEND or M386END)
// Returned: Nothing
//******************************************************************

static void ms1mmodend(void)

{
    PD  **msgpnt;
    MD   *msd;
    PD   *psd;
    MSGB *msgb;
    int   cnt;

	char *npnt;
	SD   *sgd;


	// If any MS groups were defined, we must change them into msects.
	//   Note that if there is an existing msect or segment with the
	//   right name, it will be used.

	if (msgbhead)
	{
		do
		{
			msgb = msgbhead;
			if ((cnt = msgb->msgb_size) != 0)
			{
				msgpnt = msgb->msgb_segs;
				psd = *msgpnt;
				if ((msd = psd->pd_msd) == NULL)
					msd = assocmsseg(msgb->msgb_sym->sy_name+1, psd->pd_flag);
				while (--cnt >= 0)
				{
					psd = *msgpnt++;
                    if (psd->pd_msd == NULL)
                    {
                        // CHECK FOR ATTRIBUTE CONFLICTS HERE!

                        psd->pd_msd = msd;

						psd = psd->pd_filepd;
						psd->pd_msd = msd;

                        if (msd->md_tail) // Link into psect list
                            msd->md_tail->pd_next = psd;
                        else
                            msd->md_head = psd;
                        msd->md_tail = psd;
                        psd->pd_next = NULL;
                        msd->md_psn++;
                    }
                }
            }
            else						// If have empty group, just create
            {							//   a segment and an msect for it
                npnt = strmov(symbuf+1, msgb->msgb_sym->sy_name+1);
                npnt[-1] = 's';
                symbuf[0] = SYM_SEG;
                symsize = (int)(npnt - symbuf);
				sgd = declseg(0, 0, 0, 0, 0xFFFFFFFF, 0);
				npnt[-1] = 'm';
				symbuf[0] = SYM_MSCT;
				declmsect(sgd, 0, 0, 0, 0, 0);
            }
            msgbhead = msgb->msgb_next;
            msgb->msgb_next = (MSGB *)(-1L);
        } while (msgbhead != NULL);
    }
    msgbhead = msgbtail = NULL;

    // If we have any MS segments (which are really psects!) which are
    //   not part of a group, we must create msects and real segments for
    //   them.  Note that if there is an existing msect or segment with
    //   the right name, it will be used.

    cnt = (int)(curmod->mb_psectnummax);
    msgpnt = curmod->mb_psectnumtbl + 1;
    while (--cnt >= 0)
    {
        psd = *msgpnt++;
        if (psd->pd_msd == NULL)
        {
            msd = assocmsseg(psd->pd_clsname, psd->pd_flag);
            psd->pd_msd = msd;

			psd = psd->pd_filepd;
			psd->pd_msd = msd;
            if (msd->md_tail) 			// Link into psect list
                msd->md_tail->pd_next = psd;
            else
                msd->md_head = psd;
            msd->md_tail = psd;
            psd->pd_next = NULL;
            msd->md_psn++;

			if (psd->pd_msd == NULL)
				BREAK();

        }
    }
    oneendmod();
}

//********************************************************
// Function: assocmsseg - Associate msect and real segment
//		with MS segment (which is really a psect!)
// Returned: Nothing
//********************************************************

static MD *assocmsseg(
    char *npnt,					// Pointer to base name
	int   msflags)

{
	int flags;

    npnt = strmov(strmov(symbuf+1, npnt), "_m");
	symsize = npnt - symbuf;
    symbuf[0] = SYM_MSCT;
	flags = MA_READ;
	if (msflags & PA_MSWRITE)
		flags |= MA_WRITE;
	return (declmsect(NULL, 0, flags, 0, 0, 0));
}


//****************************************************************
// Function: ms1mextdef - Process MS external definition (MEXTDEF)
// Returned: Nothing
//****************************************************************

static void ms1mextdef(void)

{
    SY  *sym;
    SY **pnt;

    while (seccnt > 1)
    {
        getmssym();						// Get symbol
        getmsnum();						// Get and discard type index
        symbuf[0] = SYM_SYM;

///		printf("### MSEXTDEF: %d %s\n", (sectypx < MSSC_LOCEXD), symbuf + 1);

        if ((sym=(sectypx < MSSC_LOCEXD)? looksym(): looklocsym()) == NULL)
        {								// Is it in the symbol table now?
            sym = entersym();			// No - put it in now
            if (sectypx >= MSSC_LOCEXD)	// Local symbol?
			{
                sym->sy_mod = curmod; 	// Yes - make it local
				sym->sy_flag |= SYF_LOCAL;
			}
            ++symtblsize;
            sym->sy_flag = SYF_UNDEF;	// Indicate undefined
            ++undefcnt;					// Count it
        }
        if ((pnt = (SY **)sbrkx(sizeof(SY *), &symnummem))
                == NULL)
            fail("Cannot allocate memory for symbol number table");
        *pnt = sym;
        (curmod->mb_symnummax)++;
    }
}


//************************************************************
// Function: ms1mtypdef - Process MS type definition (MTYPDEF)
// Returned: Nothing
//************************************************************

static void ms1mtypdef(void)

{
}


//****************************************************
// Function: ms1mpubdef - Process MS public definition
//		 (MPUBDEF and MPUB386)
// Returned: Nothing
//****************************************************

static void ms1mpubdef(void)

{
    PD   *psd;
    ulong value;
    uint  selector = 0;
    uint  header;
    int   basenum;
    int   segnum;

    basenum = (int)getmsnum();			// Get MS base group number
    segnum = (int)getmsnum();			// Get MS "segment" number
	psd = getpsd(segnum);
    if (basenum == 0 && segnum == 0)
    {
        selector = getword();
        header = (selector == 0) ? SYF_ABS : (SYF_ABS|SYF_ADDR);
    }
    else
        header = SYF_ADDR;
    while (seccnt > 1)
    {
        getmssym();						// Get symbol name
        value = (sectypx & 0x01) ? getlong() : getword();
///		if (segnum != 0)				// Relocate if relative
///			value += psd->pd_ofs;
        getmsnum();						// Get and discard type index

///		printf("### MSPUB: %d %s\n", (sectypx < MSSC_LOCPUB), symbuf + 1);

        definternal((sectypx < MSSC_LOCPUB), header, psd, value, selector);
    }									// Define the symbol
}


//*****************************************************************
// Function: ms1mlocsym - Process MS 16-bit local symbols (MLOCSYM)
// Returned: Nothing
//*****************************************************************

static void ms1mlocsym(void)

{
}


//*******************************************************************
// Function: ms1mlinnum - Process MS 16-bit src line number (MLINNUM)
// Returned: Nothing
//*******************************************************************

static void ms1mlinnum(void)

{
    PD  *psd;
	long value;
    int  linenum;

	if ((needsym || opsymsz != 0) && (sectypx == MSSC_LINSYM32 ||
			sectypx == MSSC_LIN386))
	{
		getmsnum();						// Discard group and MS "segment"
		psd = getpsd(getmsnum());		// Get MS "segment" number
		while (seccnt > 1)
		{
			linenum = getword();		// Get line number
			value = (sectypx == MSSC_LINNUM) ? getword() : getlong();
										// Get offset
			symsize = 1 + sprintf(symbuf + 1, "$%d", linenum);
			if (opsymsz)
	            resvsym();
			deflocal(SYF_ADDR, psd, value, 0);
		}
    }
}


//******************************************************
// Function: ms1mlnames - Process MS name list (MLNAMES)
// Returned: Nothing
//******************************************************

static void ms1mlnames(void)

{
    char **pnt;
    char  *name;

    while (seccnt > 1)
    {
        getmssym();						// Get name
        name = getspace(symsize + 2);
        strmov(name + 1, symbuf + 1);
        name[0] = symsize - 1;
        if ((pnt = (char **)sbrkx(sizeof(SY *), &lnamesmem)) == NULL)
            fail("Cannot allocate memory for symbol number table");
        *pnt = name;
        curmod->mb_lnamesmax++;
    }
}


int modtbl[] =
{   0,			// 0 - Absolute segment
    1,			// 1 - Relocatable, byte aligned
    2,			// 2 - Relocatable, word aligned
    16,			// 3 - Relocatable, paragraph aligned
    256,		// 4 - Relocatable, "page" aligned
    4,			// 5 - Relocatable, long aligned
    4096		// 6 - Relocatable, page aligned
};


//**************************************************
// Function: ms1msegdef - Process segment definition
//		 (MSEGDEF and MSEG386)
// Returned: Nothing
//**************************************************

static void ms1msegdef(void)

{
	PD   *psd;
    char *segname;
	char *clsname;
    ulong segsize;
    uint  segnamenum;
    uint  segclassnum;
	int   flags;
    uchar attr;
    uchar combine;
    uchar alignment;

    attr = getbyte();					// Get attribute byte
    if ((combine = (attr >> 2) & 0x07) > 7)
    {
        char buf[60];

        sprintf(buf, "Illegal MS segment combine type %d", combine);
        fail(buf);
    } 
    if ((alignment = attr >> 5) == 0)	// Absolute segment?
        fail("Absolute MS segment (unsupported) specified");
    else if (alignment > 6)
    {
        char buf[60];

        sprintf(buf, "Illegal MS segment alignment type %d", alignment);
        fail(buf);
    }
    segsize = (sectypx == MSSC_SEGDEF) ? getword() : getlong();
    if ((segnamenum = (int)getnumber()) > curmod->mb_lnamesmax)
    {									// Get segment name number
        char buf[60];

        sprintf(buf, "Illegal MS segment name index %d", segnamenum);
        fail(buf);
    }
    segname = curmod->mb_lnamestbl[segnamenum-1];
    if (alignment == 0)
    {
        fail("Fixed MS segment not supported!");
    }
    if ((segclassnum = (int)getnumber()) > curmod->mb_lnamesmax)
    {									// Get segment class number
        char buf[60];

        sprintf(buf, "Illegal MS segment class index %d", segclassnum);
        fail(buf);
    }
    clsname = curmod->mb_lnamestbl[segclassnum-1];

    getmsnum();							// Skip overlay name number

	strmov(strnmov(symbuf+1, segname+1, segname[0]), "_p");
	symsize = segname[0] + 3;

///	printf("### A:%d C:%d N:%s\n", alignment, combine, symbuf + 1);

	flags = (strcmp(curmod->mb_lnamestbl[segclassnum-1]+1,
			"CODE") == 0) ? PA_MSCODE : PA_MSWRITE;
	if (attr & 0x01)					// 32-bit segment?
		flags |= PA_MS32BIT; 			// Yes
	if (combine == 6)
		flags |= PA_OVER;
	if ((attr & 0x02) && sectypx == MSSC_SEGDEF && segsize == 0)
		segsize = 0x10000;
    if (alignment == 0)
    {
        fail("Fixed MS segment not supported!");
    }
	strmov(strnmov(symbuf + 1, segname + 1, segname[0]), "_p");
	symsize = segname[0] + 3;
	flags = (strcmp(curmod->mb_lnamestbl[segclassnum-1]+1,
			"CODE") == 0) ? PA_MSCODE : PA_MSWRITE;
	if (attr & 0x01)					// 32-bit segment?
		flags |= PA_MS32BIT; 			// Yes
	if (combine == 6)
		flags |= PA_OVER;
	if ((attr & 0x02) && sectypx == MSSC_SEGDEF && segsize == 0)
		segsize = 0x10000;

	if (strcmp(symbuf + 1, "$$SYMBOLS_p") == 0 ||
			strcmp(symbuf + 1, "$$TYPES_p") == 0)
		segsize = 0;

////qqqqqqqqqqqqqqqqqqqqqq

///	psd = declpsect(NULL, flags, 0,  (flags & PA_MSCODE) ? -1 :
///			modtbl[alignment], segsize, segsize);

	psd = declpsect(NULL, flags, 0, modtbl[alignment], segsize, segsize);

	psd->pd_clsname = getspace(clsname[0] + 1);
	strncpy(psd->pd_clsname, clsname + 1, clsname[0]);
	psd->pd_clsname[clsname[0]] = 0;
}


//*************************************************************
// Function: ms1mgrpdef - Process MS group definition (MGRPDEF)
// Returned: Nothing
//*************************************************************

static void ms1mgrpdef(void)

{
    PD  **pnt;
    SY   *sym;
    MSGB *msgb;
    PD   *msseg;
    char *name;
    uint  namenum;
    uint  deftype;
    int   cnt;

    if ((namenum = getmsnum()) > curmod->mb_lnamesmax)
    {									// Get group name number
        char buf[60];

        sprintf(buf, "Illegal MS group name index %d", namenum);
        fail(buf);
    }
	if (seccnt < 3)
	{
///		printf("### empty group #%d ignored\n", namenum);
		return;
	}
    name = curmod->mb_lnamestbl[namenum-1];
    strmov(strnmov(symbuf+1, name+1, name[0]), "_g");
    symsize = name[0] + 3;
    symbuf[0] = SYM_MSGRP;				// Indicate MS group name
    if ((sym=looksym()) == NULL)		// Find symbol table entry
    {
        sym = entersym();				// Not there - create one now
        msgb = (MSGB *)getspace(sizeof(MSGB));
		msgb->msgb_magic = 'BGSM';
        sym->sy_val.g = msgb;
        msgb->msgb_next = (MSGB *)(-1L);
        msgb->msgb_sym = sym;
        msgb->msgb_size = 0;
    }
    else
        msgb = sym->sy_val.g;
    if (msgb->msgb_next == (MSGB *)(-1L))
    {
        if (msgbtail)					// Link into MS group list
            msgbtail->msgb_next = msgb;
        else
            msgbhead = msgb;
        msgbtail = msgb;
        msgb->msgb_next = NULL;
    }

    while (seccnt > 2)
    {
        if ((deftype = getbyte()) != 0xFF)
        {
            char buf[60];

            sprintf(buf, "Illegal MS group definition type 0x%02.2X", deftype);
            fail(buf);
        }
		msseg = getpsd(getmsnum());
		pnt = msgb->msgb_segs;
		cnt = msgb->msgb_size;
		while (--cnt >= 0)
			if (*pnt++ == msseg)
				break;
        if (cnt < 0)
        {
            if (msgb->msgb_size >= MSGRPMAX) // Is the group too big?
            {
                char buf[80];

                sprintf(buf, "Too many MS segments for MS group %s",
                        symbuf+1);
                fail(buf);
            }
            msgb->msgb_segs[msgb->msgb_size] = msseg;
            msgb->msgb_size++;
        }
    }
}


//********************************************************
// Function: ms1mledata - Process MS 16-bit data (MLEDATA)
//Returned: Nothing
//********************************************************

static void ms1mledata(void)

{
	curpsd = getpsd(getmsnum());
    offset = (sectypx == MSSC_LEDATA) ? getword() : getlong();
    curpsd->pd_qsize += (offset + seccnt - 1);
}


//*****************************************************************
// Function: ms1mlidata - Process MS 16-bit repeated data (MLIDATA)
// Returned: Nothing
//*****************************************************************

static void ms1mlidata(void)

{
    int temp;

	curpsd = getpsd(getmsnum());
    offset = (sectypx == MSSC_LEDATA) ? getword() : getlong();
    curpsd->pd_qsize += (offset + (temp=lid1block()));
}


//************************************************************
// Function: lid1block - Get length of LIDATA data block field
// Returned: Length of data block field
//************************************************************

// This function consumes a data block and determines how much data it
//   generate when expanded

int lid1block(void)

{
    int reptcnt;
    int blkcnt;
    int bytecnt;
    int size;

    reptcnt = (sectypx == MSSC_LEDATA) ? getword() : getlong();
    blkcnt = getword();
    if (blkcnt == 0)
    {
        size = bytecnt = getbyte();
        while (--bytecnt >= 0)
            getbyte();
    }
    else
    {
        size = 0; 
        while (--blkcnt >= 0)
            size += lid1block();
    }
    return (reptcnt * size);
}


//***********************************************************
// Function: ms1mcomdef - Process MS communal names (MCOMDEF)
// Returned: Nothing
//***********************************************************

static void ms1mcomdef(void)

{
    int size;
    int type;
    int pflag;

    while (seccnt >= 5)
    {
        getmssym();						// Collect the symbol name
        getmsnum();						// Get and discard the type index
        type = getbyte();				// Get the data type
        if (type == 0x61)
        {
            size = getmsxnum() * getmsxnum();
            pflag = PA_OVER|PA_MS32BIT;
        }
        else if (type == 0x62)
        {
            size = getmsxnum();
            pflag = PA_OVER;
        }
        else
        {
            char buf[80];

            sprintf(buf, "Illegal type value (0x02.2X) in MS communal "
                    "definition");
            fail(buf);
        }
        defcommunal((sectypx == MSSC_COMDEF) ? 0 : 0x04, size, pflag, 4,
				symbuf + 1, "DATA_m");
    }
}


//*********************************************************************
// Function: ms1mflcexd - Process MS Loc func ext definitions (MFLCEXD)
// Returned: Nothing
//*********************************************************************

static void ms1mflcexd(void)

{
    fail("Unsupported MS record type FLCEXD");
}


//**********************************************************************
// Function: ms1mcomdef - Process MS initialized communal data (MCOMDAT)
// Returned: Nothing
//**********************************************************************

// This MS record looks for an overlayed psect with a named formed by
//   appending _p to the symbol name in the _TEXT_m or DATA_m msects.
//   If it is not found, it is created.  The symbol is defined to be an
//   internal whose value is the address of the first byte in the psect.
//   The data is ignored during pass one.  It will be loaded during pass
//   two.

static void ms1mcomdat(void)

{
    int   flags;
    int   select;
    int   alloc;
    int   align;
    int   group;
    int   temp;
    int   segnum;
    int   size;
    int   pflag;
    long  dataos;
    MD   *msd;
    PD   *msseg;
    char *name;
    char *pnt;
    char  msname[130];

    static int cdmodtbl[] = {1, 2, 16, 256, 4};

    flags = getbyte();
    select = getbyte();
    alloc = select & 0x0F;
    select >>= 4;
    if ((align = getbyte()) > 5)
        align = 1;
    dataos = (sectypx == MSSC_COMDAT)? getword(): getlong();
    getmsnum();

    // We need to determine the name of the msect to which the psect for the
    //   communal item will belong.  If an explicit MS segment is specified
    //   and that MS segment now belongs to an msect (which means it is part
    //   of an MS group) we use the name of that msect.  If it does not belong
    //   to an msect, we construct the msect name from the name of the psect
    //   by replacing the final _p with _m.  If the allocation is specified as
    //   "far code" or "code32", we use the name _TEXT_m.  If it is specified
    //   as "far data" or "data32", we use the name DATA_m.  Note that we do
    //   not differeniate between near and far items!

    // WARNING: If the MS segment specified here is later added to an MS group,
    //		this may not produce the expected results!  Generally, this
    //		should not be a problem, since MS OBJ files seem to define
    //		groups early in the file.

    pflag = PA_OVER;
    msseg = NULL;
    switch (alloc)
    {
     case 0:
        group = getmsnum();
        segnum = getmsnum();
        if ((group | segnum) == 0)
            fail("No segment specified for MS COMDAT record");    
		msseg = getpsd(segnum);
        if ((msd = msseg->pd_msd) != NULL)
            strmov(msname, msd->md_sym->sy_name+1);
        else
        {
            pnt = strmov(msname, msseg->pd_sym->sy_name+1);
            pnt[-1] = 'm';
        }
        break;

     case 3:
        pflag |= PA_MS32BIT;
     case 1:
        strcpy(msname, "_TEXT_m");
        break;

     case 4:
        pflag |= PA_MS32BIT;
     case 2:
        strcpy(msname, "DATA_m");
        break;

     default:
        fail("Illegal allocation type for MS communal data");
    }

    // Here with the name of the msect to use determined.  Now we must get the
    //   name of the communal item and create or find an existing psect for it.

    temp = getmsnum();					// Get the name of the communal item
    name = curmod->mb_lnamestbl[temp-1]+1;
    size = seccnt - 1 + dataos;
    defcommunal(flags, size, pflag, (int)((align == 0)? ((msseg == NULL)? 4:
            msseg->pd_modulus): cdmodtbl[align - 1]), name, msname);
}


//**********************************************
// Function: defcommunal - Define communal items
// Returned: Nothing
//**********************************************

void defcommunal(
    int   flags,
    int   size,
    int   pflag,
    int   modulus,
    char *symname,
    char *msname)

{
    PD   *psd;
    char *pnt;

    pnt = strmov(strmov(symbuf+1, symname), "_p");
    symsize = pnt - symbuf;
	psd = declpsect(assocmsseg(msname, pflag), pflag, 0,  modulus, size, size);
    pnt = strmov(symbuf+1, symname);	// Define the symbol as an internal
    symsize = pnt - symbuf;
    definternal(!(flags&0x04), SYF_ADDR, psd, 0, 0);
}


//***********************************************
// Function: definternal - Define internal symbol
// Returned: Pointer to symbol table entry
//***********************************************

SY *definternal(
    int   global,
    int   header,
    PD   *psd,
    ulong value,
    uint  selector)

{
    SY *sym;

	if (!global)
		header |= SYF_LOCAL;
    symbuf[0] = SYM_SYM;
    if ((sym = (global) ? looksym() : looklocsym()) == NULL)
    {									// Look for symbol in symbol table
        sym = entersym();				// Put it in if its not there now
        ++symtblsize;
///		if (sectypx >= MSSC_LOCPUB)		// Should it be local?

		if (!global)
            sym->sy_mod = curmod;		// Yes - make it local
        if (opsymsz)					// Loading debugger symbols?
            resvsym();
    }
    else
    {
        if (sym->sy_flag & SYF_UNDEF)	// Is it undefined now?
        {
            --undefcnt;					// Yes - reduce count
            if (opsymsz)				// Loading debugger symbols?
                resvsym();
        }
        else
        {
            if (value != sym->sy_val.v || ((psd->pd_msd != NULL ||
					sym->sy_psd->pd_msd != NULL) && psd != sym->sy_psd))
            {
                if (!(sym->sy_flag & SYF_MULT)) // If mult. defined
                    ++mltcnt;
                sym->sy_flag |= SYF_MULT;
            }
            return (sym);
        }
    }
    if (psd->pd_ssmax < symsize)		// Is this longest symbol name for
										//   this psect?
        psd->pd_ssmax = symsize;		// Yes
    sym->sy_flag = header & (SYF_ABS|SYF_ADDR|SYF_SUP|SYF_LOCAL); // Store flags
    sym->sy_val.v = value;				// Store value
    sym->sy_sel = selector;
    sym->sy_psd = psd;					// Store psect data block address
    if (psd->pd_symtail == NULL)		// Link into psect symbol list
        psd->pd_symhead = sym;
    else
        psd->pd_symtail->sy_next = sym;
    psd->pd_symtail = sym;
    sym->sy_next = NULL;
    return (sym);
}


//*****************************************
// Function: deflocal - Define local symbol
// Returend: Pointer to symbol table entry
//*****************************************

// Local symbols are put in the symbol list for the psect but they are
//   not put in the hash table.  They are only used to generate the
//   debugger symbol table.

static SY *deflocal(
    int   header,
    PD   *psd,
    ulong value,
    uint  selector)

{
	SY *sym;

    sym = (SY *)getspace((long)(symsize + 1 + offsetof(SY, sy_name)));
	sym->sy_magic = 'MYS*';
	sym->sy_mod = curmod;
    sym->sy_export = NULL;
	sym->sy_weak = NULL;
    strcpy(sym->sy_name, symbuf);		// Copy symbol name to entry
    sym->sy_flag = (header & (SYF_ABS|SYF_ADDR|SYF_SUP)) | SYF_LOCAL;
										// Store flags
    sym->sy_val.v = value;				// Store value
    sym->sy_sel = selector;
    sym->sy_psd = psd;					// Store psect data block address
    *psd->pd_loclast = sym;				// Link into psect local symbol list
	psd->pd_loclast = &sym->sy_next;
    sym->sy_next = NULL;
    return (sym);
}


//******************************************************************
// Function: resvsym - Reserve space for debugger symbol table entry
// Returned: Nothing
//******************************************************************

void resvsym(void)

{
    stbsize += (long)((opsymwrd? (symsize & ~1):(symsize - 1)) + opsymsz);
    ++stbnum;							// Increase size of symbol table
}


//************************************
// Function: declseg - Declare segment
// Returned: Nothing
//************************************

static SD *declseg(
	int  type,
	int  flags,
	int  linked,
	int  priv,
	long addr,
	int  select)

{
    SY   *sym;
    SD   *sgd;
    SD  **pnt;

	symbuf[0] = SYM_SEG;				// Indicate segment name
	if ((sym = looksym()) == NULL)		// Find segment symbol table entry
	{
		sym = entersym();				// Not there - create one now
		sym->sy_psd = NULL;
		sym->sy_flag = 0;
		sgd = (SD *)getspace(sizeof(SD));
		sgd->sd_magic = 'DGS*';
		sym->sy_val.s = sgd;			// Make segment data block
		sgd->sd_sym = sym;

		sgd->sd_flag = flags;

		sgd->sd_type = type;
		sgd->sd_priv = priv;
		sgd->sd_lnknum = linked;
		sgd->sd_linked = NULL;
		if (sgd->sd_flag & SA_ADDR)		// Was address specified?
		{
			sgd->sd_addr = addr;		// Yes
			sgd->sd_mod = 0xFFFFFFFFL;
		}
		else
		{
			sgd->sd_addr = 0xFFFFFFFFL; // No - value is modulus
			sgd->sd_mod = addr;
		}
		sgd->sd_select = select;
		if (sgdtail)					// Link into segment list
			sgdtail->sd_next = sgd;
		else
			sgdhead = sgd;
		sgdtail = sgd;
		sgd->sd_next = NULL;
		sgd->sd_head = NULL;			// Initialize msect list for segment
		sgd->sd_tail = NULL;
		sgd->sd_msn = 0;
		sgd->sd_num = ++seggbn;
	}
	else								// If this segment is already defined
	{									// Check for conflicts and complain
		sgd = sym->sy_val.s;			//   if find any
		if ((sgd->sd_flag ^ flags) & SA_LARGE)
			conflatr("large/small", "segment", sgd->sd_sym);
		if ((sgd->sd_flag ^ flags) & SA_32BIT)
			conflatr("32-bit/16-bit", "segment", sgd->sd_sym);
		if ((sgd->sd_flag ^ flags) & SA_READ)
			conflatr("readable", "segment", sgd->sd_sym);
		if ((sgd->sd_flag ^ flags) & SA_WRITE)
			conflatr("writable", "segment", sgd->sd_sym);
		if ((sgd->sd_flag ^ flags) & SA_EXTEND)
			conflatr("extendable", "segment", sgd->sd_sym);
		if (type != 0)
		{
			if (sgd->sd_type != 0)
			{
				if (sgd->sd_type != type)
					conflitsegtype(type, sgd->sd_type, sgd->sd_sym);
			}
			else
				sgd->sd_type = type;
		}
		if (sgd->sd_priv != priv)
			conflitm("segment", "privilege level", sgd->sd_sym);
		if (sgd->sd_select != 0)
		{
			if (select != 0 && sgd->sd_select != select)
				conflsel(sgd->sd_sym);
		}
		else
			sgd->sd_select = select;
		if (addr != 0xFFFFFFFFL && (((sgd->sd_flag ^ flags) & SA_ADDR) ||
				((flags & SA_ADDR) ? sgd->sd_addr : sgd->sd_mod) != addr))
			confladdr("segment", sgd->sd_sym, sgd->sd_addr, addr);
		sgd->sd_lnknum = linked;
	}
	if ((pnt = (SD **)sbrkx(sizeof(SD *), &segnummem)) == NULL)
		fail("Cannot allocate memory for segment number table");
	*pnt = sgd;							// Store pointer in local table
	curmod->mb_segnummax++;
	return (sgd);
}


//***************************************
// Function: declmsect = Declare an msect
// Returned: Nothing
//***************************************


static MD *declmsect(
	SD   *sgd,
	int   type,					// Not current used, always 0
	int   flags,
	int   max,
	int   priv,
	long  addr)

{
    MD **pnt;
	SY  *sym;
	MD  *msd;

	symbuf[0] = SYM_MSCT;				// Indicate msect name
	if ((sym=looksym()) == NULL)		// Find msect symbol table entry
	{
		sym = entersym();				// Create one now
		sym->sy_psd = NULL;
		sym->sy_flag = 0;
		if (sgd == NULL)
		{
			strcpy(symbuf + 1, (type == 2) ? "CODE_s" : "DATA_s");
			symsize = 7;
			symbuf[0] = SYM_SEG;
			sgd = declseg(type, (type == 2) ? (SA_READ | SA_32BIT) :
					(SA_READ | SA_WRITE | SA_32BIT), FALSE, 0, 0xFFFFFFFF, 0);
		}
		msd = (MD *)getspace(sizeof(MD));
		memset(msd, 0, sizeof(MD));
		msd->md_magic = 'DSM*';
		sym->sy_val.m = msd;			// Make msect data block
		msd->md_sym = sym;
		msd->md_sgd = sgd;
		msd->md_flag = flags;
		msd->md_type = type;
		msd->md_priv = priv;
		msd->md_max = max;
		if (msd->md_flag & MA_ADDR)		// Was address specified?
		{
			msd->md_addr = addr;		// Yes
			msd->md_mod = 0;
		}
		else
		{
			msd->md_addr = 0;			// No - value is modulus
			msd->md_mod = addr;
		}
		msd->md_tsize = 0;
		msd->md_lsize = 0;
		msd->md_rdhead = NULL;
		msd->md_rdlast = &msd->md_rdhead;
		msd->md_rdoffset = 0;
		msd->md_rdcnt = 0;
		if (sgd->sd_tail)				// Link into msect list
			sgd->sd_tail->md_next = msd;
		else
			sgd->sd_head = msd;
		sgd->sd_tail = msd;
		sgd->sd_msn++;
		msd->md_next = NULL;
		msd->md_head = NULL;			// Initialize psect list for msect
		msd->md_tail = NULL;
		mscgbn++;						// Count the msect
	}
	else								// If this msect is already defined
	{
		msd = sym->sy_val.m;
		if ((msd->md_flag ^ flags) & MA_READ)
			conflatr("readable", "msect", msd->md_sym);
		if ((msd->md_flag ^ flags) & MA_WRITE)
			conflatr("writable", "msect", msd->md_sym);
		if ((msd->md_flag ^ flags) & MA_CONF)
			conflatr("conformable", "msect", msd->md_sym);
		if (msd->md_type != type)
			conflitm("msect", "type", msd->md_sym);
		if (msd->md_priv != priv)
			conflitm("msect", "privilege level", msd->md_sym);
		if (flags & MA_ADDR)
		{
			if ((msd->md_flag & MA_ADDR) && msd->md_addr != addr)
				confladdr("msect", msd->md_sym, msd->md_addr, addr);
			else
			{
				msd->md_flag |= MA_ADDR;
				msd->md_addr = addr;
			}
		}
		if (sgd != NULL && msd->md_sgd != sgd)
			conflblk("Msect", msd->md_sym, "segment", sgd->sd_sym);
	}
	if ((pnt = (MD **)sbrkx(sizeof(MD *), &msectnummem)) == NULL)
		fail("Cannot allocate memory for msect number table");
	*pnt = msd;							// Store pointer in local table
	curmod->mb_msectnummax++;
	return (msd);
}


//**************************************
// Function: declpsect - Declare a psect
// Returned: Nothing
//**************************************

static PD *declpsect(
	MD  *msd,
	int  flags,
	long addr,
	int  mod,
	long tsize,
	long lsize)
{
    PD **pnt;
    PD  *psd;
    PD  *psdf;
    SY  *sym;

	if (mod < 0)
		mod = 0;
	if (addr < 0)
		addr = 0;
	symbuf[0] = SYM_PSCT;				// Indicate psect name
	if ((sym = looksym()) == NULL)		// Find psect symbol table entry
	{
		sym = entersym();				// Not there - create one now
		sym->sy_psd = NULL;
		psd = (PD *)getspace(sizeof(PD));
		memset(psd, 0, sizeof(PD));
		psd->pd_magic = 'DSP*';
		sym->sy_val.p = psd;			// Create psect data block
		psd->pd_sym = sym;
		psd->pd_msd = msd;
		psd->pd_loclast = &psd->pd_lochead;
		psd->pd_flag = flags;
		psd->pd_addr = addr;
		psd->pd_modulus = mod;
		if (msd != NULL)
		{
			if (msd->md_tail)			// Link into psect list
				msd->md_tail->pd_next = psd;
			else
				msd->md_head = psd;
			msd->md_tail = psd;
		}
		psd->pd_next = NULL;
		psd->pd_tsize = tsize;			// And as size
		psd->pd_qsize = lsize;
		psd->pd_lsize = lsize;
		psd->pd_num = ++pscgbn;
	}
	else								// If this psect is already defined
	{
		psd = sym->sy_val.p;
		if (((psd->pd_flag & PA_OVER) == 0) ^ ((flags & PA_OVER) == 0))
			conflatr("concatenated/overlayed", "psect", psd->pd_sym);

		if (msd != NULL && psd->pd_msd != msd)
		{
			conflblk("Psect", psd->pd_sym, "msect", msd->md_sym);
			msd = psd->pd_msd;
		}
		if (flags & PA_ADDR)			// Was an address specified for the
		{								//   psect?
			if (psd->pd_flag & PA_ADDR) // Yes - do we have an address now?
			{
				if (psd->pd_addr != addr) // Yes - are they the same?
										// No - complain
					confladdr("psect", psd->pd_sym, psd->pd_addr, addr);
			}
			else						// If don't have an address now
			{
				psd->pd_flag |= PA_ADDR;
				psd->pd_addr = addr;
			}
		}								// If address not specified

		if (mod > 1)					// Was a modulus specified?
		{
			if (psd->pd_modulus > 1)	// Yes - do we have a modulus now?
			{							// Yes - make it compatable
				mod = psd->pd_modulus = resolvemod(psd->pd_modulus, mod);

			}
			else
				psd->pd_modulus = mod;	// No - just store the new value
		}
		if (psd->pd_modulus > 1 &&			   // Check for conflict between
				(addr % psd->pd_modulus) != 0) //   address and modulus
			warnpsaddr(psd);

		if (psd->pd_flag & PA_OVER)		// Overlayed psect?
		{
			if (tsize > psd->pd_tsize)	// Yes - is this piece bigger?
				psd->pd_tsize = tsize;	// Yes - make this the current
										//   size
		}
		else
			psd->pd_tsize += tsize;
		psd->pd_qsize = lsize;
    }

	// Now set up the per-file psect data

	symsize += sprintf(symbuf + symsize, "\03%d", modulenum);
	if ((sym = looksym()) != NULL)		// Check for duplicate
		psdf = sym->sy_val.p;
	else
	{
		sym = entersym();				// Not there - create it now
		psd->pd_magic = 'DSP*';
		sym->sy_psd = NULL;
		psdf = (PD *)getspace(sizeof(PD));
		memset(psdf, 0, sizeof(PD));
		psdf->pd_magic = 'DSP*';

		psdf->pd_filepd = psd;

		sym->sy_val.p = psdf;			// Create psect data block
		psdf->pd_sym = sym;
		psdf->pd_msd = psd->pd_msd;
		psdf->pd_modblk = curmod;
		psdf->pd_loclast = &psdf->pd_lochead;
		psdf->pd_flag = psd->pd_flag;
		psdf->pd_addr = psd->pd_addr;
		psdf->pd_modulus = mod;
		psdf->pd_tsize = tsize;
		psdf->pd_qsize = lsize;
		psdf->pd_lsize = lsize;
		psdf->pd_num = psd->pd_num;
		if (psd->pd_filetail)			// Link into list for psect
			psd->pd_filetail->pd_next = psdf;
		else
			psd->pd_filehead = psdf;
		psd->pd_filetail = psdf;
		psdf->pd_next = NULL;
		if ((pnt = (PD **)sbrkx(sizeof(PD *), &psectnummem)) == NULL)
			fail("Cannot allocate memory for psect number table");
		*pnt = psdf;					// Store pointer in local table
		(curmod->mb_psectnummax)++;
    }
	if (psd->pd_magic != 'DSP*')
		BREAK();
	if (psdf->pd_magic != 'DSP*')
		BREAK();

	if (msd != NULL)
	{
		if (psd->pd_msd == NULL)
			BREAK();
		if (psdf->pd_msd == NULL)
			BREAK();
	}
	return (psdf);
}

