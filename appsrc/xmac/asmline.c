//******************************************
// Function to assemble single line for XMAC
//******************************************
// Written by John Goltz
//******************************************

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
#include <xos.h>
#include "xmac.h"

// Link to private external data

extern PSTBL  psutbl[];			// Pseudo-op table
extern int    psusize;			// Number of entries in pseudo-op table
extern int    psuents;			// Size of one entry in pseudo-op table
extern OPTBL *opcpnt[];			// Opcode pointer table
extern int    opcsize[];		// Opcode size table

//*****************************************
// Function: asmline - Assemble single line
// Returned: Nothing
//*****************************************

void asmline(void)

{
	VL    val;
    SY   *sym;
	CL   *clp;
    union
    {   OPTBL *o;
        PSTBL *p;
    }     tbl;
    char *cpnt;
    int   cnt;
    char  chr;
    char  spastop;

	if (gccflg)
	{
		if ((chr = nxtnbc()) == '/' || chr == '#')
		{
			skiprest();
			return;
		}
		hldchar = chr;
	}
    while (TRUE)
    {
        if (ffflg)						// Do we have a form-feed?
		{
            ffflg = FALSE;				// Yes
			nextpage();					// Start new page
		}
		if (getsyq() == 0)				// Collect a symbol
		{
			if (ffflg)					// No symbol - form-feed?
			{
				eolflg = FALSE;			// Yes - ignore the funny "line"
				continue;
			}
			else
			{
				chkend(stopper); 		// Must be blank line
				return;
			}
		}								// If get here, have symbol on line
		if (gccflg)						// GCC mode allows periods in symbols
			persym();
		if (isspace(stopper))			// Stopped by whitespace?
		{
			stopper = nxtnbc();			// Yes - get next non-blank character
			spastop = TRUE;				// Remember stopped by space
		}
		else
			spastop = FALSE;
		if (stopper == ':')				// Is this a label?
		{
            chkpsect();					// Yes - must have psect defined
			sym = findsym(TRUE);		// Look for symbol in symbol table
			if (sym->sy_flag & SYF_UNDEF) // Is it undefined now?
			{
				sym->sy_flag &= ~SYF_UNDEF;
				sym->sy_flag |= SYF_LABEL|SYF_ADDR; // Yes - indicate this is
												    //   a label and address
				sym->sy_val.val = curpsd->pd_ofs; // Value is current address
				sym->sy_psect = curpsd->pd_psn;
			}
			else
			{
				if (!(sym->sy_flag & SYF_LABEL) // Not defined as a label?
						|| (sym->sy_flag & SYF_MULT)) // Or mult defined?
					errflag |= ERR_M;	// Yes - error
				else
				{
					if (sym->sy_val.val != curpsd->pd_ofs // Same value
							|| sym->sy_psect != curpsd->pd_psn)
					{
						errflag |= ERR_M; // No
						sym->sy_flag |= SYF_MULT;
					}
				}
			}   
			if (prmcnt <= 0)			// Is this in a parameter file?
				sym->sy_flag |= SYF_USED; // No - indicate symbol is used
			if ((chr=nxtnbc()) == ':')	// Internal symbol?
			{							// Yes
				if (!(sym->sy_flag & SYF_EXPORT)) // Also exported?
					sym->sy_flag |= SYF_INTERN; // No - make it internal
				chr = nxtnbc ();
			}
			if (chr == '!')				// Suppressed symbol?
				sym->sy_flag |= SYF_SUP; // Yes
			else
				hldchar = chr;			// No - put the character back
			clp = clget(CL_LABEL_SZ);
			clp->type = CLT_LABEL;
			clp->offset = curpsd->pd_ofs;
			clp->lsym = sym;
			clinsert(clp);
			continue;					// Process rest of line
		}

		// Here if symbol not followed by ":"

		if (stopper == '=')				// Is this an assignment?
		{								// Yes
		    if (symsize==1 && symbuf.nsym[0]=='$')
			{							// Setting location counter?
				exprsn(&val);			// Yes - get value
				if (!chkend(stopper))	// Must have end next
					return;
				if (val.flags & VLF_UNDEF)
				{
	                errflag |= ERR_X;	// Must be defined here
					return;
				}
				curpsd->pd_ofs = val.val; // Store new location counter
				if (curpsd->pd_tsize < curpsd->pd_ofs)
					curpsd->pd_tsize = curpsd->pd_ofs;
				clp = clget(CL_OFFSET_SZ);
				clp->type = (val.flags & VLF_VOLATILE) ? CLT_VOFFSET :
						CLT_FOFFSET;
				clinsert(clp);
				return;
			}
			sym = findsym(TRUE);		// Look for symbol in symbol table
			if (prmcnt <= 0)			// Yes - is this a parameter file?
				sym->sy_flag |= SYF_USED; // No - indicate symbol used
			if (!symbuf.lsym.ls_flag)	// Local symbol?
			{
				queerr();				// Yes - Q error
				return;
			}
			if ((sym->sy_flag & SYF_LABEL) || (sym->sy_type != SYT_SYM))
										// Already defined as label or as
			{							//   segment, msect or psect?
                errflag |= ERR_M;		// Yes - multiply defined
				skiprest();
				return;
			}
			chr = nxtnbc();				// Get next character
			if (chr == '=')				// Internal symbol?
			{							// Yes
				if (!(sym->sy_flag & SYF_EXPORT)) // Also exported?
					sym->sy_flag |= SYF_INTERN; // No - make it internal
				chr = nxtnbc();
			}
			if (chr == '!')				// Suppressed symbol?
				sym->sy_flag |= SYF_SUP;// Yes
			else
				hldchar = chr;			// No
			exprsn(&val);				// Get value of expression
			if (!chkend(stopper))		// Must have end next
				return;
			strsymvalue(sym, &val);
			clp = clget(CL_SYMBOL_SZ + xstrcnt);
			clp->vald.type = VALUE_4_U;
			clp->vald.value = val;
			clp->ssym = sym;

///			printf("### VL SYM: %08.8X\n", sym);

			if (prmcnt <= 0 && val.psect != 0)
				clp->vald.value.flags |= VLF_VOLATILE;
			if (clp->vald.value.flags & (VLF_UNDEF | VLF_VOLATILE))
										// Is the expression undefined or
			{							//   volatile?
				clp->vald.exprsn = getspace(xstrcnt + 1);
				memcpy(clp->vald.exprsn, xstrbufr, xstrcnt + 1);
			}
			clp->type = CLT_SYMBOL;
			clinsert(clp);
			return;
		}

		// Here if symbol not followed by "="

		if (!spastop)					// Stopped by space?
			persym();					// No - collect rest of symbol if it
				 						//   contained a period(s)
		hldchar = stopper;				// Put back the stopper
		cpnt = symbuf.nsym;				// Convert to all upper case
		cnt = symsize;
        do
        {
			*cpnt = toupper(*cpnt);
            ++cpnt;
        } while (--cnt > 0);

        if (symbuf.nsym[symsize-1] == '%') // Want auto-size?
            symbuf.nsym[symsize-1] = (cursgd && !(segatrb & SA_32BIT))?
                    'W': 'L';
		symbuf.nsym[0] |= 0x80;			// See if this is a macro name
		if ((sym = looksym()) != NULL)
		{
			invmac(sym);				// Yes - invoke the macro
			return;
		}
		symbuf.nsym[0] &= ~0x80;		// Not macro
		if (symbuf.nsym[0] == '.')		// Might this be a pseudo-op?
		{
			if ((symsize <= 9) && (tbl.p=(PSTBL *)srchtbl(symbuf.nsym+1,
					psutbl, psusize, 8, psuents)) != NULL)
				(*tbl.p->pt_dsp)();		// Dispatch to pseudo-op routine
			else
			{
                badopcode();
				return;
			}
		}
		else							// See if this is an opcode
		{
            if ((symsize<=7) && symbuf.nsym[0] >= 'A' && symbuf.nsym[0] <= 'Z'
					&& (tbl.o = (struct optbl *)srchtbl(&(symbuf.nsym[1]),
					opcpnt[symbuf.nsym[0] - 'A'], opcsize[symbuf.nsym[0] - 'A'],
					6, sizeof(OPTBL))) != NULL && (pbits & tbl.o->ot_proc))
            {
                chkpsect();				// Make sure have psect
				opdd.exprsn[0] = 0;
				opds.exprsn[0] = 0;
				clp = clget(CL_CODE_SZ);
				clp->type = CLT_FIXED;
				clp->byte0 = 0x100;
				clp->byte1 = 0x100;
				clp->byte2 = 0x100;
				clp->byte3 = 0x100;
				(*tbl.o->ot_dsp)(tbl.o, clp); // Dispatch to opcode routine

///				if (clp->errflag != 0)
///					printf("#### errflag = %X\n", clp->errflag);

				if (clp->size > 0)
					clinsert(clp);
            }
			else
			{
                badopcode();
				return;					// All finished with this line
			}
		}
		chkend(stopper);				// Must be at end of line now
		return;
    }
}


