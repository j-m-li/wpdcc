//***************************************************
// CODELIST.C Functions which deal with the code list
//***************************************************
// Written by John Goltz
//***************************************************

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
#include "xos.h"
#include "xoserrmsg.h"
#include "xcmalloc.h"
#include "xmac.h"


CL  *clhead;
CL **cllast = &clhead;
CL  *clvarhead;
CL **clvarlast = &clvarhead;


static long  fixonevalue(CL *clp, CLVL *val, int chktrunc);
static void  showvalue(char *label, CL *clp, CLVL *vlp);


void clstoretext()

{
	CL *clp;

	clp = clget(CL_TEXT_SZ + albcnt);
	clp->errflag = errflag & ~ERR_U;
	if (prmcnt > 0 || (lstmode != 2 && inspnt != NULL))
		clp->flags |= CLF_NLST;
	clp->linenum = lstseq;
	if ((clp->flags & CLF_NLST) == 0)
		lstseq++;
	clp->pagenum = pagnum;
	clp->lineno = lineno;
	clp->srcfile = srclvl ? srcfsp[srclvl] : thissrc->sb_fsp;
	memcpy(clp->source, albbuf, albcnt);
	clp->source[albcnt] = 0;
	clp->type = CLT_TEXT;
	clinsert(clp);
}


//******************************************
// Function: clget - Get a code list element
// Returned: Nothing
//******************************************

CL *clget(
	int size)

{
	CL *clp;

	clp = (CL *)sbrk(size);
	if (((long)clp) == -1)
		femsg2(prgname, "Cannot allocate memory for code list element",
				-errno, NULL);
	memset(clp, 0, size);
	return (clp);
}


//****************************************************
// Function: clinsert - Insert a CL into the code list
// Returned: Nothing
//****************************************************

void clinsert(
	CL *clp)

{
	*cllast = clp;
	cllast = &clp->next;
	clp->lsbnum = lsbnum;
	clp->psd = curpsd;
	if (clp->type != CLT_TEXT)
	{
		if (curpsd != NULL )
		{
			clp->offset = curpsd->pd_ofs;
			curpsd->pd_ofs += clp->size;
			if (curpsd->pd_tsize < curpsd->pd_ofs)
				curpsd->pd_tsize = curpsd->pd_ofs;
			if (clp->type != CLT_BLK && clp->type != CLT_MOD &&
					curpsd->pd_lsize < curpsd->pd_ofs)
				curpsd->pd_lsize = curpsd->pd_ofs;
			*curpsd->cllast = clp;
			curpsd->cllast = &clp->psnext;
		}
	}
}


//*************************************************************
// Function: clfixvalues - Fix up undefined and volatile values
// Returned: Final adjust amount
//*************************************************************

long clfixvalues(
	int jmpfixup,
	int chktrunc)

