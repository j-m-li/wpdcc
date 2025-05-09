//********************************************************
// LISTFIN.C Functions to do final listing output for XMAC
//********************************************************
// Written by John Goltz
//********************************************************

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
#include <string.h>
#include <ctype.h>
#include "heapsort.h"
#include "xos.h"
#include <search.h>
#include "xmac.h"

// Allocate local data

char *thead1;		// Pointer to first table header line
char *thead2;		// Pointer to second table header line
char *typstr[] =	// Strings used to display msect type
{   {"None"},
    {"DATA"},
    {"CODE"},
    {"STACK"},
    {"COMB"}
};


static void listtbl(SY *sym, char *head1, char *head2, char *fill, int mac);


//***********************************************
// Function: finlisting - Do final listing output
// Returned: Nothing
//***********************************************

void finlisting(void)

{
    union
    {   SD *s;
        MD *m;
        PD *p;
    } pnt;

    lincnt = 0;							// Start new listing page
    pagcnm = 0;
    if (lstfp)
    {
		thissrc = NULL;					// No source name in header now
		strcpy(hdrstb, "Symbol table");
		nextpage();						// Start new page
		chkhead();
		if (listtype == LT_CREF && machead) // Doing cref listing?
			listtbl(machead, "Macros:",	// Yes - list macro table if need to
                    "Name                               References\n",
	            "                                       ", TRUE);
		if (symhead) 					// List symbol table if need to
			listtbl(symhead, "Symbols:", (listtype == LT_CREF) ?
					"Typ Value   Psect Name                              "
                    "References\n":
					"Typ Value   Psect Name                              "
					"Typ Value   Psect Name\n",
					"                                                  ",
                    FALSE);
        if ((pnt.s = sghead) != NULL)
        {
            listline("Segments:", 0);
            listline("Name                             Select Mod       Addr"
                     "      Type   Priv  Attributes", 0);
            do
            {   fprintf(lstfp, "%-32.32s ", pnt.s->sd_nba->sn_name);
                if (pnt.s->sd_select == 0)
                    fputs("None   ", lstfp);
                else
                    fprintf(lstfp, "%4.4X   ", pnt.s->sd_select);
                if ((pnt.s->sd_flag & SF_ADDR) || pnt.s->sd_addr == 0xFFFFFFFFL)
                    fputs("None      ", lstfp);
                else
                    fprintf(lstfp, "%8.8lX  ", pnt.s->sd_addr);
                if (!(pnt.s->sd_flag & SF_ADDR) || pnt.s->sd_addr ==
                        0xFFFFFFFFL)
                    fputs("None      ", lstfp);
                else
                    fprintf(lstfp, "%8.8lX  ", pnt.s->sd_addr);
                fprintf(lstfp, "%-5.5s %4d  ",
                        typstr[pnt.s->sd_type], pnt.s->sd_priv);
                if ((pnt.s->sd_atrb &
                        (SA_LARGE|SA_32BIT|SA_CONF|SA_READ|SA_WRITE))
                        || (pnt.s->sd_flag & SF_EXTEND))
                {
                    fputs ((pnt.s->sd_atrb & SA_32BIT)? " 32-bit": "16-bit",
                            lstfp);
                    if (pnt.s->sd_atrb & SA_LARGE)
                        fputs(" Large", lstfp);
                    if (pnt.s->sd_atrb & SA_CONF)
                        fputs(" Conformable", lstfp);
                    if (pnt.s->sd_atrb & SA_READ)
                        fputs(" Readable", lstfp);
                    if (pnt.s->sd_atrb & SA_WRITE)
                        fputs(" Writable", lstfp);
                    if (pnt.s->sd_flag & SF_EXTEND)
                        fputs(" Extendable", lstfp);
                }
                else
                    fputs(" 16-bit", lstfp);
                listnxl(0);
            } while ((pnt.s = pnt.s->sd_next) != NULL);
            listnxl(0);
        }
        if ((pnt.m = mshead) != NULL)
        {
            listline("Memory sections (msects):", 0);
            listline("Name                             Segment              "
                     "            Max       Mod       Addr      Attributes", 0);
            do
            {   fprintf(lstfp, "%-32.32s %-32.32s ", pnt.m->md_nba->mn_name,
                        pnt.m->md_sdb->sd_nba->sn_name);
                if (pnt.m->md_max == 0xFFFFFFFFL)
                    fputs("None      ", lstfp);
                else
                    fprintf(lstfp, "%8.8lX  ", pnt.m->md_max);
                if ((pnt.m->md_flag & MF_ADDR) || pnt.m->md_addr == 0xFFFFFFFFL)
                    fputs("None      ", lstfp);
                else
                    fprintf(lstfp, "%8.8lX  ", pnt.m->md_addr);
                if (!(pnt.m->md_flag & MF_ADDR) || pnt.m->md_addr ==
                        0xFFFFFFFFL)
                    fputs("None     ", lstfp);
                else
                    fprintf(lstfp, "%8.8lX ", pnt.m->md_addr);
                if (pnt.m->md_atrb & (MA_WRITE|MA_READ))
                {
                    if (pnt.m->md_atrb & MA_WRITE)
                        fputs(" Writable", lstfp);
                    if (pnt.m->md_atrb & MA_READ)
                        fputs(" Readable", lstfp);
                }
                else
                     fputs(" None", lstfp);
                listnxl(0);
            } while ((pnt.m = pnt.m->md_next) != NULL);
            listnxl(0);
        }
        if ((pnt.p = pshead) != NULL)
        {
            listline("Program sections (psects):", 0);
            listline("Name                             Memory section      "
                     "             Num Total size Loaded size Attributes", 0);
            do
            {   fprintf(lstfp, "%-32.32s %-32.32s %2.2X  %8.8lX   %8.8lX    %s",
                        pnt.p->pd_nba->pn_name, pnt.p->pd_mdb->md_nba->mn_name,
                        pnt.p->pd_psn, pnt.p->pd_tsize, pnt.p->pd_lsize,
                        pnt.p->pd_atrb & PA_OVER? "Overlayed": "None");
                listnxl(0);
            } while ((pnt.p = pnt.p->pd_next) != NULL);
            listnxl(0);
        }
    }
    listesym(undcnt, "? %d undefined symbol%c", SYF_UNDEF);
										// List undefined symbols
    listesym(mltcnt, "? %d multiply defined symbol%c", SYF_MULT);
										// List multiply defined symbols
    if (totalerr)						// Any errors?
    {									// Yes - tell him
		sprintf(albbuf, "? %d error%c detected", totalerr, (totalerr!=1) ?
				's':'\0');
		liststr(albbuf, 1);
    }
    else
    	liststr("No errors detected", FALSE); // No errors - tell him that
    if (lstfp)
		putc('\f',lstfp);				// Output final form-feed
}


