//================================================
//
// MAP.C - Routines to generate load map for XLINK
//
//================================================
//
// Written by John Goltz
//
//================================================


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <errno.h>
#include <heapsort.h>
#include <xos.h>
#include <procarg.h>
#include <xcmalloc.h>
#include <xostime.h>
#include <xossvc.h>
#include <xoserrmsg.h>
#include "xlink.h"

int    fisctrm(FILE *stream);

extern SY *sympnt;

// Allocate local data

SY     dmysym;
int    pagenum;			// Map page number
int    linecnt;			// Line counter
char   hvdnmpo;			// TRUE if any map output has been done
char   timebfr[32];		// Buffer for date and time string

// Table of msect type names 

char *typename[] = {"None", "Data", "Code", "Stack", "Combined"};


static void addrconfl2(FILE *file, MD *md1, long top1, MD *md2, long  top2);
static void mapsummary(void);
static void mapsymbols(PD *psd);


//**************************************
// Function: mapbegin - Begin map output
// Returned: Nothing
//**************************************

void mapbegin(void)

{
    time_d timeloc;

    svcSysDateTime(2, &timeloc);	// Get current data and time
    ddt2str(timebfr, "%H:%m %W %D-%3n-%y", (time_dz *)&timeloc);
									// Change to string
    if (mapfsp != 0 && strcmp(mapfsp, "#") == 0)
        mapfp = stdout;
    else
    {
        if (mapfsp == NULL)
            mapfsp = usefif(mapext);
        if ((mapfp = fopen(mapfsp, "w")) == NULL)
            femsg(prgname, -errno, mapfsp);
    }
    mapflag = !fisctrm(mapfp);		// Remember if map is to console
    pagenum = 0;
    linecnt = 0;
}


//*******************************************
// Function: mapout - Output main part of map
// Returned: Nothing
//*******************************************

void mapout(void)

{
    SD   *sgd;
    char *str;

    mapnline();
    fputs(">>> File header data", mapfp);
    mapnline();
    mapnline();
    fputs("    RUN file version:  1", mapfp);
    mapnline();
    switch (ptype)
    {
     case P_LOW:
        str = "Unspecified, low order first";
        break;

     case P_Z80:
        str = "Z80";
        break;

     case P_8086:
        str = "8086";
        break;

     case P_80186:
        str = "80186";
        break;

     case P_80286:
        str = "80286";
        break;

     case P_80386:
        str = "80386";
        break;

     case P_80486:
        str = "80486";
        break;

     case P_80586:
        str = "80586";
        break;

     case P_HIGH:
        str = "Unspecified, high order first";
        break;

     case P_68000:
        str = "68000";
        break;

     default:
        str = "??????";
        break;
    }
    fprintf(mapfp, "    Processor type: (0x%2.2X) %s", ptype, str);
    mapnline();
    switch (itype)
    {
     case I_ALONE:
        str = "Stand-alone";
        break;

     case I_XOSUSER:
        str = "XOS user";
        break;

     case I_MSDOS:
        str = "MSDOS";
        break;

     case I_XOS8086:
        str = "XOS 8086 mode";
        break;

     case I_XOSLKE:
        str = "XOS Loadable Kernel Extension";
        break;

     default:
        str = "??????";
        break;
    }
    fprintf(mapfp, "    Image type:     (0x%2.2X) %s", itype, str);
    mapnline();
    if (strpsd)
        fprintf(mapfp, "    Start address:   0x%8.8lX%c (Msect: %s)",
                stradr, (strpsd->pd_msd->md_flag&MA_ADDR)? ' ': '*',
                strpsd->pd_msd->md_sym->sy_name+1);
    else
        fputs("    No start address", mapfp);
    mapnline();
    if (dbgpsd)
        fprintf(mapfp, "    Debug address:   0x%8.8lX%c (Msect: %s)",
                dbgadr, (dbgpsd->pd_msd->md_flag&MA_ADDR)? ' ': '*',
                dbgpsd->pd_msd->md_sym->sy_name+1);
    else
        fputs("    No debug address", mapfp);
    mapnline();
    if (stkpsd)
        fprintf(mapfp, "    Stack address:   0x%8.8lX%c (Msect: %s)",
                stkadr, (stkpsd->pd_msd->md_flag&MA_ADDR)? ' ': '*',
                stkpsd->pd_msd->md_sym->sy_name+1);
    else
        fputs("    No stack address", mapfp);
    mapnline();
    mapnline();

	mapsummary();

///	if (abspsd.pd_symhead == NULL)		// Do we have any absolute symbols?
///	{
///		fputs(">>> No absolute symbols defined", mapfp); // No
///		mapnline();
///	}
///	else
///	{
		maptline(4);					// Require 4 lines on this page
		fputs(">>> Absolute symbols", mapfp); // Yes - list them
		mapnline();
		mapsymbols(&abspsd);
///	}
    mapnline();
    if ((sgd=sgdhead) != NULL)			// Point to first segment
    {
        do
        {   map1seg(sgd);
        } while ((sgd=sgd->sd_next) != NULL);
    }
}


