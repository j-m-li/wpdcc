//*****************************************
// Functions to output binary data for XMAC
//*****************************************
// Written by John Goltz
//*****************************************

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
#include <errno.h>
#include <xos.h>
#include <xoserrmsg.h>
#include "xmac.h"

// Define value prefix bits which specify length of value

#define PX_0BV   0		// No value bytes
#define PX_1BV   1		// One value byte
#define PX_2BV   2		// Two value bytes
#define PX_4BV   3		// Four value bytes
#define PX_EX0   0		// Extend value with 0s
#define PX_EX1   4		// Extend value with 1s

static PD  *binpsd;
static long binofs;
static long sectcnt;

static void binbyte(long value);
static void bindata(int value);
static void binlong(long value);
static void binnumber(ulong value);
static void binsyms(int type, int fbit);
static void binval(long value, int prefix);
static void binval2(long value1, long value2, char prefix);
static void binword(long value);
static void binxsm(char *str);
static void chkadr(PD *psd, long offset);
static void extimpsect(int type, int flags);
static int  getprefix(long value);
static void putvalue(CL *clp, CLVL *val);
static void varvalue(int bits, long value);


//**********************************************
// Function: dobinary - Generate the object file
// Returned: Nothing
//**********************************************

void dobinary(void)

{
    int   cnt;
    union
    {   SD *s;
        MD *m;
        PD *p;
        char  *c;
    } pnt;
	CL   *clp;
	int   acnt;
	char *apnt;
    char *cp;
    char  chr;

	outfsp = defspec(outfsp, (char *)NULL, "OBJ");
										// Finish setting up default
	if ((outfp = fopen(outfsp, "wb")) == NULL) // Open the output file
		femsg(prgname, -errno, outfsp); // If error
	binbyte(0x84);						// Store file format byte
	binbyte(0x00);						// Indicate program module
	binbyte(0x81);						// Store module format byte
	if (ptype == 0)						// Was a processor specified?
		ptype = P_8086;					// No - assume 8086
	binbyte(ptype);						// Store processor type
	strsect(SC_START);					// Start the start of module section
	cnt = SYMMAXSZ;						// Put null at end of module name
	pnt.c = hdrbfr;						//   (this will destroy the
	do									//   header buffer, but this
	{   if (*pnt.c++ == ' ')			//   is OK since have already
		{								//   closed listing output)
			*(--pnt.c) = '\0';
			break;
		}
	} while (--cnt > 0);
	binxsm(hdrbfr);						// Output module name
	endsect();							// End the section

	if ((pnt.s = sghead) != NULL)		// Point to first segment data block
	{
		strsect(SC_SEG);				// Start the segment definition
										//  section
		do
		{   binval((long)(pnt.s->sd_select), pnt.s->sd_atrb);
										// Output attribute byte and segment
										//   selector value
			binval(pnt.s->sd_addr, pnt.s->sd_flag);
										// Output flag byte and address or
										//   modulus value
			if (pnt.s->sd_flag & SF_LINKED) // Have a linked segment?
				binbyte(pnt.s->sd_linked->sd_sgn); // Yes
			binbyte(pnt.s->sd_type);	// Output segment type
			binbyte(pnt.s->sd_priv);	// Output priviledge level
			binxsm(pnt.s->sd_nba->sn_name); // Output segment name
		} while ((pnt.s = pnt.s->sd_next) != NULL); // Loop for all
										//   segments
		endsect();						// End this section
	}
	if ((pnt.m = mshead) != NULL)		// Point to first msect data block
	{
		strsect(SC_MSECT);				// Start the msect definition
										//  section
		do
		{
			binval(pnt.m->md_max, pnt.m->md_atrb);
										// Output attribute byte and maximum
										//   space needed
			binval(pnt.m->md_addr, pnt.m->md_flag);
										// Output flag byte and address or
										//   modulus value
			binbyte(0);					// Output msect type (not used)
			binbyte(0);					// Output priviledge level (not used)
			binbyte(pnt.m->md_sgn);		// Output segment number
			binxsm(pnt.m->md_nba->mn_name); // Output msect name
		} while ((pnt.m = pnt.m->md_next) != NULL); // Loop for all msects
		endsect();						// End this section
	}

	if ((pnt.p = pshead) != NULL)		// Point to first psect data block
	{
		strsect(SC_PSECT);				// Start the psect definition
										//  section
		do
		{
			binval(pnt.p->pd_tsize, pnt.p->pd_atrb);
										// Output attribute byte and total
										//   size value
			binval(pnt.p->pd_lsize, 0); // Output loaded size
			if (pnt.p->pd_flag & PF_ADDR)
				binval2(pnt.p->pd_address, pnt.p->pd_mod, 0x08);
			else
				binval(pnt.p->pd_mod, 0); // Output flag byte and address and/or
										  //   modulus value
			binbyte(pnt.p->pd_msn);		// Output msect number
			binxsm(pnt.p->pd_nba->pn_name); // Output psect name
		} while ((pnt.p = pnt.p->pd_next) != NULL); // Loop for all psects
		endsect();						// End this section
	}
	if (symnum != 0)					// Do we have any symbols?
	{									// Yes
		symnum = 1;
		extimpsect(SC_EXTERN, SYF_EXTERN);
		extimpsect(SC_IMPORT, SYF_IMPORT);
	}
	dbkpnt = dbkbuf;					// Initialize data block pointer
	dbkcnt = 0;							// And count
	binpsd = NULL;						// Force initial addr. output
	strsect(SC_DATA);					// Start the data section

	clp = clhead;
	while (clp != NULL)
	{
		switch (clp->type)
		{
///		 case CLT_TEXT:
///		 case CLT_LABEL:
///			printf("### SRC: %s\n", clp->source);
///			break;

		 case CLT_MOD:
			if (clp->size > 0 && clp->fill != 0)
			{
				chkadr(clp->psd, clp->offset);
				acnt = clp->size;
				while (--acnt >= 0)
					bindata(clp->fill);
			}
			break;

		 case CLT_ASCII:
			chkadr(clp->psd, clp->offset);
			apnt = clp->adata;
			acnt = clp->size;
			while (--acnt >= 0)
			{
				bindata(*apnt++);
				binofs++;
			}
			break;

		 case CLT_DATA:				
			chkadr(clp->psd, clp->offset);
			if (clp->vald.type != 0)
				putvalue(clp, &clp->vald);
			binofs += clp->size;
			break;

		 case CLT_VAR:
		 case CLT_FIXED:
			chkadr(clp->psd, clp->offset);
			if (clp->repeat != 0)
				bindata(clp->repeat);
			if (clp->seg != 0)
				bindata(clp->seg);
			if (clp->flags & CLF_DS)
				bindata(0x66);
			if (clp->flags & CLF_AS)
				bindata(0x67);
			if ((clp->byte0 & 0xFF00) != 0x100)
				bindata((uchar)clp->byte0);
			if ((clp->byte1 & 0xFF00) != 0x100)
				bindata((uchar)clp->byte1);
			if ((clp->byte2 & 0xFF00) != 0x100)
				bindata((uchar)clp->byte2);
			if ((clp->byte3 & 0xFF00) != 0x100)
				bindata((uchar)clp->byte3);
			if (clp->val1.type != 0)
				putvalue(clp, &clp->val1);
			if (clp->val2.type != 0)
				putvalue(clp, &clp->val2);
			binofs += clp->size;
			break;
		}
		clp = clp->next;
		if (sectcnt > 40000)			// Is the section getting too big?
		{
			endsect();
			strsect(SC_DATA);
		}
	}
	wrtblk();							// Write out last data block
	endsect();							// End the data section
	if (gsymflg)
	{
		binsyms(SC_INTERN, SYF_INTERN); // Output internal symbol defs
		binsyms(SC_EXPORT, SYF_EXPORT); // Output exported symbol defs
	}
	if (lsymflg)						// Want local symbol definitions?
		binsyms(SC_LOCAL, 0);			// Yes


	if (strclp != NULL)					// Have starting address?
	{
		strsect(SC_STRADR);				// Yes - start the section
		binlong(strval.val);			// Store starting address
		binbyte(strval.psect);			// Followed by its psect number
		endsect();						// End the section
	}
	if (dbgclp != NULL)					// Have debugger address?
	{
		strsect(SC_DEBUG);				// Yes - start the section
		binlong(dbgval.val);			// Store debugger address
		binbyte(dbgval.psect);			// Followed by its psect number
		endsect();						// End the section
	}
	if (stkclp != NULL)					// Have stack address?
	{
		strsect(SC_STACK);				// Yes - start the section
		binlong(stkval.val);			// Store stack address
		binbyte(stkval.psect);			// Followed by its psect number
		endsect();						// End the section
	}
	while (reqhead)						// Have any .REQUIR names?
	{
		strsect(SC_FILREQ);				// Yes - start the section
		cp = reqhead->name;
		while ((chr = *cp++) != '\0')
			binbyte(chr);				// Output character
		endsect();
		reqhead = reqhead->next;
	}
	binbyte('\0');						// Ensure at least 1 zero byte at end
	if (fclose(outfp) < 0)				// Close the binary output file
		femsg(prgname, -errno, outfsp);
}


