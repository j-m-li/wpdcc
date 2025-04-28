//*************************************
// Routines to do second pass for XLINK
//*************************************
// Written by John Goltz
//*************************************

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
#include <procarg.h>
#include <xossvc.h>
#include <xoserrmsg.h>
#include <xosrun.h>
#include "xlink.h"


static void relocout(MD *msd);
static int  cmprld(RD *a, RD *b);


//***********************************
// Function: passtwo - Do second pass
// Returned: Nothing
//***********************************

void passtwo(void)
{
    OB   *obj;					// Pointer to OBJ block
    SD   *sgd;
    MD   *msd;
    SY   *sym;
	char *pnt;
	int   cnt;
	long  saveoffset;
///	long  rtn;

    listgbl(undefcnt, "un", SYF_UNDEF);	// List undefined globals
    listgbl(mltcnt, "multiply ", SYF_MULT); // List multiply defined globals

///	if (opsymsz)						// Want a debugger symbol table?
///		stboffset = stbpsd->pd_offset + 8; // Yes - point to start

    if (needrun)						// Do we need output?
    {									// Yes - allocate output buffers
        if (outfsp == NULL)				// Was a name given?
            outfsp = usefif(outext);	// No - use name of first input file
		outopen(outfsp);				// Open the output file and allocate
										//   the output buffer
		if (auxloc == NULL)				// Make sure auxillary buffer
			auxloc = (uchar *)getspace(512L); //   is alocated
		clrbuf((long *)auxloc, 512);	// Clear it
		offset = 0;						// Output header at beginning of file
		lowfirst = (ptype < P_HIGH);	// Header is low byte first for Intel
										//   processors or high byte first
										//   for Motorolla processors
        putbyte(0xD7);					// ID bytes
        putbyte(0x22);
		putbyte(2);						// Format version number
        putbyte(ptype);					// Processor type
        putbyte(itype);					// Image type
		putbyte((fheadsz == sizeof(RUNHEAD)) ? 0 : sizeof(RUNHEAD));
										// Position of file version data
		putbyte(fheadsz - 6);			// Length of file header
        putbyte(seggbn);				// Number of segments
		putlong(stradr);				// Start address
        putbyte((strpsd) ? (strpsd->pd_msd->md_num) : 0);
        putbyte(0);
        putword(0);
        putlong(dbgadr);				// Debugger address
        putbyte((dbgpsd) ? (dbgpsd->pd_msd->md_num) : 0);
        putbyte(0);
        putword(0);
        putlong(stkadr);				// Initial stack pointer
        putbyte((stkpsd) ? (stkpsd->pd_msd->md_num) : 0);
        putbyte(0);
        putword(0);
        putlong((exportnum != 0) ? fheadsz + seggbn * SHDRSIZE +
				mscgbn * MHDRSIZE : 0);
        putlong(exportnum);
        importnumoffset = offset;		// Remember where import data goes
		if (fheadsz != sizeof(RUNHEAD))	// Do we want version data?
		{								// Yes
			offset = sizeof(RUNHEAD);	// It goes after the fixed part of the
			putword(majvrsn);			//   header
			putword(minvrsn);
			putlong(editnum);
			putbyte(vendorlen);
			cnt = vendorlen;
			pnt = vendor;
			while (--cnt >= 0)
				putbyte(*pnt++);
		}
        offset = fheadsz;
        if ((sgd=sgdhead) != NULL)
        {
            do
            {

                putbyte(SHDRSIZE);		// Length of segment header
                putbyte(sgd->sd_msn);	// Number of msects in segment
                putbyte(sgd->sd_flag);	// Segment status byte
                putbyte((sgd->sd_linked == NULL)? // Number of linked segment
                        0: sgd->sd_linked->sd_num);
                putbyte(sgd->sd_type);	// Segment type
                putbyte(sgd->sd_priv);	// Privilege level
                putword(sgd->sd_select); // Segment selector
                putlong((sgd->sd_flag & SA_ADDR)? sgd->sd_addr: sgd->sd_mod);
										// Address or modulus
                if ((msd=sgd->sd_head) != NULL)
                {						// Scan all msects for segment
                    do
                    {
                        putbyte(MHDRSIZE); // Length of msect header
                        putbyte(0);		// Not used
                        putbyte(msd->md_flag); // Msect status byte
                        putbyte(0);		// Not used
                        putbyte(msd->md_type); // Msect type
                        putbyte(msd->md_priv); // Privledge level
                        putword(0);
                        putlong((msd->md_flag & MA_ADDR) ?   // Address or
                                msd->md_addr : msd->md_mod); //   modulus?
                        putlong(msd->md_max); // Maximum segment needed
                        putlong(msd->md_tsize); // Total size of msect
                        putlong((msd->md_lsize)? msd->md_offset: 0);
										// Offset from start of file to data
                        putlong(msd->md_lsize); // Loaded size of msect
                        msd->md_rdhdros = offset;
										// Remember where to put data
										//   about the relocation data
                        putlong(0);		// Space for offset to relocation
										//   data
                        putlong(0);		// Space for size of relocation data
                    } while ((msd=msd->md_next) != NULL);
                }						// Advance to next msect in address
										//   space
            } while ((sgd=sgd->sd_next) != NULL); // Advance to next address
		}										  //   space
		if (exportnum != 0)
		{
			sym = exporthead;
			do
			{
				putbyte((sym->sy_flag & (SYF_ABS|SYF_ADDR|SYF_SUP)) | 0x03);
				putlong(sym->sy_val.v);
				if (sym->sy_flag & SYF_ADDR)
				{
					if (sym->sy_flag & SYF_ABS)
						putword(sym->sy_sel);
					else
						putbyte(sym->sy_psd->pd_msd->md_num);
				}
				outname(sym->sy_name+1);
			} while ((sym = sym->sy_export) != NULL);
		}
	}
	obj = firstobj;						// Start with first OBJ file
	while (obj)							// Process each OBJ file
	{
		filetwo(obj);
		obj = obj->obj_next;
	}
	if (outdd == 0)						// Finished here if not doing output
		return;
	offset = rdoffset;

	if (importnum != 0)					// Have any imported symbols?
	{									// Yes - output the imported symbol
		importnum = 0;					//   table
		importos = offset;
		sym = importhead;
		while (sym != NULL)
		{
			if (sym->sy_flag & SYF_USED) // Need this one?
			{							// Yes
				putbyte(0);				// Output header byte
				outname(sym->sy_name+1); // Output the symbol name
				sym->sy_val.v = ++importnum;
			}
			sym = sym->sy_next;
		}
	}
    if (opsymsz)						// Building symbol table in memory?
		domemsymtbl();					// Yes - generate in-memory symbol table
	sgd = sgdhead;						// Output relocation data
	while (sgd != NULL)
	{
		msd = sgd->sd_head;				// Scan all msects for segment
		while (msd != NULL)
		{
			printf("### REL: %8.8X %s\n", msd->md_rdhead,
					msd->md_sym->sy_name + 1);

		    if (msd->md_rdhead != NULL)	// Have any relocation data?
				relocout(msd);			// Yes - output it
			if (msd->md_rdoffset != 0)	// Fix up header data if necessary
			{
				saveoffset = offset;
				offset = msd->md_rdhdros;
				putlong((msd->md_rdcnt) ? msd->md_rdoffset : 0);
				putlong(msd->md_rdcnt);
				offset = saveoffset;
			}
			msd = msd->md_next;
		}								// Advance to next msect in segment
		sgd = sgd->sd_next;				// Advance to next segment
	}
	if (importnum)
	{
		offset = importnumoffset;
		putlong(importos);
		putlong(importnum);
	}
	outclose();							// Close the output file
}