//********************************************************
// Function: mapsummary - Generate summary part of the map
// Returned: Nothing
//********************************************************

static void mapsummary(void)
{
	SD *sgd;
	MD *msd;
	PD *psd;

	maptline(4);
	fputs(">>> Structure summary:", mapfp);
	mapnline();
	mapnline();
	sgd = sgdhead;
	while (sgd != NULL)
	{
	    fprintf(mapfp, "    Segment #%-2d %-15s %08X %04X", sgd->sd_num,
				sgd->sd_sym->sy_name + 1, sgd->sd_addr, sgd->sd_select);
	    mapnline();

		msd = sgd->sd_head;
		while (msd != NULL)
		{
			fprintf(mapfp, "      Msect #%-2d %-15s %08X %d", msd->md_num,
				msd->md_sym->sy_name + 1, msd->md_addr, msd->md_lsize);
		    mapnline();
			psd = msd->md_head;
			while (psd != NULL)
			{
	            fprintf(mapfp, "        Psect   %-15s %08X %d",
						psd->pd_sym->sy_name + 1, psd->pd_base,
						psd->pd_lsize);
			    mapnline();
				psd = psd->pd_next;
			}
			msd = msd->md_next;
		}
		sgd = sgd->sd_next;
	}
    mapnline();
}


//***********************************************
// Function: map1seg - Generate map for a segment
// Returned: Nothing
//***********************************************

void map1seg(
    SD *sgd)

{
    MD *msd;

    maptline(10);			// Require 10 lines here
    fprintf(mapfp, ">>> Segment #%-2d  %s", sgd->sd_num,
            sgd->sd_sym->sy_name+1);
    mapnline();
    if (sgd->sd_linked == NULL)
        fputs("    Linked segment: None", mapfp);
    else
        fprintf(mapfp, "    Linked segment: #%d (%s)", sgd->sd_linked->sd_num,
                sgd->sd_linked ->sd_sym->sy_name+1);
    mapnline();
    fprintf(mapfp, "    Segment type: %s", typename[sgd->sd_type]);
    mapnline();
    fprintf(mapfp, "    Privilege level: %d", sgd->sd_priv);
    mapnline();
    fputs("    Attributes:", mapfp);
    if (sgd->sd_flag &
            (SA_LARGE|SA_32BIT|SA_CONF|SA_WRITE|SA_READ|SA_EXTEND|SA_SHARE))
    {
        if (sgd->sd_flag & SA_LARGE)
            fputs(" Large", mapfp);
        if (sgd->sd_flag & SA_32BIT)
            fputs(" 32-bit", mapfp);
        if (sgd->sd_flag & SA_CONF)
            fputs(" Conformable", mapfp);
        if (sgd->sd_flag & SA_WRITE)
            fputs(" Writable", mapfp);
        if (sgd->sd_flag & SA_READ)
            fputs(" Readable", mapfp);
        if (sgd->sd_flag & SA_EXTEND)
            fputs(" Extendable", mapfp);
        if (sgd->sd_flag & SA_SHARE)
            fputs(" Sharable", mapfp);
    }
    else
        fputs(" None", mapfp);
    mapnline();
    if (sgd->sd_select == 0)
        fputs("    No selector specified", mapfp);
    else
        fprintf(mapfp, "    Segment selector: %4.4X", (ushort)(sgd->sd_select));
    mapnline();
    if (sgd->sd_addr == 0xFFFFFFFFL)
        fputs("    No address or modulus specified", mapfp);
    else
        fprintf(mapfp, "    %s: %8.8lX", (sgd->sd_flag & SA_ADDR)?
                "Address": "Modulus", sgd->sd_addr);
    mapnline();
    mapnline();
    if ((msd = sgd->sd_head) != NULL) // Point to first msect
    {
        do
        {   map1msect(msd);
        } while ((msd = msd->md_next) != NULL);
    }
}



//***********************************************
// Function: map1msect - Generate map for 1 msect
// Returned: Nothing
//***********************************************

void map1msect(
    MD *msd)