void strsymvalue(
	SY *sym,
	VL *vlp)

{
	if (vlp->kind == VLK_EXT || vlp->kind == VLK_SLX) // External?
	{
		errorx();						// Yes - error
		return;
	}
	sym->sy_flag &= ~(SYF_UNDEF|SYF_REG|SYF_ABS|SYF_ADDR);

	if (vlp->flags & VLF_UNDEF) 		// Was anything undefined?
		sym->sy_flag |= SYF_UNDEF;		// Yes - indicate this symbol is also
										//   undefined
	sym->sy_val.val = vlp->val;			// Store value
	sym->sy_psect = vlp->psect;

	if (vlp->kind == VLK_ABS)			// Absolute value?
		sym->sy_flag |= SYF_ABS; 		// Yes
	else if (vlp->kind == VLK_ABA) 		// Absolute address?
		sym->sy_flag |= SYF_ABS|SYF_ADDR; // Yes
	else if (vlp->kind == VLK_OFS) 		// Address?
		sym->sy_flag |= SYF_ADDR; 		// Yes
	else if (vlp->kind == VLK_REG)	 	// Register value?
	{
		if (sym->sy_flag & SYF_INTERN)	// Yes - cannot be internal
			errorx();
		else
			sym->sy_flag |= SYF_REG;
	}
	else if (vlp->type != VLT_VAL) 		// Normal value?
		errorx();						// No - error
}