//***********************************************
// Function: listtbl - List macro or symbol table
// Returned: Nothing
//***********************************************

static void listtbl(
    SY   *sym,				// Pointer to first symbol block
    char *head1,			// First table header line
    char *head2,			// Second table header line
    char *fill,				// String to fill start of continued cref
							//   listing lines
    int    mac)				// TRUE if macro table
{
    int  entcnt;			// Entires on line count
    int  stmp;
    char str[20];

    sym = heapsort(sym, (int (*)(void *a, void *b, void *data))compsym, 0);
    thead1 = head1;						// Sort the symbol table
    thead2 = head2;						// Remember the header lines
    if (lincnt < 5)						// Room to start table on this page?
		nextpage();						// No - start new page
    tblhead();							// Output table heading
    entcnt = 2;
    do
    {
		if (listtype == LT_CREF)		// Cref listing?
		{
			tblnxtl();					// Yes - start new line
			listent(sym, mac, 1);		// List the symbol
			entcnt = mac? (fputs("  ", lstfp), 7): 6;
			if (setref(sym))			// Setup to read the reference list
			{
				while ((stmp=nextref()) != '\0')
				{
					if (--entcnt < 0)	// Room for more on this line?
					{
						entcnt = mac? 6: 5;// No - start new line
						tblnxtl();
						fputs(fill, lstfp);
					}
					sprintf(str, "%5hd-%d%c", stmp&0x7FFF, nextref(),
							(stmp&0x8000)? '#': '\0');
					fprintf(lstfp, "%-14.14s", str);
				}
			}
		}
		else
		{
			if (--entcnt < 0)			// Not cref listing - will it fit?
			{
				tblnxtl();				// No - start new line
				entcnt = 1;
			}
			listent(sym, mac, entcnt);	// List the symbol
		}
    } while ((sym = sym->sy_hash) != NULL);
    listnxl(0);							// End the last line
    listnxl(0);							// Plus one blank line
}