{
    PD *psd;

    maptline(10);			// Require 10 lines here
    fprintf(mapfp, ">>> Msect #%-2d  %s/%s", msd->md_num,
			msd->md_sgd->sd_sym->sy_name + 1, msd->md_sym->sy_name + 1);
    mapnline();
    fprintf(mapfp, "    Loaded size:  0x%08.8X (%d.)", msd->md_lsize,
            msd->md_lsize);
    mapnline();
    fprintf(mapfp, "    Total size:   0x%08.8X (%d.)", msd->md_tsize,
            msd->md_tsize);
    mapnline();
    fprintf(mapfp, "    Msect type: %s", typename[msd->md_type]);
    mapnline();
    fprintf(mapfp, "    Privilege level: %d", msd->md_priv);
    mapnline();
    fputs("    Attributes:", mapfp);
    if (msd->md_flag & (MA_CONF|MA_WRITE|MA_READ|MA_SHARE))
    {
        if (msd->md_flag & MA_CONF)
            fputs(" Conformable", mapfp);
        if (msd->md_flag & MA_WRITE)
            fputs(" Writable", mapfp);
        if (msd->md_flag & MA_READ)
            fputs(" Readable", mapfp);
        if (msd->md_flag & MA_SHARE)
            fputs(" Sharable", mapfp);
    }
    else
        fputs(" None", mapfp);
    mapnline();
    if (!(msd->md_flag & MA_ADDR) && msd->md_mod == 0xFFFFFFFFL)
        fputs("    No address or modulus specified", mapfp);
    else
	{
		if (msd->md_flag & MA_ADDR)
	        fprintf(mapfp, "    Address spec: 0x%08.8X", msd->md_addr);
		else
    	    fprintf(mapfp, "    Modulus spec: 0x%08.8X", msd->md_mod);
	}
    mapnline();

    if (msd->md_flag & MA_ADDR)
        fprintf(mapfp, "    Address used: 0x%08.8X", msd->md_addr);
    else
        fputs("    No address used", mapfp);
    mapnline();
    mapnline();
    if ((psd = msd->md_head) != NULL)	// Point to first psect
    {
        do
        {
			maptline(7);				// Require 7 lines here
            fprintf(mapfp, ">>> Psect  %s/%s/%s",
					psd->pd_msd->md_sgd->sd_sym->sy_name + 1,
					psd->pd_msd->md_sym->sy_name + 1, psd->pd_sym->sy_name + 1);
            mapnline();
            fprintf(mapfp, "    Loaded size:  0x%08.8X (%d.)", psd->pd_lsize,
                    psd->pd_lsize);
            mapnline();
            fprintf(mapfp, "    Total size:   0x%08.8X (%d.)", psd->pd_tsize,
                    psd->pd_tsize);
            mapnline();
            fprintf(mapfp, "    Attributes: %s",
                        (psd->pd_flag & PA_OVER)? "Overlayed": "Concatenated");
            mapnline();
            if (psd->pd_flag & PA_ADDR)
	            fprintf(mapfp, "    Address specified: 0x%08.8lX",
						psd->pd_addr);
			else
                fputs("    No address specified", mapfp);
			mapnline();
			if (psd->pd_modulus > 1)
	            fprintf(mapfp, "    Modulus spec: 0x%08.8X",
						psd->pd_modulus);
			else
                fputs("    No modulus specified", mapfp);
            mapnline();
            fprintf(mapfp, "    Address used: 0x%08.8lX", psd->pd_base);
            mapnline();
			mapsymbols(psd);
            mapnline();
        } while ((psd = psd->pd_next) != NULL);
    }
}


//**********************************************************
// Function: mapsymbols - Output symbols for map for a psect
// Returned: Nothing
//**********************************************************

static void mapsymbols(
	PD *psd)

