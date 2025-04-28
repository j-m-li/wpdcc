//*********************************
// LIST.C Utility routines for XMAC
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
#include <ctype.h>
#include <string.h>
#include <xos.h>
#include <malloc.h>
#include <xcmalloc.h>
#include "xmac.h"

// Link to global data

extern int symsize;			// Length of symbol name in symbuf
extern SY *hashtbl[];		// The symbol hash table
extern struct fb *freelist;	// Memory block free list head pointer

// Allocate local data

int curhash;				// Current hash table index


//***************************************************
// Function: advanceline - Advance to next input line
// Returned: Nothing
//***************************************************

void advanceline(void)

{
    eolflg = FALSE;
	hldchar = '\0';
    albcnt = 0;
    albpnt = albbuf;
    bnoflg = FALSE;

///    if (curpsd)
///    {
///        lstadr = curpsd->pd_ofs;
///        lstpsn = curpsd->pd_psn;
///    }
///    else
///    {
///        lstadr = 0;
///        lstpsn = 0;
///    }

    lstinp = inspnt;
    errflag = 0;
}


//*******************************************
// Function: getblock - Allocate memory block
// Returned: Pointer to block
//*******************************************

struct fb *getblock(void)

{
    struct fb *temp;
    int    cnt;

    if (freelist == NULL)		// If have something on the freelist
    {					// If free list is empty
	temp = (struct fb *)getspace((long)(100*BLKSIZE));
        freelist = temp;		// Get some more blocks
        cnt = 100;
        while (--cnt >= 0)
        {
            temp->fb_next = temp + 1;
            temp++;
        }
        (temp-1)->fb_next = NULL;
    }
    temp = freelist;			// Get block from freelist
    freelist = temp->fb_next;
    temp->fb_next = NULL;
    return (temp);
}

//******************************************
// Function: givblock - Give up memory block
// Returned: Nothing
//******************************************

void givblock(
    struct fb *block)		// Pointer to block to give up

{
    block->fb_next = freelist;		// Put block on the free list
    freelist = block;
}


//***********************************************************
// Function: getsyq - Collect symbol in symbuf (also collects
//	local symbols)
// Returned: TRUE if symbol collected, FALSE if not a symbol
//***********************************************************

int getsyq(void)

{
    char chr;

    chr = nxtnbc();						// Get first character
    if (isdigit(chr))					// Is it a digit?
    {
        chr = getadg(chr);				// Yes - collect digits
        if (chr != '$')					// Stopped by dollar sign?
        {
            errorq();					// No - indicate Q error
            stopper = chr;
            return (FALSE);
        }
        else
        {
            getals();					// Yes - collect local symbol
            stopper = nxtnbc();
            return (TRUE);
        }
    }
    else
        return (getsym(chr));			// If not local symbol
}


//******************************************************************
// Function: getsym - Collect symbol in symbuf (use this function if
//	already have first character of symbol
// Returned: TRUE if symbol collected, FALSE if not a symbol
//******************************************************************

int getsym(
    char chr)

{
    int   cnt;
    char *pnt;

    chr = nxtnb0(chr);
    if (chr != '.' && !chksys(chr))		// Can this start a symbol?
    {
		stopper = chr;					// No - fail
		return (FALSE);
    }
    for (pnt = symbuf.nsym, cnt = SYMMAXSZ; --cnt >= 0;)
		*pnt++ = ' ';					// Clear symbuf

    symsize = 0;						// Clear symbol size
    sympnt = symbuf.nsym;
    do
    {   if (symsize < SYMMAXSZ)			// Is symbol full now?
		{
			++symsize;					// No
			*sympnt++ = chr;			// Store character in symbol
		}
		chr = nxtchar();				// Get next character
    } while (chksym(chr));
    stopper = chr;						// Store stopper
    return (TRUE);						// Indicate have symbol
}


//***********************************************
// Function: persym - Collect remainder of symbol
//	which may contain a period
// Returned: Nothing
//***********************************************

void persym(void)

{
    char chr;

    if ((chr=stopper) == '.')
    {
		while (chr == '.' || chksym(chr))
		{
			if (symsize < SYMMAXSZ)		// Is symbol full now?
			{
				++symsize;				// No
				*sympnt++ = chr;		// Store character in symbol
			}
			chr = nxtchar();			// Get next character
		}
		stopper = nxtnb0(chr);
    }
}


//****************************************
// Function: getals - Collect local symbol
// Returned: Nothing
//****************************************

void getals(void)

{
    symbuf.lsym.ls_flag = 0;			// Indicate local symbol
    symbuf.lsym.ls_blk = lsbnum;		// Store local symbol block number
    symbuf.lsym.ls_val = getanum(16);	// Store value of digits
    symsize = sizeof(struct locsym);
}


//**************************************************
// Function: getanum -  Return value of number after
//	digits have been collected in digbuf
// Returned: Value of number
//**************************************************

long getanum(
    int  radix)

{
    long  value;
    long  dig;
    char *pnt;

    value = 0;
    pnt = digbuf;
    while (--digcnt >= 0)
    {
	if ((dig = *pnt++) >= radix)
	{
            errorx();
	    dig = radix - 1;
	}
	value = value * radix + dig;
    }
    return (value);
}


//**********************************
// Function: getadg - Collect digits
// Returned: Stopper character
//**********************************

char getadg(
    char chr)