//************************************
// Function: queerr - Indicate Q error
// Returned: FALSE
//************************************

int queerr(void)

{
    skiprest();
    errorq();
    return (FALSE);
}

//****************************************
// Function: badopcode - Report bad opcode
// Returned: Nothing
//****************************************

void badopcode(void)

{
    skiprest();							// Skip rest of line
    errflag |= ERR_O;					// Indicate bad opcode
}

//*******************************************************************
// Function: chkendx - Check for end of line, indicate Q error if not
// Returned: TRUE if end is next, FALSE if not end
//*******************************************************************

int chkendx(void)

{
    return (chkend(nxtnbc()));
}

//******************************************************************
// Function: chkend - Check for end of line, indicate Q error if not
// Returned: TRUE if end is next, FALSE if not end
//******************************************************************

int chkend(
    char chr)

{
    if (ifend(chr))
		return (TRUE);
    return (queerr());					// Not end - indicate Q error
}

//************************************************
// Function: ifend - See if end of line is next
// Returned: TRUE if end is next, FALSE if not end
//************************************************

int ifend(
    char chr)

{
    chr = nxtnb0(chr);					// Make sure have non-blank character
    if (chr == '\0')					// If real end of line
		return (TRUE);					// Finished
    while (chr == '}')					// Skip any right braces
		chr = nxtnbc();
    if (chr == ';')						// Comment next?
    {
        skiprest();						// Yes - eat the rest of the line
		return (TRUE);
    }
    else
        return (FALSE);					// Not end - return FALSE
}

//*******************************************
// Function: skiprest - Skip rest of the line
// Returned: Nothing
//*******************************************

void skiprest(void)

{
    while (!eolflg)						// Eat the rest of the line
		nxtnbc();
    stopper = '\0';
}

//****************************************************************
// Function: findsym - Search symbol table for reference to symbol
// Returned: Address of symbol table entry
//****************************************************************

SY *findsym(
    int def)					// TRUE if defining reference

{
    SY *sym;

    if ((sym=looksym()) == 0)			// Search symbol table
    {   sym = entersym();				// Not there - put it in the table
		sym->sy_flag = SYF_UNDEF;		// Indicate undefined
		sym->sy_val.val = 0;			// Initialize value
		sym->sy_psect = 0;
		sym->sy_ref = 0;				// No reference list now
        sym->sy_type = SYT_SYM;
    }
    if (prmcnt <= 0)					// Is this a parameter file?
    {
		sym->sy_flag |= SYF_USED;		// No - indicate symbol has been used
		refsym(sym,def);				// Make entry in reference list if
										//   necessary
    }
    return (sym);
}
