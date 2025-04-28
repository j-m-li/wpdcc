//************************************
// Routines to do first pass for XLINK
//************************************
// Written by John Goltz
//************************************

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
#include <xos.h>
#include <xosrun.h>
#include <procarg.h>
#include "xlink.h"

// Allocate local data

///static long curphs;			// Used to hold phase address while setting
///								//   up psects


static void assignpsects(MD *msd);


//**********************************
// Function: passone - Do first pass
// Returned: Nothing
//**********************************

#if CFG_XLINK_MULTIPASS
 extern int		g_cUndefPrev;
 extern int		g_nPass1Lev;
#endif

void passone(void)
{
    OB  *obj;
    SY  *sym;
    SY  *wsym;
    SD  *sgd;
    MD  *msd;
    MD  *msd2;
    PD  *psd;
    SY **tblpnt;
	long top1;
	long top2;
    int  tblcnt;
    int  mscnum;

	stbsize = 10;
	modulenum = 1;
	if ((obj = firstobj) != NULL)		// Start with first OBJ file
	{
		do								// Process each OBJ file
		{
			fileone( obj );
		} while( (obj = obj->obj_next) != NULL );
	}

	// Note that while we do keep track of weak and lazy symbols seperately
	//   we treat them the same here!

	if (haveweak || havelazy)
	{
	    tblcnt = HASHSIZE;
	    tblpnt = hashtbl;
	    while (--tblcnt >= 0)			// Scan the hash table
	    {
			if ((sym=*tblpnt++) != NULL)
			{
				do
				{
					if ((sym->sy_flag & SYF_UNDEF) &&
							(wsym = sym->sy_weak) != NULL)
					{
///						printf("### weak sym: %s --> %s\n", sym->sy_name + 1,
///								wsym->sy_name + 1);

						sym->sy_flag = wsym->sy_flag;
						sym->sy_val.v = wsym->sy_val.v;
						sym->sy_sel = wsym->sy_sel;
						psd = wsym->sy_psd;
						sym->sy_psd = psd;
						if (psd->pd_symtail == NULL) // Link into psect symbol
							psd->pd_symhead = sym;   //   list
						else
							psd->pd_symtail->sy_next = sym;
						psd->pd_symtail = sym;
						sym->sy_next = NULL;

						if ((sym->sy_flag & SYF_UNDEF) == 0) // Still undefined?
						{
				            --undefcnt;	// No - reduce count
							if (opsymsz) // Loading debugger symbols?
								resvsym();
						}
					}
				} while ((sym=sym->sy_hash) != NULL);
			}
		}
	}
	if (opsymsz)						// Want a symbol table in memory?
    {
        strcpy(&symbuf[1], "_symbols_p"); // Yes - find the psect for it
        symsize = 11;
        symbuf[0] = SYM_PSCT;			// Indicate psect name
        if ((sym = looksym()) == NULL)	// Find psect symbol table entry
            nosyms("present");			// If not there
        else
        {
            stbpsd = sym->sy_val.p->pd_filehead;
            if (!(stbpsd->pd_flag & PA_OVER)) // It must be overlayed
                nosyms("overlayed");
            else
            {
                stbmsd = stbpsd->pd_msd;
                if (stbpsd->pd_lsize < stbsize)
                    stbpsd->pd_lsize = stbsize;
                if (stbpsd->pd_tsize < stbsize)
                    stbpsd->pd_tsize = stbsize;
            }
        }
    }

    if (exportnum)						// Have any exported symbols?
	{									// Yes - scan the list to see how much
		sym = exporthead;				//   space the exported symbol section
		do								//   will occupy
		{
			if (!(sym->sy_flag & SYF_ADDR))
				exporttotal += ((sym->sy_flag & SYF_ABS)? 2 : 1);

			exporttotal += ( 5 + strlen(sym->sy_name) );
		} while ((sym = sym->sy_export) != NULL);
    }

	// Get the length of the run file header (the vendor item is variable
	//   length, everything else is fixed length).

	fheadsz = (majvrsn != 0 || minvrsn != 0 || vendorlen > 0) ?
			(sizeof(RUNHEAD) + offsetof(VERDATA, vendor) +
			 ((vendorlen + 4) & 0xFFFFFFFC) - 1) : sizeof(RUNHEAD);	

    offset = fheadsz + seggbn * SHDRSIZE + mscgbn * MHDRSIZE + exporttotal;
    mscnum = 0;

	// Scan all segments and assign file offsets

    if ((sgd=sgdhead) != NULL)			// Scan all segments
    {
        do
        {
            sgd->sd_offset = offset;
            if ((msd=sgd->sd_head) != NULL)
            {
                do						// Scan all msects
                {
                    if (ptype >= P_Z80 && ptype <= P_80286)
                    {
                        if (msd->md_next)
                        {
                            fprintf(stderr, "? XLINK: More than 1 msect in"
                                    " segment %s for processor\n"
                                    "         which does not allow this"
                                    " in file %s\n", sgd->sd_sym->sy_name+1,
                                    curobj->obj_fsp);
                            exit(1);
                        }
                        if ((msd->md_flag & MA_ADDR) && msd->md_addr != 0)
                            warnaddr(msd);
                        msd->md_flag |= MA_ADDR;
                        msd->md_addr = 0;
                    }
                    msd->md_num = ++mscnum;
                    msd->md_offset = offset;
					assignpsects(msd);
                    offset += msd->md_lsize;
                } while ((msd=msd->md_next) != NULL);
            }
        } while ((sgd=sgd->sd_next) != NULL);
    }
    rdoffset = offset;					// Remember where to put relocation
										//   data
    if (strpsd)							// Relocate starting address if have
    {									//   one
		stradr += strpsd->pd_base;
		stradr -= strpsd->pd_msd->md_addr;
    }
    if (dbgpsd)							// Relocate debugger address if have
    {									//   one
        dbgadr += dbgpsd->pd_base;
		dbgadr -= dbgpsd->pd_msd->md_addr;
    }
    if (stkpsd)							// Relocate stack address if have one
    {
        stkadr += stkpsd->pd_base;
		stkadr -= stkpsd->pd_msd->md_addr;
    }

	// Scan all segments and report address conflicts

	if (checkflg)
	{
		sgd = sgdhead;
		while (sgd != NULL)
		{
			msd = sgd->sd_head;
			while (msd != NULL)
			{
				if (msd->md_addr != 0 && msd->md_tsize != 0)
				{
					msd2 = msd->md_next;
					while (msd2 != NULL)
					{
						if (msd2->md_addr != 0 && msd2->md_tsize != 0)
						{
							top1 = (msd->md_addr + msd->md_tsize) | 0xFFF;
							top2 = (msd2->md_addr + msd2->md_tsize) | 0xFFF;
							if ((msd2->md_addr >= msd->md_addr &&
									msd2->md_addr < top1) ||
									(msd->md_addr >= msd2->md_addr &&
									msd->md_addr < top2))
								addrconfl(msd, top1, msd2, top2);
						}
						msd2 = msd2->md_next;
					}
				}
				msd = msd->md_next;
			}
			sgd = sgd->sd_next;
		}
	}
}