{
    char *pnt;
    int   cnt;

    pnt = digbuf;
    cnt = 0;
    do
    {
		if (cnt < DIGMAXSZ)
		{
			if (chr >= 'A')				// Adjust value between A and F
				chr += 9;
			*pnt++ = chr & 0xF;			// Store value in digit buffer
			++cnt;
		}
		else
            errorx();
		chr = nxtchar();				// Get next character
    } while (isxdigit(chr));
    digcnt = cnt;
    return (chr);
}


//*****************************************************
// Function: chksum - Determine if a character can be
//				part of a symbol
// Returned: TRUE if character can be part of a symbol,
//				FALSE if not
//*****************************************************

int chksym(
    char chr)

{
    return(isdigit(chr)) ? TRUE : chksys(chr);
}


//*************************************************************
// Function: chksym - Determine if a character can be the first
//	character in a symbol
// Returned: TRUE if character can start a symbol, FALSE if not
//*************************************************************

int chksys(
    char chr)

{
    return (isalpha(chr) || chr == '@' || chr == '%' || chr == '_'
			|| chr == '$' || chr == '?');
}


//****************************************************
// Function: looksym - Look for symbol in symbol table
// Returned: Pointer to symbol table entry
//****************************************************

SY *looksym(void)

{
    SY   *pnt;
    char *pnt1;
    char *pnt2;
    int   cnt;
    char  indexx;

    cnt = symsize;
    pnt1 = symbuf.nsym;
    indexx = 0;
    while (--cnt >= 0)
    {
		indexx <<= 1;
		if (indexx < 0)
			++indexx;
		indexx ^= *pnt1++;
    }
    curhash = indexx & 0x7F;
    pnt = (SY *)(&hashtbl[curhash]);	// Point to hash table entry
 
   while ((pnt = pnt->sy_hash) != NULL) // Scan down the hash chain
    {
		cnt = symsize;					// Check this entry
		pnt1 = symbuf.nsym;
		pnt2 = pnt->sy_name;
		while (--cnt >= 0)
		{
			if (*pnt1++ != *pnt2++)
				break;
		}

		if (cnt < 0 && (*pnt2 == '\0' || symsize == SYMMAXSZ))
			return (pnt);				// If this is it
    }
    return NULL;						// Not found if get here
}


//**************************************************
// Function: entersym - Enter symbol in symbol table
// Returned: Pointer to symbol table entry
//**************************************************

SY *entersym(void)

{
    SY   *pnt;
    char *pnt1;
    char *pnt2;
    int   cnt;

    pnt = (SY *)getblock();				// Get block for symbol table entry
    pnt->sy_hash = hashtbl[curhash];	// Link into hash list
    hashtbl[curhash] = pnt;
    cnt = symsize;						// Copy symbol to entry
    pnt1 = symbuf.nsym;
    pnt2 = pnt->sy_name;
    while (--cnt >= 0)
		*pnt2++ = *pnt1++;
    cnt = SYMMAXSZ - symsize;			// Set remainder to nulls
    while (--cnt >= 0)
		*pnt2++ = '\0';
    pnt->sy_symnum = 0;
    symnum++;
    return (pnt);						// Finished
}


//******************************************
// Function: givlist - Give up argument list
// Returned: Nothing
//******************************************

void givlist(
    struct ab *block)

{
    struct ab *save;

    while (block)
    {
	save = block->ab_link;
	givblock((struct fb *)block);
	block = save;
    }
}

//***********************************************************
// Function: chkpsect - Check that a psect has been specified
// Returned: Nothing
//***********************************************************

void chkpsect(void)

{
    if (ptype == 0)						// Processor type specified?
        ptype = P_8086;					// No - assume 8086
    if (curpsd == NULL || curmsd == NULL || cursgd == NULL)
    {									// Psect specified?
        errflag |= ERR_NP;				// No - fatal error!
        skiprest();

//// qqqqqqqqq        listline();

        fputs("? XMAC: No psect specified - cannot continue\n", stderr);
        exit(1);
    }
}

//**************************************************************
// Function: getsegnumps - Get segment number given psect number
// Returned: Segment number
//**************************************************************

int getsegnumps(
    int psect)

{
    struct pd *psd;

    if ((psd=pshead) != NULL)
    {
        do
        {   if (psd->pd_psn == psect)
                return (psd->pd_mdb->md_sdb->sd_sgn);
        } while ((psd=psd->pd_next) != NULL);
    }
    return (0xFFFF);
}


//**************************************************************
// Function: getsegnumms - Get segment number given msect number
// Returned: Segment number
//**************************************************************

int getsegnumms(
    int msect)

{
    struct md *msd;

    if ((msd=mshead) != NULL)
    {
        do
        {   if (msd->md_msn == msect)
                return (msd->md_sdb->sd_sgn);
        } while ((msd=msd->md_next) != NULL);
    }
    return (0xFFFF);
}


//**********************************
// Function: errorq - Report Q error
// Returned: Nothing
//**********************************

void errorq(void)

{
    errflag |= ERR_Q;
}


//**********************************
// Function: errorc - Report C error
// Returned: Nothing
//**********************************

void errorc(void)

{
    errflag |= ERR_C;
}


//**********************************
// Function: errorx - Report X error
// Returned: Nothing
//**********************************

void errorx(void)

{
    errflag |= ERR_X;
}