//*******************************************************
// Function: extimpsect - Output EXTERN or IMPORT section
// Returned: Nothing
//*******************************************************

static void extimpsect(
    int type,
    int flags)

{
    SY   *sp;
	char *pnt;
    int   scnt;

    scnt = 0;
	sp = symhead;
	while (sp != NULL)
	{
		if ((sp->sy_type == SYT_SYM) && (sp->sy_flag & flags))
		{								// Do we want this one?
			if (scnt == 0)				// Yes
				strsect(type);
			scnt++;
			sp->sy_symnum = symnum++; // Assign symbol number
			if (flags & SYF_IMPORT)
				binbyte((sp->sy_flag&SYF_ENTRY)? 0x40: 0);
			pnt = sp->sy_name;
			if (gccflg)
			{
				if (sp->sy_name[0] == '_')
					pnt++;
				else if (sp->sy_name[0] != '$')
					binbyte('$');
			}
			binxsm(pnt);				// Output symbol name
			if (scnt >= 200)			// Only allow 200 symbols in each
			{							//   section
				endsect();
				scnt = 0;
            } 
		}
		sp = sp->sy_hash;
    } 
    if (scnt != 0)
        endsect();
}


static struct
{	int  size;
	int  polop;
	long omask;
} vtbl[] =
{	{0, 0       , 0},
	{1, PO_STRSB, 0xFFFFFF80},	// VALUE_1_S = 1  - 1 byte signed value
	{1, PO_STRUB, 0xFFFFFF00},	// VALUE_1_U = 2  - 1 byte unsigned value
	{2, PO_STRSW, 0xFFFF8000},	// VALUE_2_S = 3  - 2 byte signed value
	{2, PO_STRUW, 0xFFFF0000},	// VALUE_2_U = 4  - 2 byte unsigned value
	{4, PO_STRSL, 0},			// VALUE_4_S = 5  - 4 byte signed value
	{4, PO_STRUL, 0},			// VALUE_4_U = 6  - 4 byte signed value
	{8, PO_STRQ , 0},			// VALUE_8   = 7  - 8 byte value
	{2, PO_STRAW, 0xFFFF0000},	// VALUE_2_A = 8  - 2 byte address
	{4, PO_STRAL, 0},			// VALUE_4_A = 9  - 4 byte address
	{8, PO_STRAQ, 0}			// VALUE_8_A = 10 - 8 byte address
};