//**************************************************
// Function: assignpsects - Assign offsets of psects
// Returned: Nothing
//**************************************************

// NOTE: This is a function instead of being in-line only for readability!

static void assignpsects(
	MD *msd)

{
	PD *psd;
	PD *psdf;
	SY *sym;

	msd->md_lsize = msd->md_tsize = 0;
	psd = msd->md_head;
	while (psd != NULL)
	{
		psd->pd_lsize = psd->pd_tsize = 0;
		if (psd->pd_modulus > 1)
			msd->md_tsize = ((msd->md_tsize + psd->pd_modulus - 1) /
					psd->pd_modulus) * psd->pd_modulus;
		psd->pd_offset = msd->md_offset + msd->md_tsize;
		psd->pd_base = msd->md_addr + msd->md_tsize;

///		printf("### %s %X %08X\n", psd->pd_sym->sy_name + 1, psd->pd_modulus,
///				psd->pd_base);

		psdf = psd->pd_filehead;
		while (psdf != NULL)
		{
			if (psdf->pd_magic != 'DSP*')
				BREAK();

			if (psdf->pd_modulus > 1)
				msd->md_tsize = ((msd->md_tsize + psdf->pd_modulus - 1) /
						psdf->pd_modulus) * psdf->pd_modulus;

			psdf->pd_offset = msd->md_offset + msd->md_tsize;
			psdf->pd_base = msd->md_addr + msd->md_tsize;
			sym = psdf->pd_symhead;
			while (sym != NULL)
			{
				sym->sy_val.v += psdf->pd_base; // Relocate symbol
				sym = sym->sy_next;
			}
			sym = psdf->pd_lochead;
			while (sym != NULL)
			{
				sym->sy_val.v += psdf->pd_base; // Relocate symbol
				sym = sym->sy_next;
			}
			if (psd->pd_flag & PA_OVER)
			{
				if (psd->pd_lsize < psdf->pd_lsize)
					psd->pd_lsize = psdf->pd_lsize;
				if (psd->pd_tsize < psdf->pd_tsize)
					psd->pd_tsize = psdf->pd_tsize;
			}
			else
			{
				if (psdf->pd_lsize > 0)
				{
					psd->pd_lsize = psd->pd_tsize + psdf->pd_lsize;
					msd->md_lsize = msd->md_tsize + psdf->pd_lsize;
				}
				psd->pd_tsize += psdf->pd_tsize;
				msd->md_tsize += psdf->pd_tsize;
			}

///			printf("#####   %X %08X\n", psdf->pd_modulus, psdf->pd_base);

			psdf = psdf->pd_next;
		}
		if (psd->pd_flag & PA_OVER)
		{
			if (psd->pd_lsize > 0)
				msd->md_lsize = msd->md_tsize + psd->pd_lsize;
			msd->md_tsize += psd->pd_tsize;
		}
		psd = psd->pd_next;
	}
}