{
	PD   *pdp;
	CL   *clp;
	long  osv;
	long  adjust;
	long  total;
	long  tmp;
	long  rem;
///	long  forward;
///	uchar previous;

///	printf("######### FIXUP\n");

	adjusted = FALSE;
	total = 0;
	pdp = pshead;
	while (pdp != NULL)
	{
		adjust = 0;
		clp = pdp->clhead;
		while (clp != NULL)
		{
			lsbnum = clp->lsbnum;
///			previous = clp->offset;
			clp->offset += adjust;

///			printf("### TYPE: %d flags: %02X  %02X (%d,%d) offset %08X\n",
///					clp->type, clp->flags, clp->val1.value.flags,
///					clp->val1.type, clp->val2.type, clp->offset);

			switch (clp->type)
			{
			 case CLT_MOD:
				tmp = clp->offset - clp->mofs;
				if ((rem = tmp % clp->mval) != 0)
					tmp += (clp->mval - rem);
				tmp += clp->mofs;
				tmp -= clp->offset;
				if (tmp > clp->limit)
					tmp = 0;
				if (tmp != clp->size)
				{
					adjusted = TRUE;
					adjust += (tmp - clp->size);
					clp->size = tmp;
				}
				break;

			 case CLT_LABEL:
				clp->lsym->sy_val.val += adjust;
				break;

			 case CLT_SYMBOL:
				if (clp->vald.type != 0)
				{
					fixonevalue(clp, &clp->vald, chktrunc);
					strsymvalue(clp->ssym, &clp->vald.value);
				}
				break;

			 case CLT_VOFFSET:
			 case CLT_FOFFSET:
				clp->offset -= adjust;
				adjust = 0;
				break;

			 case CLT_DATA:
				if (clp->vald.type != 0)
					fixonevalue(clp, &clp->vald, chktrunc);
				break;

			 case CLT_FIXED:
				if (clp->val1.type != 0)
				{
					if ((tmp = fixonevalue(clp, &clp->val1, chktrunc)) != 0)
					{
						adjusted = TRUE;
						adjust += tmp;
					}
				}
				if (clp->val2.type != 0)
					fixonevalue(clp, &clp->val2, chktrunc);
				break;

			 case CLT_VAR:
				if (clp->val1.type != 0)
				{
///					forward = (clp->val1.value.val > previous);

					if ((tmp = fixonevalue(clp, &clp->val1, chktrunc)) != 0)
					{
						adjusted = TRUE;
						adjust += tmp;
					}
					if (jmpfixup && !(clp->val1.value.flags & VLF_UNDEF))
					{
						if (clp->val1.value.kind == VLK_EXT ||
								clp->val1.value.psect != clp->psd->pd_psn)
						{
							if ((tmp = expandjump(clp)) != 0)
							{
								adjusted = TRUE;
								adjust += tmp;
							}
						}
						else
						{
							osv = clp->val1.value.val - clp->offset - clp->size;
							if (clp->val1.forward)
								osv += adjust;
							if (osv >= 0x80 || osv < -0x80) // Is the jump out
															//   of range?
							{								// Yes - expand it
								if ((tmp = expandjump(clp)) != 0)
								{
									adjusted = TRUE;
									adjust += tmp;
								}
							}
						}
					}
					else
						clp->val1.forward = (clp->val1.value.val > clp->offset);
				}
				if (clp->val2.type != 0)
					fixonevalue(clp, &clp->val2, chktrunc);
				break;
			}
			clp = clp->psnext;
		}
		pdp->pd_tsize += adjust;
		pdp->pd_lsize += adjust;
		total += adjust;
		pdp = pdp->pd_next;
	}
	return (total);
}


//*****************************************
// Function: fixonevalue - Fix up one value
// Returned: Amount size adjusted
//*****************************************

static long fixonevalue(
	CL   *clp,
	CLVL *vlp,
	int   chktrunc)

{
	SY  *sym;
	long val;
	int  type;
	int  flags;

	if (vlp->value.flags & (VLF_UNDEF | VLF_VOLATILE))
	{
		if (clp->psd != NULL)
			clp->psd->pd_ofs = clp->offset;
		type = vlp->value.type;
		flags = vlp->value.flags;
		curpsd = clp->psd;
		hldchar = 0;
		exprsnwo(&vlp->value, vlp->exprsn);
		vlp->value.type = type;
		vlp->value.flags |= (flags & (VLF_REL | VLF_FAR | VLF_VOLATILE));
		if (vlp->value.flags & VLF_UNDEF)
		{
			if (dtxflg)					// Want to default to external?
			{
				vlp->value.kind = VLK_EXT; // Yes
				if ((sym = vlp->value.sym) != NULL)
				{
					sym->sy_flag &= ~SYF_UNDEF;
					sym->sy_flag |= SYF_EXTERN;
					sym->sy_psect = 0;
				}
			}
			else
				clp->errflag |= ERR_U;	// No - report the error
			vlp->value.flags &= ~VLF_UNDEF;
		}
	}
	if (chktrunc && clp->type != CLT_VAR && vlp->type < VALUE_4_S &&
			vlp->value.kind != VLK_EXT)
	{

		val = vlp->value.val;
		if (vlp->value.flags & VLF_REL)
			val -= (clp->offset + clp->size);

///		printf("### VAL: %08X offset: %08X size: %d\n", val, clp->offset,
///				clp->size);

		if (vlp->type < VALUE_2_S)		// Single byte value?
		{
			if (vlp->type == VALUE_1_S)
			{
				val &= 0xFFFFFF80;
				if (val != 0 && val != 0xFFFFFF80)
				{
					printf("### S T %08X %08X %08X %08X\n", val, vlp->value.val,
							clp->offset, clp->size);

					clp->errflag |= ERR_T;	// Report the error
				}
			}
			else
			{

				val &= 0xFFFFFF00;
				if (val != 0 && val != 0xFFFFFF00)
				{
					printf("### U T %08X\n", val);

					clp->errflag |= ERR_T;	// Report the error
				}
			}
		}
		else
		{
			if (vlp->type == VALUE_2_S)
			{
				val &= 0xFFFF8000;
				if (val != 0 && val != 0xFFFF8000)
					clp->errflag |= ERR_T;	// Report the error
			}
			else
			{
				val &= 0xFFFF0000;
				if (val != 0 && val != 0xFFFF0000)
					clp->errflag |= ERR_T;	// Report the error
			}
		}
	}
	return (0);
}