{
    SY   *thissym;
	PD   *psdf;
	MD   *msd;
    char *symfmt1a = 0;
    char *symfmt1b;
    char *symfmt2a = 0;
    char *symfmt2b;
    char *symfmt3a = 0;
    char *symfmt3b;
    int   symcnt;		// Symbol counter
    int   symmax;
    int   select = 0;
	int   relflag;

	relflag = (psd->pd_msd == NULL || (psd->pd_msd->md_flag & MA_ADDR)) ?
			' ' : '*';
	psdf = psd->pd_filehead;
	while (psdf != NULL)
	{
		if (psdf->pd_symhead != NULL || psdf->pd_tsize != 0)
		{
			mapnline();
			maptline(5);				// Require 5 lines on this page
			if (psd == &abspsd)
	            fprintf(mapfp, ">>> Module  Absolute/%s [%s]",
						psdf->pd_modblk->mb_name,
						psdf->pd_modblk->mb_obj->obj_fsp);
			else
	            fprintf(mapfp, ">>> Module  %s/%s/%s/%s [%s]",
						psd->pd_msd->md_sgd->sd_sym->sy_name + 1,
						psd->pd_msd->md_sym->sy_name + 1,
						psd->pd_sym->sy_name + 1,
						psdf->pd_modblk->mb_name,
						psdf->pd_modblk->mb_obj->obj_fsp);
		    mapnline();
			if (psdf->pd_msd != NULL)
			{
				fprintf(mapfp, "    Loaded size:  0x%08.8X (%d.)",
						psdf->pd_lsize, psdf->pd_lsize);
				mapnline();
				fprintf(mapfp, "    Total size:   0x%08.8X (%d.)",
						psdf->pd_tsize, psdf->pd_tsize);
				mapnline();
				if (psdf->pd_modulus > 1)
					fprintf(mapfp, "    Modulus spec: 0x%08.8X",
							psdf->pd_modulus);
				else
					fputs("    No modulus specified", mapfp);
				mapnline();
				fprintf(mapfp, "    Address used: 0x%08.8X", psdf->pd_base);
				mapnline();
			}
			if (psdf->pd_symhead != NULL)
			{
				mapnline();
				maptline(3);			// Require 3 lines on this page
				if (psdf->pd_ssmax > 23+1 || psdf->pd_symhead->sy_next == NULL)
				{
					fputs("      Value     Symbol", mapfp);
								symmax = 0;
					symfmt1b = "      %8.8lX%c %s";
					symfmt2b = " %4.4X:%8.8lX%c %s";
					symfmt3b = " ---- %8.8lX%c %s";
				}
				else if (psdf->pd_ssmax > 10+1)
				{
					fputs("      Value     Symbol                       Value"
						  "     Symbol", mapfp);
					symmax = 1;
					symfmt1a = "      %8.8lX%c %-23s";
					symfmt1b = "      %8.8lX%c %s";
					symfmt2a = " %4.4X:%8.8lX%c %-23s";
					symfmt2b = " %4.4X:%8.8lX%c %s";
					symfmt3a = " ---- %8.8lX%c %-23s";
					symfmt3b = " ---- %8.8lX%c %s";
				}
				else
				{
					fputs((psdf->pd_symhead->sy_next->sy_next != NULL) ?
							"      Value     Symbol          Value     Symbol"
							"          Value     Symbol" :
							"      Value     Symbol          Value     Symbol",
							mapfp);
					symmax = 2;
					symfmt1a = "      %8.8lX%c %-10s";
					symfmt1b = "      %8.8lX%c %s";
					symfmt2a = " %4.4X:%8.8lX%c %-10s";
					symfmt2b = " %4.4X:%8.8lX%c %s";
					symfmt3a = " ---- %8.8lX%c %-10s";
					symfmt3b = " ---- %8.8lX%c %s";
				}
				symcnt = 0;
				if ((thissym = psdf->pd_symhead) != NULL)
				{
					psdf->pd_symhead = thissym = (SY *)heapsort(thissym,
							(int (*)(void *__p1,void *__p2,void *__p3))cmpsym, 
							psdf);			// Sort symbols for the psect
					do
					{
						if (!(thissym->sy_flag & SYF_LOCAL))
						{
							if (--symcnt < 0) // Room for more on this line?
							{
								symcnt = symmax; // No - start new line
								mapnline();
							}
							if (thissym->sy_flag & SYF_ADDR)
							{
								msd = thissym->sy_psd->pd_msd;
								if ((thissym->sy_flag & SYF_ABS) || (select =
										(msd == NULL) ? 0 :
										msd->md_sgd->sd_select) != 0)
									fprintf(mapfp, (symcnt) ? symfmt2a :
											symfmt2b,
											(thissym->sy_flag & SYF_ABS) ?
											((ushort)(thissym->sy_sel)) :
											((ushort)select), thissym->sy_val.v,
											relflag, thissym->sy_name+1);
								else
									fprintf(mapfp, (symcnt) ? symfmt3a :
											symfmt3b, thissym->sy_val.v,
											relflag, thissym->sy_name+1);
							}
							else
								fprintf(mapfp, (symcnt) ? symfmt1a : symfmt1b,
										thissym->sy_val.v, relflag,
										thissym->sy_name+1);
						}
					} while ((thissym = thissym->sy_next) != NULL);
			    }
			    mapnline();				// End the last line
			}
		}
		psdf = psdf->pd_next;
	}
}


//************************************************
// Function: cmpsym - Compare symbols for heapsort
// Returned: +1 if a > b, 0 if a == b, -1 if a < b
//************************************************

int cmpsym(
    SY *a,
    SY *b,
    PD *psd)

{
    if (psd->pd_msd == NULL)
    {
		if (a->sy_sel != b->sy_sel)
			return ((((ushort)(a->sy_sel)) > ((ushort)(b->sy_sel))) ? 1 : -1);
    }
    return (a->sy_val.v - b->sy_val.v);
}