//******************************************
// Function: putvalue - Output general value
// Returned: Nothing
//******************************************

static void putvalue(
	CL   *clp,
	CLVL *val)

{
	int  size;
	union
	{	long l;
		struct
		{	uchar b0;
			uchar b1;
			uchar b2;
			uchar b3;
		};
	}   value;


	value.l = val->value.val;
	size = vtbl[val->type].size;

///	printf("### putvalue: VT:%d T:%d K:%d V:%08.8X P:%04.4X %08.8X\n", val->type,
///			val->value.type, val->value.kind, val->value.val,
///			val->value.psect, clp);

	if ((val->value.flags & VLF_REL) && (val->value.sym == NULL ||
			(val->value.sym->sy_flag & (SYF_EXTERN | SYF_IMPORT))))
		value.l -= (clp->offset + clp->size);

	if (val->value.kind == VLK_ABS || val->value.kind == VLK_ABA ||
			((val->value.flags & VLF_REL) && val->value.psect ==
			clp->psd->pd_psn))
	{
		bindata(value.b0);
		if (size >= 2)
		{
			bindata(value.b1);
			if (size >= 4)
			{
				bindata(value.b2);
				bindata(value.b3);
			}
			if (size == 8)
			{
				bindata(0);
				bindata(0);
				bindata(0);
				bindata(0);
			}
		}

		if (val->value.flags & VLF_FAR)
		{
			bindata(val->value.psect);
			bindata(val->value.psect >> 8);
		}
	}
	else if (val->value.kind == VLK_SEG)
	{
		bindata(0);						// THIS SEEMS WRONG!!!!!
		bindata(0);
	}
	else
	{
		// Here if need a Polish string for this value

///		printf("### psect:%d  kind:%d  val:%04X\n", val->value.psect,
///				val->value.kind, val->value.val);
///		if (val->value.sym != NULL)
///			printf("###   sym:%s val:%04X\n", val->value.sym->sy_name);
///		else
///			printf("###   NO SYM!\n");

	    wrtblk();						// Write out current data block
		if (val->value.flags & VLF_REL) // Relative value?
			val->value.val -= size;		// Yes - adjust for size of operand
		if (val->value.kind == VLK_ABS && (val->value.flags & VLF_REL))
										// Absolute relative value?
			binval(val->value.val, PO_PVALREL); // Yes
		else if (val->value.kind == VLK_EXT) // No - external?
    	{	   							// Yes
			binval(val->value.val, (val->value.flags & VLF_REL) ?
					PO_POFSRELSYM : PO_POFSSYM);

///			if (val->value.sym->sy_symnum < 1 ||
///					val->value.sym->sy_symnum > 700)
///				printf("### bad sym num %d for %s\n", val->value.sym->sy_symnum,
///						val->value.sym->sy_name);

			binnumber(val->value.sym->sy_symnum);
    	}
		else if (val->value.kind < VLK_MSC) // Real value?
	    {
    	    if (val->value.psect == binpsd->pd_psn)

										// Relocatable - for current psect?
										//   (note that this cannot be
										//   relative since self-relative
										//   references have already been
										//   resolved)
				binval(val->value.val,	PO_POFSCP); // Yes
			else						// No - some other psect
			{   
				binval(val->value.val, (val->value.flags & VLF_REL) ?
						PO_POFSRELAP : PO_POFSAP);
				binbyte(val->value.psect); // Output psect number
			}
		}
		else if (val->value.kind == VLK_MSC) // Msect offset?
		{
			binbyte(PO_POFSAM);			// Yes
			binbyte(val->value.psect);
			if (val->value.val)			// Any offset?
			{
				binval(val->value.val, PO_PVAL); // Yes - push it
				binbyte(PO_ADD);		// Add offset
			}
		}
		else if (val->value.kind == VLK_SEL) // Selector?
		{
			binbyte(PO_PSELAS);			// Yes
			binbyte(val->value.psect);
		}
		else							// Must be external selector
		{
			binbyte(PO_PSELSYM);		// Yes
			binnumber(val->value.sym->sy_symnum);
		}
		binbyte(vtbl[val->type].polop);	// Output store operator


/*
		if (val->value.flags & VLF_FAR) // Is this a far value?
		{
			if (val->value.kind == VLK_ABA)
			{							// Want selector of absolute address?
				bindata(val->value.psect); // Yes
				bindata(val->value.psect>>8);
			}
			else if (val->value.kind >= VLK_SEL || val->value.kind == VLK_ABS)
			{							// Want selector of selector(!) or
										//   of absolute value?
			    errflag |= ERR_NA;		// Yes - complain
				bindata(0);				// And store 0
				bindata(0);
			}
			else
			{
				wrtblk();
				if (val->value.kind == VLK_EXT) // External value?
				{
					binbyte(PO_PSELSYM); // Yes
					binnumber(val->value.sym->sy_symnum);
				}
				else					// Not external
				{          
					if (val->value.psect == clp->psd->pd_psn)
										// Current psect?
						binbyte(PO_PSELCP);	// Yes
					else
					{
						binbyte(PO_PSELAP);	// No
						binbyte(val->value.psect);
					}
				}
				binbyte(PO_STRUW);		// Output store operator
			}
		}
*/
	}
}