//*******************************************************************
// Function: tblnxtl - Advance to next line for macro or symbol table
// Returned: Nothing
//*******************************************************************

void tblnxtl(void)

{
    listnxl(0);							// End previous line
    if (lincnt == 0)					// At end of page?
		tblhead();						// Yes - output table heading
}

//*************************************************************
// Function: tblhead - Output heading for macro or symbol table
// Returned: Nothing
//*************************************************************

void tblhead(void)

{
    chkhead();
    fputs(thead1, lstfp);
    listnxl(0);
    fputs(thead2, lstfp);
}


//***************************************************************
// Function: listent - List single entry in macro or symbol table
// Returned: Nothing
//***************************************************************

void listent(
    SY *sym,
    int mac,			// TRUE if macro table
    int endflag)		// 0 if last item on line

{
    int stemp;

    if (!mac)
    {
		if (sym->sy_flag & SYF_UNDEF)
			fputs("    undefined    ", lstfp);
		else if (sym->sy_flag & SYF_EXTERN)
			fputs("    external     ", lstfp);
		else if (sym->sy_flag & SYF_IMPORT)
			fputs("    imported     ", lstfp);
		else
		{
			stemp = sym->sy_flag;
			fprintf(lstfp, "%c%c%c %8.8lX ", (stemp & SYF_SUP)? 'S': ' ',
					(stemp & SYF_ENTRY)? 'E': ((stemp & SYF_INTERN)? 'I': ' '),
					(stemp & SYF_LABEL)? 'L': ((stemp & SYF_REG)? 'R': ' '),
					sym->sy_val.val);
			if (!(stemp & SYF_ABS) && sym->sy_psect != 0)
				fprintf(lstfp, "--%2.2X", sym->sy_psect);
			else if (sym->sy_psect != 0)
				fprintf(lstfp, "%4.4X", sym->sy_psect);
			else
				fputs("    ", lstfp);
		}
    }
    fprintf(lstfp, endflag? " %-32.32s  ": " %s", sym->sy_name);
}


//****************************************************************
// Function: listesym - List undefined or multiply defined symbols
// Returned: Nothing
//****************************************************************

void listesym(
    int   cnt,			// Number of bad symbols
    char *fmt,			// Format string for header output
    int   flag)			// Bit to test in sy_flag

{
    SY   *sp;
    int   num;
    int   ssc;
    char *ssp;

	static char *usyfmt[] = {" %-10.10s", " %-21.21s", " %-32.32s", "%-64.64s",
			"%-128.128s"};

    if (cnt)
    {
		sprintf(albbuf, fmt, cnt, cnt!=1?'s':'\0');
		liststr(albbuf, 1);
		num = 0;
		sp = (SY *)&symhead;
		while ((sp = sp->sy_hash) != NULL)
		{
			if (sp->sy_flag & flag)		// Is this one undefined?
			{
                ssc = SYMMAXSZ;			// Yes - see how long this symbol is
                ssp = sp->sy_name;
                do
                {   if (*ssp++ == '\0')
                        break;
                } while (--ssc > 0);
                ssc = (SYMMAXSZ - ssc)/11; // Get number of fields needed - 1
				if (num < (ssc+1))		// Need new line?
				{
					listnxl(1);			// Yes
					num = 7;
				}
                num -= ssc + 1;			// Reduce count
				sprintf(albbuf, usyfmt[ssc], sp->sy_name);
				liststr(albbuf, 1);
			}
		}
        listnxl(1);						// End last line
    }
}
