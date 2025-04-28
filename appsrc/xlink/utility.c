//***************************
// Utility routines for XLINK
//***************************
// Written by John Goltz
//***************************

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <xcmalloc.h>
#include <xos.h>
#include <procarg.h>
#include "xlink.h"

// Allocate local data
int curhash;			// Current hash table index

void nullfunc(void)
{
}

//*************************************************************
// Function: looksym - Look for symbol in symbol table
// Returned: Pointer to symbol table entry or NULL if not found
//*************************************************************

SY *looksym(void)

{
    int    cnt;
    SY    *pnt;
    short  hindex;
    short *hpnt;
    char  *pnt1;
    char  *pnt2;

    cnt = (symsize + 1)/2;
    hpnt = (short *)symbuf;
    hindex = 0;
    while (--cnt >= 0)
    {
		hindex <<= 1;
		if (hindex < 0)
			++hindex;
		hindex ^= *hpnt++;
    }
    curhash = hindex & HASHMASK;
    if ((pnt = hashtbl[curhash]) != NULL)
    {
        do
        {
            cnt = symsize + 1;			// Check this entry
            pnt1 = symbuf;
            pnt2 = pnt->sy_name;
            while (--cnt >= 0)
            {
                if (*pnt1++ != *pnt2++)
                    break;
            }
            if (cnt < 0 && (pnt->sy_mod == NULL || pnt->sy_mod == curmod))
                return (pnt);
        } while ((pnt=pnt->sy_hash) != NULL);
    }
    return (NULL);						// Not found if get here
}


//*************************************************************
// Function: looklocsym - Look for local symbol in symbol table
// Returned: Pointer to symbol table entry or NULL if not found
//*************************************************************

SY *looklocsym(void)

{
    int    cnt;
    SY    *pnt;
    short  hindex;
    short *hpnt;
    char  *pnt1;
    char  *pnt2;

    cnt = (symsize+1)/2;
    hpnt = (short *)symbuf;
    hindex = 0;
    while (--cnt >= 0)
    {
		hindex <<= 1;
		if (hindex < 0)
			++hindex;
		hindex ^= *hpnt++;
    }
    curhash = hindex & HASHMASK;
    if ((pnt = hashtbl[curhash]) != NULL)
    {
        do
        {
            cnt = symsize + 1;			// Check this entry
            pnt1 = symbuf;
            pnt2 = pnt->sy_name;
            if (pnt->sy_mod == curmod)
            {
                while (--cnt >= 0)
                {
                    if (*pnt1++ != *pnt2++)
                        break;
                }
                if (cnt < 0)
                    return (pnt);
            }
        } while ((pnt=pnt->sy_hash) != NULL);
    }
    return (NULL);						// Not found if get here
}


//***************************************************
// Function: enetersym - Enter symbol in symbol table
// Returned: Pointer to symbol table entry
//***************************************************

SY *entersym(void)

{
    SY *sym;

    sym = (SY *)getspace((long)(symsize + 1 + offsetof(SY, sy_name)));
	sym->sy_magic = 'MYS*';				// Get memory for symbol table entry
    sym->sy_hash = hashtbl[curhash];	// Link into hash list
    sym->sy_mod = NULL;
    sym->sy_export = NULL;
	sym->sy_weak = NULL;
    hashtbl[curhash] = sym;

///	printf("#### sym: %.*s\n", symsize - 1, symbuf + 1);

    strncpy(sym->sy_name, symbuf, symsize); // Copy symbol name to entry
	sym->sy_name[symsize ] = 0;
    return (sym);						// Finished
}


//***************************************************
// Function: getpsd - Get address of psect data block
// Returned: Pointer to psect data block
//***************************************************

PD *getpsd(
    int local)					// Local psect number

{
	char buf[50];

	if (local < 0 || local > curmod->mb_psectnummax) // Valid psect number?
	{									// No - fail!
		sprintf(buf, "Illegal psect number %d", local);
		fail(buf);
	}
	return (curmod->mb_psectnumtbl[local]); // Yes - return address of
}											//   psect data block


//***************************************************
// Function: getmsd - Get address of msect data block
// Returned: Pointer to msect data block
//***************************************************

MD *getmsd(
    int local)					// Local msect number

{
	char buf[50];

    if (local <= 0 || local > curmod->mb_msectnummax) // Valid msect number?
    {									// No - fail

        sprintf(buf, "Illegal msect number %d", local);
		fail(buf);
    }
    return (curmod->mb_msectnumtbl[local - 1]); // Yes - return address of
}											    //   msect data block


//*****************************************************
// Function: getsgd - Get address of segment data block
// Returned: Pointer to segment data block
//*****************************************************

SD *getsgd(
    int local)					// Local segment number

{
	char buf[50];

    if (local <= 0 || local > curmod->mb_segnummax) // Valid segment number?
    {									// No - fail!
        sprintf(buf, "Illegal segment number %d", local);
		fail(buf);
    }
    return (curmod->mb_segnumtbl[local - 1]); // Yes - return address of
}											  //   segment data block


//*************************************
// Function: clrbuf - Clear disk buffer
// Returned: Nothing
//*************************************

void clrbuf(
    long *buffer,
    int   size)

{
    size >>= 2;
    do
    {
		*buffer++ = 0;
    } while (--size > 0);
}


//*************************************************************
// Function: vallength - Get number of bytes required for value
// Returned: Number of bytes
//*************************************************************

int vallength(
    long value)

{
    int rtn;

    rtn = 0;
    if (value == 0)
        return (0);
    if ((value & 0xFFFFFF00) == 0 || (value & 0xFFFFFF00) == 0xFFFFFF00)
        return (1);
    if ((value & 0xFFFF0000) == 0 || (value & 0xFFFF0000) == 0xFFFF0000)
        return (2);
    return (4);
}


//***************************************
// Function: outname - Output name string
// Returned: Nothing
//***************************************

void outname(
    char *pnt)

{
    char chr;

    for (;;)
    {
        chr = *pnt++;
        if (*pnt == 0)
            break;
        putbyte(chr);
    }
    putbyte(chr | 0x80);
}


//**************************************************
// Function: resolvemod - Resolve two modulus values
// Returned: New modulus value
//**************************************************

int resolvemod(
	int oldmod,
	int newmod)

{
	int big;
	int little;

	if (newmod <= 1)
		return (oldmod);
	if (oldmod <= 1)
		return (newmod);

	// We are really trying to find the lowest common demonanator here.
	//   Since this is hard in general and we know that almost all mod
	//   values will be a power of 2, we just reduce out common factors
	//   of 2 and use the product.

	if (newmod < oldmod)				// We don't really care which is the
	{									//  new one, just which one is bigger
		big = oldmod;
		little = newmod;
	}
	else
	{
		big = newmod;
		little = oldmod;
	}

	// First eliminate the simple case where the smaller mod divides evenly
	//   into the larger mod

	if ((big % little) == 0)
		return (big);

	// Now reduce each by 2 as many times as we can

	while ((big & 0x01) == 0 && (little & 0x01) == 0)
	{
		big >>= 1;
		little >>= 1;
	}
	return (big * little);
}