//***************************************************
// Function: mapconstr - Output string to console and
//				to map (string must NOT contain \n)
// Returned: Nothing
//***************************************************

void mapconstr(str)
char *str;

{
    fputs(str, stderr);
    if (mapflag)
        fputs(str, mapfp);
}


// Function to start new line on console and for map

void xeline(void)

{
    putc('\n', stderr);
    if (mapflag)
		mapnline();
}


void xnline(void)
{
    putc('\n', stdout);
    if (mapflag)
		mapnline();
}


// Function to start new line for map

void mapnline(void)
{
    if (--linecnt >= 0)					// Need new page?
		putc('\n', mapfp);				// No - just start new line
    else
		mapnpage();
}


// Function to ensure that have n lines on current page

void maptline(
	int lines)
{
    if (linecnt < lines)				// Enough left?
		mapnpage();						// No - start new page
}


// Function to start new page for map

void mapnpage(void)
{
	char text[64];

    if (hvdnmpo)						// Is this the first time?
		fputs("\n\f", mapfp);			// No - end previous page
    else
	hvdnmpo = TRUE;
    linecnt = MAPPGSZ;					// Yes - reset count
	sprintf(text, "%d.%d.%d", MV, NV, EN);
    fprintf(mapfp, "Load map    %s    XLINK - version %-32sPage %d\n\n\n",
			timebfr, text, ++pagenum);
}


//***************************************************************
// Function: listgbl - List undefined or multiply defined globals
// Returned: Nothing
//***************************************************************

void listgbl(
    int   cnt,			// Number to list
    char *msg,			// Header message
    int   stst)			// Bit to test in sy_flag

{
    char  s;
    int   symnum;
    int   symmax;
    int   symcnt;
    int   tblcnt;
    SY   *sym;
    SY  **tblpnt;
    char *cpnt;

    static char header[] = "? %d. %sdefined global symbol%c";

    if (cnt == 0)						// Anything to do here?
		return;							// No
    s = (cnt == 1)? '\0':'s';
    fprintf(stderr, header, cnt, msg, s); // Output header to terminal
    if (mapflag)						// And to map if should
	{
		maptline(4);
		fprintf(mapfp, header, cnt, msg, s);
	}
    symmax = 0;
    tblcnt = HASHSIZE;
    tblpnt = hashtbl;
    while (--tblcnt >= 0)				// Scan the hash table to find
    {									//   longest symbol to print
		if ((sym=*tblpnt++) != NULL)
		{
			do
			{
				if (sym->sy_flag & stst) // Should we print this symbol?
				{
					cpnt = sym->sy_name+1; // Yes - see how long it is
					symcnt = 1;
					while (*cpnt++ > 0)
						symcnt++;
					if (symmax < symcnt)
						symmax = symcnt;
				}
			} while ((sym=sym->sy_hash) != NULL);
		}
    }
    symnum = 79/(symmax+2) - 1;			// Number of symbols per line - 1
    symcnt = 0;
    tblcnt = HASHSIZE;
    tblpnt = hashtbl;
    while (--tblcnt >= 0)				// Scan the hash table
    {
		if ((sym=*tblpnt++) != NULL)
			do
			{   if (sym->sy_flag & stst) // Should we print this symbol?
				{
					if (--symcnt < 0)	// Need new line?
					{
						symcnt = symnum; // Yes
						xeline();
					}
                    cpnt = sym->sy_name+1; // Output symbol name
                    fprintf(stderr, "  %-*s", symmax, cpnt);
					if (mapflag)
                        fprintf(mapfp, "  %-*s", symmax, cpnt);
			}
	    } while ((sym=sym->sy_hash) != NULL);
    }
    xeline();
    if (mapfsp)
        mapnline();
}

// Function to output illegal relocatable expression message

void badrexp(void)

{
    mapconstr("Illegal relocatable expression");
    showfile(2);
    showplace(NULL);
}

// Function to output expression stack underflow message

void underflow(void)

{
    mapconstr("Expression stack underflow");
    showfile(2);
}

// Function to output expression stack overflow message

void overflow(void)

{
    mapconstr("Expression stack overflow");
    showfile(2);
}


//**************************************************************
// Function: warnaddr - Output warning message if address or 
//				modulus specified for Z80 or 8086 type processor
// Returned: Nothing
//**************************************************************

void warnaddr(
	MD *msd)

{
    static char fmt[] = "Address or modulus specified for msect %s for";
    char  *name;

    name = msd->md_sym->sy_name+1;
    fprintf(stderr, fmt, name);
    if (mapflag)
	{
		maptline(2);
        fprintf(mapfp, fmt, name);
	}
    xeline();
    mapconstr("  processor which does not allow this");
    showfile(2);
}