//*************************************************************
// Function: relocout - Output relocation data for single msect
// Returned: Nothing
//*************************************************************

static void relocout(
    MD *msd)

{
    RD   *rld;
    ulong prevloc;
    ulong loc;
    int   type;

    msd->md_rdoffset = offset;
    if (msd->md_rdhead->rd_next != NULL) // Sort the relocation list
		msd->md_rdhead = heapsort(msd->md_rdhead, 
				(int (*)(void *, void *, void *))cmprld, NULL);
	prevloc = 0;
	rld = msd->md_rdhead;
	while (rld != NULL)
	{
		loc = rld->rd_loc - prevloc;
		prevloc = rld->rd_loc;
		type = rld->rd_type;			// Get relocation type and kind
		if (loc > 0xFFFFFF)				// Add in the length of the
			type |= 3;					//   delta offset field
		else if (loc > 0xFFFF)
			type |= 2;
		else if (loc > 0xFF)
			type |= 1;
		putbyte(type);
		if ((type&0x0C) == RT_SYMBOL)
			putvarval(rld->rd_data.s->sy_val.v);
		else if ((type&0x0C) != RT_NONE)
			putbyte(rld->rd_data.n);
		switch(type & 0x03)
		{								// Output delta offset field
		 case 0:
			putbyte(loc);
			break;

		 case 1:
			putword(loc);
			break;

		 case 2:
			if (lowfirst)
			{
				putbyte(loc);
				putword(loc>>8);
			}
			else
			{
				putword(loc>>8);
				putbyte(loc);
			}
			break;

		default:
			putlong(loc);
			break;
		}
		msd->md_rdcnt++;
        rld = rld->rd_next;
    }
}

//*********************************************************
// Function: cmprld - Compare relocation items for heapsort
// Returned: +1 if a > b, 0 if a == b, -1 if a < b
//*********************************************************

static int cmprld(
    RD *a,
    RD *b)

{
	return (a->rd_loc - b->rd_loc);
}

