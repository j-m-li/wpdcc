//*********************************
// LIST.C Listing routines for XMAC
//*********************************
// Written by John Goltz
//*********************************

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
#include <time.h>
#include <ctype.h>
#include <errno.h>
#include <xos.h>
#include <xosstr.h>
#include <xostime.h>
#include <xossvc.h>
#include <xoserrmsg.h>
#include "xmac.h"


static CL   *clp;
static CL   *first;
static char *src;
static char *pnt;
static long  offset;
static long  errors;
static long  lastnum;
static uchar needbin;
static uchar listthis;
static char  blank[] = "";


static void  doline(char *line, char *bufr);
static char *geterrorltrs(long errflag);
static char *listvalue(char *pnt, CL *clp, CLVL *val);


//***************************************************
// Function: listinit - Initialize for listing output
// Returned: Nothing
//***************************************************

extern int fisctrm(FILE *);

void listinit(void)

{
    time_d timeloc;

    if (lstfsp)							// Do we want listing output?
    {
        svcSysDateTime(2, &timeloc);	// Get current data and time
        ddt2str(timebfr, "%H:%m %W %D-%3n-%y", (time_dz *)&timeloc);
										// Change to string
        if (*lstfsp == '\0')
            lstfp = stdout;
        else
        {
			if ((lstfp = fopen(lstfsp, "w")) == NULL)
			femsg(prgname, -errno, lstfsp);
        }
        lststd = fisctrm(lstfp);		// Remember if listing is to console
    }
    albcnt = 0;							// Nothing to list now
    lincnt = 0;
}


//*************************************************************
// Function: dolisting - Generate listing and/or error messages
// Returned: Nothing
//*************************************************************

void dolisting()

{
	CL   *pclp;
	char *apnt;
	long  tmp;
	int   acnt;
	char  line[600];
	char  bufr[512];
	char  bufr1[400];
	char  bufr2[400];

	first = NULL;
	src = blank;
	pnt = bufr;
	lastnum = -1;
	offset = 0;
	totalerr = 0;
	errors = 0;
	tmp = 0;
	needbin = FALSE;
	clp = clhead;
	while (clp != NULL)
	{
		if (clp->type != CLT_TEXT)
		{
			if (first == NULL)
			{
				first = clp;
				offset = clp->offset;
				pnt = bufr;
			}
		}
		else
		{
			errors = clp->errflag;
			src = clp->source;
			lsbnum = clp->lsbnum;
			if (first != NULL)
			{

				if (lstfp == NULL || clp->flags & CLF_NLST)
				{						// If not listing, determine quickly
					pclp = first;		//   if we need to list this line
					while (pclp != clp && errors == 0)
					{
						errors |= pclp->errflag;
						pclp = pclp->next;
					}
				}

				listthis = (lstfp != NULL && !(clp->flags & CLF_NLST));

				if ((lstfp != NULL && (clp->flags & CLF_NLST) == 0) ||
						errors != 0)
				{
					offset = first->offset;
					while (first != clp)
					{
						errors |= first->errflag;
						switch (first->type)
						{
						 case CLT_VOFFSET:
						 case CLT_FOFFSET:
							needbin = TRUE;
							break;

						 case CLT_LABEL:
							needbin = TRUE;
							break;

						 case CLT_ASCII:
							if (listthis || errors)
							{
								apnt = first->adata;
								acnt = first->size;
								while (--acnt >= 0)
								{
									tmp++;
									pnt += sprintf(pnt, "%02.2X ", *apnt++);
									if (pnt > (bufr + 26))
									{
										needbin = TRUE;
										doline(line, bufr);
										offset += tmp;
										tmp = 0;
									}
								}
								needbin = TRUE;
							}
							break;

						 case CLT_MOD:
							if (listthis || errors)
							{
								if (first->fill == 0)
									offset += first->size;
								else
								{
									acnt = first->size;
									while (--acnt >= 0)
									{
										tmp++;
										pnt += sprintf(pnt, "%02.2X ",
												(uchar)first->fill);
										if (pnt > (bufr + 26))
										{
											needbin = TRUE;
											doline(line, bufr);
											offset += tmp;
											tmp = 0;
										}
									}
								}
							}
						 case CLT_BLK:
							needbin = TRUE;
							break;

						 case CLT_DATA:
							if (pnt > (bufr + 26))
							{
								doline(line, bufr);
								offset += tmp;
								tmp = 0;
							}
							tmp += first->size;
							if (first->vald.type != 0)
								pnt = listvalue(pnt, first, &first->vald);
							needbin = TRUE;
							break;

						 case CLT_SYMBOL:
							if (first->vald.type != 0)
								pnt = listvalue(pnt, first, &first->vald);
							break;

						 case CLT_FIXED:
						 case CLT_VAR:
							if (first->val1.type != 0)
								listvalue(bufr1, first, &first->val1);
							else
								bufr1[0] = 0;
							if (first->val2.type != 0)
								listvalue(bufr2, first, &first->val2);
							else
								bufr2[0] = 0;
							if (listthis || errors)
							{
								if (first->repeat != 0)
									pnt += sprintf(pnt, "%02.2X ", first->repeat);
								if (first->seg != 0)
									pnt += sprintf(pnt, "%02.2X ", first->seg);
								if (first->flags & CLF_DS)
									pnt += sprintf(pnt, "66 ");
								if (first->flags & CLF_AS)
									pnt += sprintf(pnt, "67 ");
								if ((first->byte0 & 0xFF00) != 0x100)
									pnt += sprintf(pnt, "%02.2X ", first->byte0);
								if ((first->byte1 & 0xFF00) != 0x100)
									pnt += sprintf(pnt, "%02.2X ", first->byte1);
								if ((first->byte2 & 0xFF00) != 0x100)
									pnt += sprintf(pnt, "%02.2X ", first->byte2);
								if ((first->byte3 & 0xFF00) != 0x100)
									pnt += sprintf(pnt, "%02.2X ", first->byte3);
								pnt = strmov(strmov(pnt, bufr1), bufr2);
							}
							needbin = TRUE;
							break;
						}
						first = first->next;
					}
					doline(line, bufr);
					tmp = 0;
				}
				src = blank;
			}
			else
			{
				if ((lstfp != NULL && (clp->flags & CLF_NLST) == 0) ||
						clp->errflag != 0)
				{
					if (clp->linenum < lastnum)
					{
						lincnt = -1;
						chkhead();
					}
					lastnum = clp->linenum;
					errors = clp->errflag;
					sprintf(line, "%-3s%3d%42s%s", geterrorltrs(errors),
							clp->linenum, "", src);
					chkhead();
					listline(line, errors);
					tmp = 0;
				}
			}
			if (errors != 0)
			{
				pnt = line + sprintf(line, "****** Error on page %d ",
						clp->pagenum);
				if (clp->lineno == -1)
					strcpy(pnt, "(end of source)");
				else
					sprintf(pnt, "(line %ld in file %s)", clp->lineno,
							clp->srcfile);
				if (!lststd)
					fprintf(stderr, "%s\n", line);
				if (lstfp)
				{
					listline(line, 0);
					listnxl(0);
				}
			}
			first = NULL;
			needbin = FALSE;
		}
		clp = clp->next;
	}
}


