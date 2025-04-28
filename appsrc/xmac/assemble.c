//**************************************
// Function to do assembly pass for XMAC
//**************************************
// Written by John Goltz
//**************************************

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
#include "xmac.h"


//*************************************
// Function: assemble: Do assembly pass
// Returned: Nothing
//*************************************

void assemble(void)

{
	long total;
	long adj;
	int  pcnt;
	int  zcnt;

    eofflg = FALSE;						// Reset EOF indicator
    ttlflg = FALSE;						// And .TITLE seen flag
    tocflg = TRUE;						// Default is list table of contents
    lstseq = 1;							// Reset listing sequence number
    pagnum = 1;							// Reset page number
    lsbnum = 1;							// Reset local symbol block number
    gnacnt = 0;							// Reset generated argument number
    iradix = 16;						// Reset input radix
    curpsd = NULL;						// No psect to begin with
    condlvl = 0;						// Not in conditional
    hdrstb[0] = '\0';					// Indicate no subtitle text
    listinit();							// Initialize listing routines
    thissrc = (struct sb *)(&firstsrc);

	// Process each source file

    while (getsrc())					// Setup for input
	{
		do
		{
		    advanceline();				// Advance to next line
			asmline();					// Process single input line
			if (albcnt > 0)
				clstoretext();
		} while (!eofflg);				// Continue until run out of input
    }

	// Finish the assembly

    while (inspnt)						// Clean up any hanging insersion!
		endins();
    if (condlvl)						// Have open conditional?
		errflag |= ERR_CO;				// Yes - complain
    if (errflag != 0 && albcnt == 0)	// Make sure list final error(s)
    {
		albcnt = 1;
		albbuf[0] = '\0';
    }

	// The code list is now complete. Do the fix-up passes.

	clfixvalues(FALSE, FALSE);			// Do an initial pass without jmp
										//   fixup to resolve forward
										//   references
	total = 0;
	pcnt = 0;
	zcnt = 1;
	do
	{
		if (listtype == LT_DEBUG)
		{
			listnxl(0);
			listline("***** Calling clfixvalues: ", 0);
			listnxl(0);
			clshow();
		}
		adj = clfixvalues(TRUE, FALSE);
		total += adj;
		pcnt++;
		if (listtype == LT_DEBUG)
		{
			listnxl(0);
			sprintf(albbuf, "***** clfixvalues is complete, expanded by %d "
					"byte%s", adj, (adj == 1) ? "" : "s");
			listline(albbuf, 0);
			listnxl(0);

///			dolisting();		// #####

		}
		if (adjusted)
			zcnt = 0;
		else
			zcnt++;
	} while (zcnt < 2);					// Fix up values
	if (listtype == LT_DEBUG)
	{
		clshow();
		listnxl(0);
		sprintf(albbuf, "***** %d fixup pass%s, expanded by %d byte%s", pcnt,
				(pcnt == 1) ? "" : "es", total, (total == 1) ? "" : "s");
		listline(albbuf, 0);
		listnxl(0);
		listnxl(0);
	}
	clfixvalues(FALSE, TRUE);			// Do a final pass to check for and
										//   report truncation
	if (strclp != NULL)
		recalcaddr(strclp, &strval, strexp);
	if (dbgclp != NULL)
		recalcaddr(dbgclp, &dbgval, dbgexp);
	if (stkclp != NULL)
		recalcaddr(stkclp, &stkval, stkexp);

	// Fix-up is complete. Generate listing if needed and display error lines

    finish();				// Reorganize the symbol table
	dolisting();			// Generate a listing (or just error lines) if
							//   needed
    finlisting();			// Finish up listing output

	// Finally, generate the object file if needed

	if (!test)
		dobinary();
}
