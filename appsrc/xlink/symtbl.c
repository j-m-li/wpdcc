//========================================================
//
// SYMTBL>C - Routines to generate symbol tables for XLINK
//
//========================================================
//
// Written by John Goltz
//
//========================================================

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
#include <xcmalloc.h>
#include <heapsort.h>
#include <xos.h>
#include <xossvc.h>
#include <procarg.h>
#include <xoserrmsg.h>
#include "xlink.h"

static void getsymvalues(SY *sym);
static void putsyminfile(char *name, SY *sym);
static void putsyminmem(char *name, SY *sym);


//*******************************************************************
// Function: domemsymtbl - Generate the in-memory symbol if necessary
// Returned: Nothing
//*******************************************************************

void domemsymtbl(void)

{
	PD **ptp;
	PD  *psd;
	OB  *obj;
	SY  *sym;
	long savoffset;
	int  cnt;
	int  scnt;
    char savfirst;

	savoffset = offset;
	savfirst = lowfirst;
	lowfirst = opsymord;				// Set byte order for symbol table

///	printf("### MEMSYM: md:%8.8X pd:%8.8X\n", stbmsd->md_offset,
///			stbpsd->pd_offset);

	offset = stbpsd->pd_offset + 8;
	scnt = 0;
	obj = firstobj;
	while (obj != NULL)
	{
		curmod = obj->obj_mdl;
		while (curmod != NULL)
		{
			putsyminmem(curmod->mb_name, NULL);
			scnt++;
			ptp = curmod->mb_psectnumtbl;
			cnt = curmod->mb_psectnummax + 1;
			while (--cnt >= 0)
			{
				psd = *ptp++;
				sym = psd->pd_symhead;
				while (sym != NULL)
				{
					if ((sym->sy_flag & SYF_UNDEF) == 0)
					{
						putsyminmem(sym->sy_name + 1, sym);
						scnt++;
					}
					sym = sym->sy_next;
				}
				sym = psd->pd_lochead = (SY *)heapsort(psd->pd_lochead,
						(int (*)(void *__p1,void *__p2,void *__p3))cmpsym, 
						psd);			// Sort local symbols
				while (sym != NULL)
				{
					putsyminmem(sym->sy_name + 1, sym);
					scnt++;
					sym = sym->sy_next;
				}
				psd = psd->pd_next;
			}
			curmod = curmod->mb_next;
		}
		obj = obj->obj_next;
	}
	offset = stbpsd->pd_offset;
	putlong(scnt);
	lowfirst = savfirst;
	offset = savoffset;
}


//******************************************************************
// Function: putsyminmem - Put entry into the in-memory symbol table
// Returned: Nothing
//******************************************************************

static void putsyminmem(
	char *name,
	SY   *sym)

{
	RD  *rld;
	MD  *symmsd;
	long symvalue;
	int  rltype;
	int  rlofs = 0;
	int  symselect;
	int  symflags;
	int  len;

	char *tmpp;

	tmpp = name;

	if (sym != NULL)
	{
		symflags = sym->sy_flag & (DBF_ADR | DBF_SUP);
		symvalue = sym->sy_val.v;
		if (sym->sy_flag & SYF_IMPORT)
			symflags |= DBF_IMP;
		if ((sym->sy_flag & SYF_LOCAL) == 0)
			symflags |= DBF_INT;
		if ((symmsd = sym->sy_psd->pd_msd) != NULL)
		{
			if ((symselect = symmsd->md_sgd->sd_select) == 0)
			{
				if (symmsd->md_flag & MA_ADDR)
				{
					rltype = RT_SELECTOR | RT_MSECT;
					rlofs = (opsymsz == 6) ? 2 : 4;
				}
				else
				{
					rltype = (opsymsz == 6) ? (RT_16ADDR | RT_MSECT) :
							(RT_32ADDR | RT_MSECT);
					rlofs = 0;
				}
			}
			else
			{
				if (symmsd->md_flag & MA_ADDR)
					rltype = -1;
				else
				{
					rltype = (opsymsz == 6) ? (RT_SELECTOR | RT_MSECT) :
							(RT_32ABSOFS | RT_MSECT);
					rlofs = 0;
				}
			}
		}
		else
		{
			symselect = sym->sy_sel;
			rltype = -1;
		}
	}
	else
	{
		symflags = DBF_MOD;
		symvalue = 0;
		symselect = 0;
		symmsd = NULL;
		rltype = -1;
	}

///	printf("### SYM: SO:%8.8X V:%4.4X:%8.8X N:%s\n", offset, symselect,
///			symvalue, name);

	len = strlen(name);
	putbyte(opsymwrd ? ((len + 1) & ~1) : len); // Store length of symbol
	putbyte(symflags);					// Store symbol flags
	if (rltype >= 0)
	{									// Is it relocatable?

///		printf("### reloc sym tbl %s into %s\n", tmpp,
///				stbmsd->md_sym->sy_name + 1);

		rld = (RD *)getspace(sizeof(RD)); // Yes
		rld->rd_magic = 'DLR*';			// Allocate relocation data block
		*stbmsd->md_rdlast = rld;
		stbmsd->md_rdlast = &rld->rd_next;
		rld->rd_next = NULL;
		rld->rd_sort = 0;
		rld->rd_loc = offset - stbmsd->md_offset + rlofs;
		rld->rd_type = rltype;
		rld->rd_data.n = symmsd->md_num;
	}
	if (opsymsz == 6)					// Store symbol value (16 or 32
		putword(symvalue);				//   bits)
	else
		putlong(symvalue);
	putword(symselect);					// Store selector value
	while (--len >= 0)					// Store symbol name
		putbyte(*name++);
	if (opsymwrd && (len & 1))
		putbyte(0);
	++stbtotal;
}