//****************************************
// Function: listvalue - List single value
// Returned: Updated pointer
//****************************************

static char *listvalue(
	char *pnt,
	CL   *clp,
	CLVL *val)

{
	long value;
	long tmp;

	value = val->value.val;
	if ((val->value.flags & VLF_REL) && (val->value.sym == NULL ||
			(val->value.sym->sy_flag & (SYF_EXTERN | SYF_IMPORT))))
		value -= (clp->offset + clp->size);
	switch (val->type)
	{
	 case VALUE_1_S:
		tmp = value & 0xFFFFFF80;
		if (tmp != 0 && tmp != 0xFFFFFF80)
			errors |= ERR_T;
		goto printbyte;

	 case VALUE_1_U:
		tmp = value & 0xFFFFFF00;
		if (tmp != 0 && tmp != 0xFFFFFF00)
			errors |= ERR_T;
	 printbyte:
		if (listthis || errors)
			pnt += sprintf(pnt, "%02.2X", (uchar)value);
		break;

	 case VALUE_2_S:
	 case VALUE_2_U:
		if (listthis || errors)
			pnt += sprintf(pnt, "%04.4X", (ushort)value);
		break;

	 case VALUE_4_S:
	 case VALUE_4_U:
		if (listthis || errors)
			pnt += sprintf(pnt, "%08.8X", value);
		break;

	 case VALUE_8:
		if (listthis || errors)
			pnt += sprintf(pnt, "00000000%08.8X", value);
		break;

	 case VALUE_2_A:
		if (listthis || errors)
			pnt += sprintf(pnt, "%04.4X", (ushort)value);
		break;

	 case VALUE_4_A:
		if (listthis || errors)
			pnt += sprintf(pnt, "%08.8X", value);
		break;

	 case VALUE_8_A:
		if (listthis || errors)
			pnt += sprintf(pnt, "00000000%08.8X", value);
		break;
	}
	if (listthis || errors)
	{
		*pnt++ = (val->value.psect != 0 && val->value.kind == VLK_OFS &&
				(val->value.flags & VLF_REL) == 0) ? '\'' :
				(val->value.flags & VLF_UNDEF) ? '?' :
				(val->value.sym == NULL) ? ' ' :
				(val->value.sym->sy_flag & SYF_IMPORT) ? '#' :
				(val->value.sym->sy_flag & SYF_EXTERN) ? '*' :
				(val->value.sym->sy_flag & SYF_UNDEF) ? '?' : ' ';
		if (val->value.flags & VLF_FAR)
			pnt += sprintf(pnt, (val->value.kind == VLK_OFS) ? "--%02.2X " :
					"%04.4X ", val->value.psect);
		else
			*pnt = 0;
	}
	return (pnt);
}