// Function to output message to complain if absolute address is specified
//   as a starting, debug, or stack address

void absaddr(msg)
char *msg;

{
    static char fmt[] = "Absolute %s address specified";

    fprintf(stderr, fmt, msg);
    if (mapflag)
	{
		maptline(2);
        fprintf(mapfp, fmt, msg);
	}
    showfile(2);
}

// Function to output message to complain if multpile values are specified
//   as a starting, debug, or stack address

void multaddr(msg)
char *msg;

{
    static char fmt[] = "Multiple %s addresses specified";

    fprintf(stderr, fmt, msg);
    if (mapflag)
	{
		maptline(2);
        fprintf(mapfp, fmt, msg);
	}
    showfile(2);
}

// Function to output message to complain if no segment associated with
//   global symbol

void nosymseg(void)

{
    static char fmt[] = "No segment associated with symbol %s";
    char  *name;

    name = sympnt->sy_name+1;
    fprintf(stderr, fmt, name);
    if (mapflag)
	{
		maptline(2);
        fprintf(mapfp, fmt, name);
	}
    showfile(2);
}


// Function to output message to complain about missing or bad symbol
//   table psect

void nosyms(
	char *msg)
{
    static char fmt[] = "Psect _symbols_p not %s, no symbol table loaded";

    fprintf(stderr, fmt, msg);
    if (mapflag)
	{
		maptline(2);
		fprintf(mapfp, fmt, msg);
	}
    xeline();
    stbpsd = NULL;
    opsymsz = 0;
}


// Function to output truncation warning message

void truncate(
    int size)
{
    static char fmt[] = "Value truncated to %d. bits";

    fprintf(stderr, fmt, size);
    if (mapflag)
	{
		maptline(3);
		fprintf(mapfp, fmt, size);
	}
    showfile(2);
    showplace(NULL);
}


// Function to output message for relative reference to different address
//   space

void diffspace(
	SY *sym)
{
    mapconstr("Relative reference to different segment");
    showfile(2);
    showplace(sym);
}


// Function to report where in image an error occured

void showplace(
	SY *sym)
{
    long   place;
    char   reloc;
    static char fmt1[] = "symbol %s ";
    static char fmt2[] = "in psect %s at address %8.8lX%c";

    mapconstr("  ");
    if (sym != NULL)
    {
        fprintf(stderr, fmt1, sym->sy_name+1);
        if (mapflag)
            fprintf(mapfp, fmt1, sym->sy_name+1);
    }
    place = curpsd->pd_base + offset - curpsd->pd_offset;
    reloc = (curmsd->md_flag & MA_ADDR)? ' ': '*';
    fprintf(stderr, fmt2, curpsd->pd_sym->sy_name+1, place, reloc);
    if (mapflag)
	fprintf(mapfp, fmt2, curpsd->pd_sym->sy_name+1, place, reloc);
    xeline();
}


//******************************************************
// Function: impaldef - Report imported symbol conflicts
// Returned: Nothing
//******************************************************

void impaldef(
	SY *sym)
{
	static char fmt[] = "Imported symbol %s is already\n"
			"defined as external or internal";

    fprintf(stderr, fmt, sym->sy_name + 1);
    if (mapflag)
	{
		maptline(2);
	    fprintf(mapfp, fmt, sym->sy_name + 1);
	}
    xeline();
}


// Function to output warning message for multiply defined symbols

void multdef(
    SY  *sym,
    long value,
    int  psn)
{
    char *msg;
    PD   *psd;
    char  msgx[50];

    static char fmt1[] = "  value = %8.8lX (%s)";

    varwmsg(sym, "Multiply defined", "defined"); // Output the first line
    if (psn == 0)						// If absolute value
		msg = "absolute";
    else 								// If for psect
    {
		psd = getpsd(psn);
        sprintf(msgx, "psect %s", psd->pd_sym->sy_name+1);
        msg = msgx;
    }
    fprintf(stderr, fmt1, value, msg);
    if (mapflag)
	{
		maptline(2);
	    fprintf(mapfp, fmt1, value, msg);
	}
    xeline();
}


// Function to output warning message for undefined or multiply defined symbols

void varwmsg(
	SY   *sym,					// Pointer to symbol table entry
	char *msg1,
	char  *msg2)
{
    static char fmt[] = "%s symbol %s %s";

    fprintf(stderr, fmt, msg1, sym->sy_name+1, msg2);
    if (mapflag)
	{
		maptline(2);
		fprintf(mapfp, fmt, msg1, sym->sy_name+1, msg2);
	}
    showfile(2);
}