//*****************************************************
// Function: expandjump - Expand JMP or Jcc instruction
//				from 1 to 2 or 4 byte offset
// Returned: Amount expanded (bytes)
//*****************************************************

long expandjump(
	CL *clp)

{
	int  sizediff;
	char line[128];

	if (clp->val1.value.type == VLT_WORD) // Get size difference
	{
		sizediff = 1;
		clp->val1.type = VALUE_2_U;
	}
	else
	{
		sizediff = 3;
		clp->val1.type = VALUE_4_U;
	}
	if (clp->byte1 == 0xEB)				// Is this a JMP instruction?
	{									// Yes
		clp->size += sizediff;
		clp->byte1 = 0xE9;
		if (listtype == LT_DEBUG)
		{
			sprintf(line, "expanding JMP to %d bytes at %08.8X, offset=%08.8X",
					sizediff + 2, clp->offset, clp->val1.value.val);
			listline(line, 0);
		}
	}
	else								// If conditional jump (Jcc)
	{
		sizediff++;
		clp->size += sizediff;
		clp->byte2 = clp->byte1 + 0x10;
		clp->byte1 = 0x0F;
		if (listtype == LT_DEBUG)
		{
			sprintf(line, "expanding Jcc to %d bytes at %08.8X, offset=%08.8X",
					sizediff + 2, clp->offset, clp->val1.value.val);
			listline(line, 0);
		}
	}
	clp->type = CLT_FIXED;
	return (sizediff);
}


void clshow(void)

{
	CL  *clp;
	union
	{   char   *nsym;			// Normal symbol
	    struct locsym *lsym;	// Local symbol
	} name;
	char text[64];
	char line[256];

	listline("***** Code list:", 0);
	listnxl(0);
	clp = clhead;
	while (clp != NULL)
	{
		switch (clp->type)
		{
		 case CLT_LABEL:
			name.nsym = clp->lsym->sy_name;

			if (name.lsym->ls_flag == 0)
				sprintf(text, "$%X/%d", name.lsym->ls_val, name.lsym->ls_blk);
			else
				strcpy(text, name.nsym);
			sprintf(line, "%08.8X LABEL:              V   S:%-10s %08.8X",
					clp->offset, text, clp->lsym->sy_val);
			listline(line, 0);
			break;

		 case CLT_ASCII:
			listline("ASCII:", 0);
			break;

		 case CLT_DATA:				
			showvalue("DATA:", clp, &clp->vald);
			break;

		 case CLT_SYMBOL:
			showvalue("SYMBOL:", clp, &clp->vald);
			break;

		 case CLT_FIXED:
			showvalue("FIXED1:", clp, &clp->val1);
			showvalue("FIXED2:", clp, &clp->val2);
			break;

		 case CLT_VAR:
			showvalue("VAR1:", clp, &clp->val1);
			showvalue("VAR2:", clp, &clp->val2);
			break;
		}
		clp = clp->next;
	}
	listnxl(0);
}


//********************************************************
// Function: showvalue - Show a single value for debugging
// Returned: Nothing
//********************************************************

static void showvalue(
	char *label,
	CL   *clp,
	CLVL *vlp)

{
	char *pnt;
	char  line[256];

	if (vlp->type != 0)
	{
		pnt = line + sprintf(line, "%08.8X %-8s%2d %02.2X %2d%2d %c%c%c%c "
				"S:%-10s %08.8X |%s|", clp->offset, label, vlp->type,
				vlp->value.flags, vlp->value.type, vlp->value.kind,
				(vlp->value.flags & VLF_UNDEF) ? '?' : ' ',
				(vlp->value.flags & VLF_VOLATILE) ? 'V' : ' ',
				(vlp->value.flags & VLF_REL) ? 'R' : ' ',
				(vlp->value.flags & VLF_FAR) ? 'F' : ' ',
				(vlp->value.sym == NULL) ? "" : vlp->value.sym->sy_name,
				vlp->value.val, (vlp->exprsn == NULL) ? "" : vlp->exprsn);
		if (clp->type == CLT_SYMBOL)
			sprintf(pnt, " %s= %04.4X", clp->ssym->sy_name, clp->ssym->sy_flag);
		listline(line, 0);
	}
}