//*********************************************************************
// Function: dofilesymtbl - Generate the symbol table file if necessary
// Returned: Nothing
//*********************************************************************

// Note that since this function is called after output of the RUN file
//   is complete, we can reuse the RUN file buffer.

void dofilesymtbl(void)

{
	PD  **ptp;
	PD   *psd;
	SY   *sym;
	OB   *obj;
	char *fpnt;
	long  left;
	long  amount;
    long  rtn;
	int   cnt;
	int   symcnt;

	if (!needsym)
		return;
        if (symfsp == NULL)
            symfsp = usefif(symext);

	if ((symdd = svcIoOpen(XO_CREATE|XO_TRUNCA|XO_IN|XO_OUT, symfsp, NULL)) < 0)
										// Open the symbol table file
		femsg(prgname, symdd, symfsp);	// If error
	if ((OUTBUFBASE_ = realloc(OUTBUFBASE_, 0x1000)) < 0)
		femsg2(prgname, "Error initializing symbol file buffer", outbuftop,
				NULL);
	outbuftop = 0x1000;
	outbufsize = 12;
	((long *)OUTBUFBASE_)[0] = 0x040222D4; // Store header bytes
	offset = 12;
	outdd = -1;							// Make sure putbyte works!
	symcnt = 0;
	obj = firstobj;						// Start with first OBJ file
	while (obj != NULL)
	{
		curmod = obj->obj_mdl;
		while (curmod != NULL)
		{
			putsyminfile(curmod->mb_name, NULL);
			symcnt++;
			ptp = curmod->mb_psectnumtbl;
			cnt = curmod->mb_psectnummax + 1;
			while (--cnt >= 0)
			{
				psd = *ptp++;

//// SORT SYMBOLS IF NEED TO

				sym = psd->pd_symhead;
				while (sym != NULL)
				{
///					printf("### SYS: %02.2X %s %s\n", sym->sy_flag,
///							sym->sy_name + 1, (psd->pd_sym == NULL) ?
///							"**ABS**" : psd->pd_sym->sy_name + 1);


					if ((sym->sy_flag & SYF_UNDEF) == 0)
					{
						putsyminfile(sym->sy_name + 1, sym);
						symcnt++;
					}
					sym = sym->sy_next;
				}
				sym = psd->pd_lochead = (SY *)heapsort(psd->pd_lochead,
						(int (*)(void *__p1,void *__p2,void *__p3))cmpsym, 
						psd);				// Sort local symbols
				while (sym != NULL)
				{
					putsyminfile(sym->sy_name + 1, sym);
					symcnt++;
					sym = sym->sy_next;
				}
				psd = psd->pd_next;
			}
			curmod = curmod->mb_next;
		}
		obj = obj->obj_next;
	}
///	putbyte(0);						// Put a dummy module entry at the end
///	putbyte(DBF_MOD);
///	symcnt++;
	((long *)OUTBUFBASE_)[1] = offset - 12; // Store length of symbol table
	((long *)OUTBUFBASE_)[2] = symcnt;	// Store total number of symbols

	// Output the symbol table file

	fpnt = OUTBUFBASE_;
	left = outbufsize;
	do
	{
		if ((amount = left) > 0x100000)
			amount = 0x100000;
		if ((rtn = svcIoOutBlock(symdd, fpnt, amount)) < 0)
										// Write the output file
			femsg2(prgname, "Error writing symbol table file", rtn, symfsp);
		left -= amount;
		fpnt += amount;
	} while (left > 0);
    if ((rtn = svcIoClose(symdd, 0)) < 0) // Close the symbol table file
		femsg2(prgname, "Error closing symbol table file", rtn, outfsp);
}


//**************************************************************
// Function: putsyminfile - Put entry into the symbol table file
// Returned: Nothing
//**************************************************************

static void putsyminfile(
	char *name,
	SY   *sym)

{
	MD   *msd;
	SD   *sgd;
	char *cpnt;
	int   ccnt;
	long  symvalue;
	int   symselect;
	int   symflags;

	if (sym != NULL)
	{
		symflags = sym->sy_flag & (DBF_ADR|DBF_SUP);
		symvalue = sym->sy_val.v;

		if (sym->sy_flag & SYF_IMPORT)
			symflags |= DBF_IMP;
		if ((sym->sy_flag & SYF_LOCAL) == 0)
			symflags |= DBF_INT;
		if (symflags & DBF_ADR)
		{		
			if ((sym->sy_flag & SYF_ABS) == 0)
			{
				msd = sym->sy_psd->pd_msd;
				sgd = msd->md_sgd;
				if ((msd->md_flag & MA_ADDR) && sgd->sd_select != 0)
					symselect = sgd->sd_select;
				else
				{
					symselect = msd->md_num;
					symvalue -= msd->md_addr;
					symflags |= DBF_REL;
				}
			}
			else
				symselect = sym->sy_sel;
		}
		else
			symselect = 0;
	}
	else
	{
		symflags = DBF_MOD;
		symvalue = 0;
		symselect = 0;
	}

///	printf("### SYM: F:%04.4X/%04.4X V:%04.4X:%08.8X %s\n", (sym == NULL) ? 0 :
///			sym->sy_flag, symflags, symselect, symvalue, name);

	putbyte(symflags);				// Output symbol flags
	putlong(symvalue);				// Output value of symbol
	putword(symselect);
	cpnt = name;					// Output name of symbol
	ccnt = strlen(name);
	while (--ccnt > 0)
		putbyte(*cpnt++);
	putbyte(*cpnt | 0x80);
}