//*************************************************************
// Function: addrconfl - Report address conflict between Msects
// Returned: Nothing
//*************************************************************

void addrconfl(
	MD  *md1,
	long top1,
	MD  *md2,
	long top2)
{
	errorflg = TRUE;					// Indicate fatal error
	addrconfl2(stderr, md1, top1, md2, top2);
	if (mapflag)
	{
		maptline(3);
		addrconfl2(mapfp, md1, top1, md2, top2);
	}
}


static void addrconfl2(
	FILE *file,
	MD   *md1,
	long  top1,
	MD   *md2,
	long  top2)
{
	char *name1;
	char *name2;
	char fmt1[] = "Address conflict between Msects %s and %s\n";
	char fmt2[] = "  %18s: 0x%08X to 0x%08X\n";

	name1 = md1->md_sym->sy_name + 1,
	name2 = md2->md_sym->sy_name + 1;
	fprintf(file, fmt1, name1, name2);
	fprintf(file, fmt2, name1, md1->md_addr, top1);
	fprintf(file, fmt2, name2, md2->md_addr, top2);
}



// Function to report attribute conflict

void conflatr(
	char *atrib,
	char *blk,
	SY   *name)
{
    static char fmt[] = "Conflicting attribute \"%s\" for %s %s";

    fprintf(stderr, fmt, atrib, blk, name->sy_name+1);
    if (mapflag)
	{
		maptline(2);
		fprintf(mapfp, fmt, atrib, blk, name->sy_name+1);
	}
    showfile(2);
}

// Function to report msect/segment conflict

void conflblk(
	char *blk1,
	SY   *name1,
	char *blk2,
	SY   *name2)
{
    static char fmt[] = "%s %s defined in conflicting %s %s";

    fprintf(stderr, fmt, blk1, name1->sy_name+1, blk2, name2->sy_name+1);
    if (mapflag)
	{
		maptline(2);
		fprintf(mapfp, fmt, blk1, name1->sy_name+1, blk2, name2->sy_name+1);
	}
    showfile(2);
}

// Function to report conflicting attribute

void conflitm(
	char *blk,
	char *thing,
	SY   *name)
{
    static char fmt[] = "Conflicting %s specified for %s %s";

    fprintf(stderr, fmt, thing, blk, name->sy_name+1);
    if (mapflag)
	{
		maptline(2);
        fprintf(mapfp, fmt, thing, blk, name->sy_name+1);
	}
    showfile(2);
}


// Function to report conflicting segment type

void conflitsegtype(
	int   newtype,
	int   oldtype,
	SY   *name)
{
    static char fmt[] = "Conflicting type specified for segment %s\n"
			"  Old type: %s, new type: %s";

    fprintf(stderr, fmt, name->sy_name+1, typename[oldtype], typename[newtype]);
    if (mapflag)
	{
		maptline(3);
	    fprintf(mapfp, fmt, name->sy_name+1, typename[oldtype],
				typename[newtype]);
	}
    showfile(2);
}



// Function to report address conflict

void confladdr(
	char *blk,
	SY   *name,
	int   curval,
	int   newval)
{
    static char fmt1[] = "Conflicting addresses for %s %s";
	static char fmt2[] = "    Current address = 0x%08.8X, New address = "
			"0x%08.8X\n";

    fprintf(stderr, fmt1, blk, name->sy_name+1);
    if (mapflag)
	{
		maptline(3);
		fprintf(mapfp, fmt1, blk, name->sy_name+1);
	}
    showfile(2);
    fprintf(stderr, fmt2, curval, newval);
    if (mapflag)
		fprintf(mapfp, fmt2, blk, curval, newval);
}


void msundef(
	char *msectname,
	char *label,
	char *name)
{
	char fmt[] = "Msect %s is not defined for MS %s %s";

	fprintf(stderr, fmt, msectname, label, name);
	if (mapflag)
	{
		maptline(2);
		fprintf(mapfp, fmt, msectname, label, name);
	}
	showfile(2);
}


//******************************************************************
// Function: warnpsaddr - Warn if psect address and modulus conflict
// Returned: Nothng
//******************************************************************

void  warnpsaddr(
	PD *psd)
{
    static char fmt1[] = "Addresse (0x%X) for psect %s conflicts ";
	static char fmt2[] = "with modulus (0x%X)";

    fprintf(stderr, fmt1, psd->pd_addr, psd->pd_sym->sy_name + 1);
    if (mapflag)
	{
		maptline(3);
		fprintf(mapfp, fmt1, psd->pd_addr, psd->pd_sym->sy_name + 1);
	}
    showfile(2);
    fprintf(stderr, fmt2, psd->pd_modulus);
    if (mapflag)
		fprintf(mapfp, fmt2, psd->pd_modulus);
}