#if 0
//**************************************************
// Function: assignpsects - Assign offsets of psects
// Returned: Nothing
//**************************************************

// NOTE: This is a function instead of being in-line only for readability!

static void assignpsects(
	MD *msd)

{
	PD *psd;
	PD *psdf;
	SY *sym;

	msd->md_lsize = msd->md_tsize = 0;
	psd = msd->md_head;
	while (psd != NULL)
	{
		psd->pd_lsize = psd->pd_tsize = 0;
		psd->pd_offset = msd->md_offset + msd->md_tsize;
		psd->pd_base = msd->md_addr + msd->md_tsize;
		psdf = psd->pd_filehead;
		while (psdf != NULL)
		{

			if (psdf->pd_magic != 'DSP*')
				BREAK();

			if (psdf->pd_modulus > 1)
				msd->md_tsize = ((msd->md_tsize + psdf->pd_modulus - 1) /
						psdf->pd_modulus) * psdf->pd_modulus;

			psdf->pd_offset = msd->md_offset + msd->md_tsize;
			psdf->pd_base = msd->md_addr + msd->md_tsize;
			sym = psdf->pd_symhead;
			while (sym != NULL)
			{
				sym->sy_val.v += psdf->pd_base; // Relocate symbol
				sym = sym->sy_next;
			}
			sym = psdf->pd_lochead;
			while (sym != NULL)
			{
				sym->sy_val.v += psdf->pd_base; // Relocate symbol
				sym = sym->sy_next;
			}
			if (psd->pd_flag & PA_OVER)
			{
				if (psd->pd_lsize < psdf->pd_lsize)
					psd->pd_lsize = psdf->pd_lsize;
				if (psd->pd_tsize < psdf->pd_tsize)
					psd->pd_tsize = psdf->pd_tsize;
			}
			else
			{
				if (psdf->pd_lsize > 0)
				{
					psd->pd_lsize = psd->pd_tsize + psdf->pd_lsize;
					msd->md_lsize = msd->md_tsize + psdf->pd_lsize;
				}
				psd->pd_tsize += psdf->pd_tsize;
				msd->md_tsize += psdf->pd_tsize;
			}
			psdf = psdf->pd_next;
		}
		if (psd->pd_flag & PA_OVER)
		{
			if (psd->pd_lsize > 0)
				msd->md_lsize = msd->md_tsize + psd->pd_lsize;
			msd->md_tsize += psd->pd_tsize;
		}
		psd = psd->pd_next;
	}
}
#endif