//*********************************************************
// Function: chkadr - Check if address is needed in the OBJ
//	file and output it if so 
// Returned: Nothing
//*********************************************************

static void chkadr(
	PD  *psd,
	long offset)

{
    if (psd != binpsd || offset != binofs)
    {   
		wrtblk();						// Need address - output current
										//  data block
		binpsd = psd;
		binofs = offset;
		binval(binofs, 0x78);			// Output address
		binbyte(binpsd->pd_psn);		// Output psect number
    }
}


//****************************************************************
// Function: bindata - Store data byte into the current data block
// Returned: Nothing
//****************************************************************

static void bindata(
	int value)
{
    if (dbkcnt >= 119)					// Is the current data block full?
		wrtblk();						// Yes - write it out
    ++dbkcnt;							// Bump count
    *dbkpnt++ = value;					// Store value
}


//*************************************************
// Function: wrtblk - Output the current data block
// Returned: Nothing
//*************************************************

void wrtblk(void)

{
    char *pnt;
    char  byte;

    if (dbkcnt)							// Anything in the data block?
    {   
		byte = dbkcnt;
		binbyte(byte);					// Output header byte
		pnt = dbkbuf;					// Point to buffer
		dbkpnt = pnt;					// Reset store pointer
		do
		{
			binbyte(*pnt++);			// Output data byte
		} while (--dbkcnt > 0);
    }
}