// Function to report conflict in segment selector value

void conflsel(
	SY *name)
{
    static char fmt[] = "Conflicting selector value for segment %s";

    fprintf(stderr, fmt, name->sy_name+1);
    if (mapflag)
	{
		maptline(2);
		fprintf(mapfp, fmt, name->sy_name+1);
	}
    showfile(2);
}


//*********************************************************
// Function: confllink - Report conflict in segment linking
// Returned: Nothng
//*********************************************************

void confllink(
    SY *name)
{
    static char fmt[] = "Conflicting segment linkage for segment %s";

    fprintf(stderr, fmt, name->sy_name+1);
    if (mapflag)
	{
		maptline(2);
		fprintf(mapfp, fmt, name->sy_name+1);
	}
    showfile(2);
}


//********************************************
// Function: showfile = Output file and module
//				names at end of error message
// Returned: Nothing
//********************************************

void showfile(
    int spaces)
{
    static char fmt[] = "%*sin module %s [%s]";

    errorflg = TRUE;					// Indicate fatal error
    xeline();
    fprintf(stderr, fmt, spaces, "", modname, curobj->obj_fsp);
    if (mapflag)
	{
		maptline(2);
        fprintf(mapfp, fmt, spaces, "", modname, curobj->obj_fsp);
	}
    xeline();
}


/************************************************/
/* Function: dosummary - Produce summary output	*/
/* Returned: Nothing				*/
/************************************************/

void dosummary(void)
{
    int  temp;
    int  hcnt;
    int  ucnt;
    int  num;
    int  len;
    int  max;
    SY  *sym;
    SY **hpnt;

    static char fmthash[] = "Hash table usage: %d/%d; "
            "string length: ave=%d.%03.3d, max=%d";
    static char fmtseg[]   = "Segments:  ref: %-6d def: %d";
    static char fmtmsect[] = "Msects:    ref: %-6d def: %d";
    static char fmtpsect[] = "Psects:    ref: %-6d def: %d";
    static char fmtsym[]   = "Symbols:   ref: %-6d def: %d";
    static char fmtnames[] = "MS names:  ref: %d";

    if (mapflag)
    {
        fputs(">>> Summary", mapfp);
        mapnline();
        mapnline();
    }
    hpnt = hashtbl;			// Determine hash table usage
    hcnt = HASHSIZE;
    ucnt = 0;
    temp = 0;
    max = 0;
    do
    {   if ((sym=*hpnt++) != NULL)
	{
	    ++ucnt;
            len = 0;
	    do
            {   ++temp;
                ++len;
	    } while ((sym=sym->sy_hash) != NULL);
            if (len > max)
                max = len;
	}
    } while (--hcnt > 0);
    temp  = (temp * 1000) / ucnt;
    hcnt = temp % 1000;
    temp /= 1000;
    printf(fmthash, ucnt, HASHSIZE, temp, hcnt, max);
    if (mapflag)
        fprintf(mapfp, fmthash, ucnt, HASHSIZE, temp, hcnt, max);
    xnline();
    if (segnummem.mab_mempnt != SEGNUMBASE)
    {
        
        num = (int)(segnummem.mab_mempnt - segnummem.mab_membase)/4;
        printf(fmtseg, num, seggbn);
        if (mapflag)
            fprintf(mapfp, fmtseg, num, seggbn);
        xnline();
    }

    if (msectnummem.mab_mempnt != MSECTNUMBASE)
    {
        num = (int)(msectnummem.mab_mempnt - msectnummem.mab_membase)/4;
        printf(fmtmsect, num, mscgbn);
        if (mapflag)
            fprintf(mapfp, fmtmsect, num, mscgbn);
        xnline();
    }

    if (psectnummem.mab_mempnt != PSECTNUMBASE)
    {
        num = (int)(psectnummem.mab_mempnt - psectnummem.mab_membase)/4;
        printf(fmtpsect, num, pscgbn);
        if (mapflag)
            fprintf(mapfp, fmtmsect, num, pscgbn);
        xnline();
    }

    if (symnummem.mab_mempnt != SYMNUMBASE)
    {
        num = (int)(symnummem.mab_mempnt - symnummem.mab_membase)/4;
        printf(fmtsym, num, symtblsize);
        if (mapflag)
            fprintf(mapfp, fmtsym, num, symtblsize);
        xnline();
    }

    if (lnamesmem.mab_mempnt != LNAMESBASE)
    {
        num = (int)(lnamesmem.mab_mempnt - lnamesmem.mab_membase)/4;
        printf(fmtnames, num);
        if (mapflag)
            fprintf(mapfp, fmtnames, num);
        xnline();
    }
}
