//========================================================
//
// OUTPUT.C - Routines to output to the RUN file for XLINK
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
#include <xos.h>
#include <procarg.h>
#include <xossvc.h>
#include <xoserr.h>
#include <xoserrmsg.h>
#include "xlink.h"

char *OUTBUFBASE_ = 0;


//*********************************************
// Function: outopen - Open the output file and
//				allocate the output buffer
// Returned: Nothing (does not return if error)
//*********************************************

void outopen(
	char *spec)

{
	if ((outdd = svcIoOpen(XO_CREATE|XO_TRUNCA|XO_IN|XO_OUT, spec, NULL)) < 0)
										// Open the output file
		femsg(prgname, outdd, spec);	// If error
	if ((OUTBUFBASE_ = realloc(OUTBUFBASE_, 0x1000)) < 0) 
		femsg2(prgname, "Erroring creating output buffer", outbuftop, NULL);
	outbuftop = 0x1000;
	outbufsize = 0;
}


//***************************************************************
// Function: putvarval - Output variable length value to RUN file
// Returned: Nothing
//***************************************************************

void putvarval(
    long value)

{
    if (value <= 0x7F)
        putbyte(value);
    else if (value <= 0x3FFF)
    {
        putbyte((value >> 8) | 0x80);
        putbyte(value);
    }
    else if (value <= 0x1FFFFF)
    {
        putbyte((value >> 16) | 0xC0);
        putbyte(value >> 8);
        putbyte(value);
    }
    else
    {
        putbyte((value >> 24) | 0xE0);
        putbyte(value >> 16);
        putbyte(value >> 8);
        putbyte(value);
    }
}


//********************************************
// Function: putlong - Output long to RUN file
// Returned: Nothing
//********************************************

void putlong(
    long value)

{
    short shrt;

    if (lowfirst)
    {
		shrt = value;
		putword(shrt);
		shrt = value >> 16;
		putword(shrt);
    }
    else
    {
		shrt = value >> 16;
		putword(shrt);
		shrt = value;
		putword(shrt);
    }
}


//********************************************
// Function: putword - Output word to RUN file
// Returned: Nothing
//********************************************

void putword(
    long value)

{
    if (lowfirst)
    {
		putbyte(value);
		putbyte(value >> 8);
    }
    else
    {
		putbyte(value >> 8);
		putbyte(value);
    }
}


//***************************************************
// Function: putbyte - Output single byte to RUN file
// Returned: Nothing
//***************************************************

void putbyte(
	long data)			// Byte to output

{
    if (outdd)							// Only do this if doing output
    {
		if (offset >= outbuftop)
		{
			if ((OUTBUFBASE_ = realloc(OUTBUFBASE_,
					offset + 1)) < 0)
				femsg2(prgname, "Error allocating memory", outbuftop, NULL);
			outbuftop = offset + 1;
		}
		OUTBUFBASE_[offset] = data;
	}
    offset++;
	if (offset > outbufsize)
		outbufsize = offset;
}


//*******************************************
// Function: outclose - Close the output file
// Returned: Nothing
//*******************************************

void outclose(void)

{
	char *pnt;
	long  left;
	long  amount;
    long  rtn;

	pnt = OUTBUFBASE_;
	left = outbufsize;

	do
	{
		if ((amount = left) > 0x100000)
			amount = 0x100000;
		if ((rtn = svcIoOutBlock(outdd, pnt, amount)) < 0)
										// Write the output file
			femsg2(prgname, "Error writing output file", rtn, outfsp);
		left -= amount;
		pnt += amount;
	} while (left > 0);
    if ((rtn = svcIoClose(outdd, 0)) < 0) // Close the output file
		femsg2(prgname, "Error closing output file", rtn, outfsp);
}