//*************************************************
// Function: binsyms - Output all defined global or
//				local symbols to the OBJ file
// Returned: Nothing
//*************************************************

static void binsyms(
    int type,			// Section type
    int fbit)			// Value for SYF_INTERN bit
{
    SY   *sym;
	char *pnt;
    int   bits;
    char  head;

	head = FALSE;
	if ((sym = symhead) != NULL)		// Point to start of symbol list
	{
		do
		{
			if (!(sym->sy_flag & SYF_UNDEF) && 
					!(sym->sy_flag & (SYF_EXTERN|SYF_IMPORT)) &&
					(sym->sy_flag & (SYF_INTERN|SYF_EXPORT)) == fbit &&
					(!gccflg || type != SC_LOCAL || savesymflg ||
						sym->sy_name[0] != 'L'))
			{							// Is this one we want?
				if (!head)				// Have we started the section?
				{
					strsect(type);		// No - do so now
					head = TRUE;
				}
				bits = sym->sy_flag & (SYF_ABS|SYF_ENTRY|SYF_ADDR|SYF_SUP);
				if (!(bits & SYF_ADDR))	// Have an address?
					bits |= SYF_ABS;	// No - always absolute!
				binval(sym->sy_val.val, bits);
                if (bits & SYF_ABS)		// Absolute?
                {
                    if (bits & SYF_ADDR) // Yes - address?
                        binword(sym->sy_psect); // Yes - output selector
                }
                else
                    binbyte(sym->sy_psect); // No - output psect number
				pnt = sym->sy_name;
				if (gccflg)
				{
					if (sym->sy_name[0] == '_')
						pnt++;
					else if (sym->sy_name[0] != '$')
						binbyte('$');
				}
                binxsm(pnt);			// Output name
				if (sectcnt > 58000)
				{
					endsect();
					head = FALSE;
				}
            }
        } while ((sym = sym->sy_hash) != NULL);	// Loop for all symbols
        if (head)						// Did we do any output?
            endsect();					// Yes - end the section
    }
}


//*****************************************************************
// Function: binval - Store variable length value into the OBJ file
// Returned: Nothing
//*****************************************************************

static void binval(
    long value,			// Value to output
    int  prefix)		// High order 5 bits for prefix byte

{
	int bits;

	bits = getprefix(value);
	binbyte((prefix & 0x0F8) | bits);
	varvalue(bits, value);
}


static void binval2(
	long value1,
	long value2,
	char prefix)

{

	int bits1;
	int bits2;

	bits1 = getprefix(value1);
	bits2 = getprefix(value2);
	binbyte((prefix & 0x88) | bits1 | (bits2 << 4));
	varvalue(bits1, value1);
	varvalue(bits2, value2);
}



static int getprefix(
	long value)