//********************************************************
// Function: doline - Generate listing output for one line
// Returned: Nothing
//********************************************************

static void doline(
	char *line,
	char *bufr)

{
	char text[8];

	*pnt = 0;
	if (clp->linenum < lastnum)
	{
		lincnt = -1;
		chkhead();
	}
	if (clp->linenum != lastnum)
	{
		sprintf(text, "%3d", clp->linenum);
		lastnum = clp->linenum;
	}
	else
		text[0] = 0;
	if (needbin)
	{
		sprintf(line, "%-3s%3s %08.8X%c %-31s%s", geterrorltrs(errors), text,
				offset, (clp->psd != NULL) ? '\'' : ' ', bufr, src);
	}
	else
		sprintf(line, "%-3s%3s          %-32s%s", geterrorltrs(errors), text,
				bufr, src);

	chkhead();
	listline(line, errors);
	src = blank;
	pnt = bufr;
}


//***********************************************
// Function: geterrorltrs - Get error letters for
//				listing or error message line
// Returned: Pointer to string
//***********************************************

static char *geterrorltrs(
	long errflag)

{
	static char text[4];

	char  *tpnt;
	char **epnt;

	tpnt = text;
	epnt = errmsgtbl;
	while (errflag != 0 && tpnt < (text + 3))
	{
		if (errflag & 0x80000000)
		{
			*tpnt++ = (*epnt)[0];
			totalerr++;
		}
		errflag <<= 1;
		epnt++;
	}
	*tpnt = 0;
	return (text);
}


//************************************
// Function: nextpage - Start new page
// Returned: Nothing
//************************************

void nextpage(void)

{
	lincnt = 0;							// Force new page
	pagcnm = 0;							// Reset continuation number
    lstseq = 1;							// Reset sequence number
    ++pagnum;							// Bump page number
    ++lsbnum;							// Bump local symbol block number
}


//***********************************
// Function: listline - List one line
// Returned: Nothing
//***********************************

void listline(
	char *line,
	long  err)

{
	liststr(line, err);
	listnxl(err);
}


//************************************************
// Function: listtoc - List table of contents line
// Returned: Nothing
//************************************************

void listtoc(void)

{
    if (lstfp && tocflg)				// Do we need output here?
    {
		if (!tchflg)					// Have we output the heading yet?
		{
			tchflg = TRUE;				// No - do it now
            chkhead();
			fputs("***  Table of contents  ***", lstfp);
			listnxl(0);
			listnxl(0);
			fputs("  Page-Line     Sub-title", lstfp);
			listnxl(0);
		}
        chkhead();
		fprintf(lstfp, "%6d-%-6ld  %s", pagnum, lstseq, hdrstb);
		listnxl(0);
    }
}


//********************************
// Function: liststr - List string
// Returned: Nothing
//********************************

void liststr(
	char *str,
	long  err)

{
    if (err && !lststd)
		fputs(str, stderr);
    if (lstfp)
    {
		chkhead();
		fputs(str, lstfp);
    }
}


//***********************************************
// Function: listnxl - Start next line on listing
// Returned: Nothing
//***********************************************

void listnxl(
	long err)

{
    if (err && !lststd)
        putc('\n', stderr);
    if (lstfp)
    {
        chkhead();						// See if need heading here
        putc('\n', lstfp);
        --lincnt;
    }
}


//*************************************************
// Function: chkhead - Output page header if needed
// Returned: Nothing
//*************************************************

void chkhead(void)

{
    if (lincnt <= 0)					// At top of page?
    {
		if (lstfp != NULL)
		{
			if (fhdflg)					// Is this the first header line?
				putc('\f', lstfp);		// No - output form-feed first
			else
				fhdflg = TRUE;			// Yes
			fprintf(lstfp, "\n\n\n\n\n%-65.65s %-24.24s   XMAC - version "
					"%d.%d   Page %d", hdrbfr, timebfr, version, editnum,
					pagnum);
			if (pagcnm)
				fprintf(lstfp, ".%d", pagcnm);
    		fprintf(lstfp, "\n%s\n%s\n\n", thissrc?thissrc->sb_fsp:"", hdrstb);
		}
		++pagcnm;
		lincnt = LSTPGSZ;
    }
}