{
    if ((value & 0xFFFF0000L) == 0)		// Is the high word of the value 0?
    {
		if ((value & 0xFF00L) == 0)		// Yes - is the high byte of the low
		{								//   word of the value 0?
			if (value == 0)				// Yes - is the entire value 0?
				return (PX_EX0|PX_0BV);	// Yes - no value needed
			return (PX_EX0|PX_1BV);		// No - need single byte value
		}
		return (PX_EX0|PX_2BV);			// Low word not zero - need 2 byte
										//   value
	}
    if ((value & 0xFFFF0000L) == 0xFFFF0000L) // Is the high word
    {										  //   0xFFFF?
		if ((value & 0xFF00L) == 0xFF00L) // Is the high byte of the
		{								  //   low word 0xFF?
			if (value  == 0xFFFFFFFFL)	// Is the entire value
										//   0xFFFFFFFF?
				return (PX_EX1|PX_0BV);	// Yes - no value is needed
			return (PX_EX1|PX_1BV);		// No - need 1 byte value
		}
		return (PX_EX1|PX_2BV);			// High byte of low word not
	}				     				//   0xFF need 2 byte value
	return (PX_4BV);					// Need full 4 byte value
}


static void varvalue(
	int  bits,
	long value)

{
	switch (bits & 0x03)
	{
	 case 0:							// No value is needed
		break;

	 case 1:							// One byte value is needed
		binbyte(value);
		break;

	 case 2:							// Two byte value is needed
		binword(value);
		break;

	 case 3:
		binlong(value);					// Four byte value is needed
		break;
	}
}


//***************************************************************
// Function: binxsm -  Store symbol name string into the OBJ file
// Returned: Nothing
//***************************************************************

static void binxsm(
    char *str)
{
    int  cnt;
    char prev;
    char this;

    cnt = SYMMAXSZ -1;
    prev = *str++;
    do
    {   if ((this = *str++) != '\0')	// Get next character
	{
	    binbyte(prev & 0x7F);			// Output previous character
	    prev = this;
	}
	else
	    break;							// If at end
    } while (--cnt > 0);
    binbyte(prev | 0x80);				// Output last character
}


//*******************************************
// Function: strsect - Start OBJ file section
// Returned: Nothing
//*******************************************

void strsect(
    char type)

{
    binbyte(type);						// Output section type
    sectsize = ftell(outfp);			// Save position for section size
    binbyte('\0');						// Reserve space for size (16 bits)
    binbyte('\0');
	sectcnt = 0;
}


//*****************************************
// Function: endsect - End OBJ file section
// Returned: Nothing
//*****************************************

void endsect(void)

{
    long place;
    int  size;

    place = ftell(outfp);				// Save current position in file
    if (fseek(outfp, sectsize, 0) < 0)	// Set to place to put section size
		femsg(prgname, -errno, outfsp);
    size = place - sectsize - 2;		// Calculate size of section
    binword(size);						// Store size in section header
    if (fseek(outfp, place, 0) < 0)		// Restore position in file
		femsg(prgname, -errno, outfsp);
}


//********************************************************
// Function: binlong - Output 32 bit value to the OBJ file
// Returned: Nothing
//********************************************************

static void binlong(
	long value)

{
    binword(value);						// Output low order word
    binword(value >> 16);				// Output high order word
}

//********************************************************
// Function: binword - Output 16 bit value to the OBJ file
// Returned: Nothing
//********************************************************

static void binword(
    long value)

{
    binbyte(value);						// Output low order byte
    binbyte(value >> 8);				// Output high order byte
}

//*******************************************************
// Function: binbyte - Output 8 bit value to the OBJ file
// Returned: Nothing
//*******************************************************

static void binbyte(
    long value)

{
    if (putc(value, outfp) < 0)			// Output the byte
		femsg(prgname, -errno, outfsp);	// If error on output
	sectcnt++;
}


//****************************************************************************
// Function: binnumber - Output number (variable length) value to the OBJ file
// Returned: Nothing
//****************************************************************************

static void binnumber(
    ulong value)

{
    if (value <= 0x7FL)
        binbyte((char)value);
    else if (value <= 0x3FFFL)
    {
        binbyte(((value>>8) & 0xFF) | 0x80);
        binbyte(value & 0xFF);
    }
    else if (value <= 0x1FFFFFL)
    {
        binbyte(((value>>16) & 0xFF) | 0xC0);
        binbyte((value>>8) & 0xFF);
        binbyte(value & 0xFF);
    }
    else
    {
        binbyte(((value>>24) & 0xFF) | 0xE0);
        binbyte((value>>16) & 0xFF);
        binbyte((value>>8) & 0xFF);
        binbyte(value & 0xFF);
    }
}
