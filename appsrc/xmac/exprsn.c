//*******************************************
// Functions to evaluate expressions for XMAC
//*******************************************
// Written by John Goltz
//*******************************************

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

// Allocate local data

char *sympnt;					// Pointer to symbuf
char  regntbl[] =				// Register name table
{   'A','L',' ',' ', RG_AL,  1,
    'A','H',' ',' ', RG_AH,  1,
    'B','L',' ',' ', RG_BL,  1,
    'B','H',' ',' ', RG_BH,  1,
    'C','L',' ',' ', RG_CL,  1,
    'C','H',' ',' ', RG_CH,  1,
    'D','L',' ',' ', RG_DL,  1,
    'D','H',' ',' ', RG_DH,  1,
    'A','X',' ',' ', RG_AX,  2,
    'B','X',' ',' ', RG_BX,  2,
    'C','X',' ',' ', RG_CX,  2,
    'D','X',' ',' ', RG_DX,  2,
    'S','I',' ',' ', RG_SI,  2,
    'D','I',' ',' ', RG_DI,  2,
    'B','P',' ',' ', RG_BP,  2,
    'S','P',' ',' ', RG_SP,  2,
    'E','A','X',' ', RG_EAX, 4,
    'E','B','X',' ', RG_EBX, 4,
    'E','C','X',' ', RG_ECX, 4,
    'E','D','X',' ', RG_EDX, 4,
    'E','S','I',' ', RG_ESI, 4,
    'E','D','I',' ', RG_EDI, 4,
    'E','B','P',' ', RG_EBP, 4,
    'E','S','P',' ', RG_ESP, 4,
    'C','S',' ',' ', RG_CS,  2,
    'D','S',' ',' ', RG_DS,  2,
    'S','S',' ',' ', RG_SS,  2,
    'E','S',' ',' ', RG_ES,  2,
    'F','S',' ',' ', RG_FS,  2,
    'G','S',' ',' ', RG_GS,  2,
    'P','C',' ',' ', RG_PC,  4,
    'F','R',' ',' ', RG_FR,  2,
    'L','D','T','R', RG_LDTR,2,
    'M','S','W',' ', RG_MSW, 2,
    'T','R',' ',' ', RG_TR,  2,
    'G','D','T','R', RG_GDTR,6,
    'I','D','T','R', RG_IDTR,6,
    'C','R','0',' ', RG_CR0, 4,
    'C','R','2',' ', RG_CR2, 4,
    'C','R','3',' ', RG_CR3, 4,
    'C','R','4',' ', RG_CR4, 4,
    'D','R','0',' ', RG_DR0, 4,
    'D','R','1',' ', RG_DR1, 4,
    'D','R','2',' ', RG_DR2, 4,
    'D','R','3',' ', RG_DR3, 4,
    'D','R','6',' ', RG_DR6, 4,
    'D','R','7',' ', RG_DR7, 4,
    'T','R','6',' ', RG_TR6, 4,
    'T','R','7',' ', RG_TR7, 4,
    'S','T',' ',' ', RG_ST0, 4,
    'S','T','0',' ', RG_ST0, 4,
    'S','T','1',' ', RG_ST1, 4,
    'S','T','2',' ', RG_ST2, 4,
    'S','T','3',' ', RG_ST3, 4,
    'S','T','4',' ', RG_ST4, 4,
    'S','T','5',' ', RG_ST5, 4,
    'S','T','6',' ', RG_ST6, 4,
    'S','T','7',' ', RG_ST7, 4
};
#define REGNUM ((sizeof(regntbl))/5)

char *xinpnt;
char *xstrpnt;
int   xstrcnt;
char  xstrbufr[XSTRMAX + 4];


static char getadgx(char chr);
static int  getsymx(char chr);
static void level1(VL *value);
static void level2(VL *value);
static void level3(VL *value);
static void level4(VL *value);
static void level5(VL *value);
static void level6(VL *value);
static void level7(VL *value);
static void level8(VL *value);
static void level9(VL *value);
static int  nxtcharx(void);
static int  nxtnbc0x(int chr);
static int  nxtnbcx(void);
static int  nxtnb0x(int chr);
static void primary(VL *value);


//***************************************************
// Function: getabs - Evaluate general expression and
//				return absolute value only
// Returned: Nothing
//***************************************************

void getabs(
	VL   *value)

{
    exprsn(value);						// Evalulate expression
    if (value->kind != VLK_ABS)			// Simple absolute value?
    {
        errorx();						// No - complain
		value->psect = 0;				// And return 0 as value
        value->kind = VLK_ABS;
		value->val = 0;
		value->type = VLT_VAL;
    }
}


//***********************************************
// Function: exprsn - Evaluate general expression
// Returned: Nothing
//***********************************************

void exprsn(
	VL   *value)

{
	xinpnt = NULL;
	xstrpnt = xstrbufr;
	xstrcnt = 0;
	xstrbufr[0] = 0;
	if (hldchar != 0)
	{
		*xstrpnt++ = hldchar;
		xstrcnt++;
	}
	memset(value, 0, sizeof(VL));
    stopper = nxtnbcx();
    level1(value);
    if (stopper == '.')					// Stopped by period?
    {
        stopper = toupper(nxtcharx());	// Yes - get next character
        if (value->type == VLT_VAL)		// If normal value
        {
            switch(stopper)
            {
             case 'B':					// Byte value
			 case 'S':
                value->type = VLT_BYTE;
                value->size = 1;
                break;

             case 'W':					// Word value
                value->type = VLT_WORD;
                value->size = 2;
                break;

             case 'L':					// Long value
                value->type = VLT_LONG;
                value->size = 4;
                break;

///			 case 'Q':					// Quad value
///				value->type = VL_QUAD;
///				value->size = 8;
///				break;

             default:					// Invalid modifier
                errorx();
				*xstrpnt = 0;
                return;
            }
        }
        else							// Must be register value
        {
            errorx();
			*xstrpnt = 0;
            return;
        }
        stopper = nxtnbcx();			// Get stopper
    }
	*xstrpnt = 0;

///	printf("### EXP: |%s|\n", xstrbufr);
}


//**************************************************
// Function: simpleexp - Evaluate general expression
//				without selector or suffix
// Returned: Nothing
//**************************************************

void simpleexp(
	VL   *value,
	char *pntr)

{
	xinpnt = pntr;
	xstrpnt = xstrbufr;
	xstrcnt = 0;
	xstrbufr[0] = 0;
	if (hldchar != 0)
	{
		*xstrpnt++ = hldchar;
		xstrcnt++;
	}
	memset(value, 0, sizeof(VL));
    stopper = nxtnbcx();
    level2(value);
	*xstrpnt = 0;
}


//****************************************************************
// Function: exprsnwo - Evaluate general expression without suffix
// Returned: Nothing
//****************************************************************

void exprsnwo(
	VL   *value,
	char *pntr)

{
	xinpnt = pntr;
	xstrpnt = xstrbufr;
	xstrcnt = 0;
	xstrbufr[0] = 0;
	if (hldchar != 0)
	{
		*xstrpnt++ = hldchar;
		xstrcnt++;
	}
	memset(value, 0, sizeof(VL));
    stopper = nxtnbcx();
    level1(value);
	*xstrpnt = 0;
}


void getatom(
	VL *value)

{
	xinpnt = NULL;
	xstrpnt = xstrbufr;
	xstrcnt = 0;
	xstrbufr[0] = 0;
	if (hldchar != 0)
	{
		*xstrpnt++ = hldchar;
		xstrcnt++;
	}
    stopper = nxtnbcx();
	memset(value, 0, sizeof(VL));
    primary(value);
	*xstrpnt = 0;
}


//*********************************************
// Function: level1 - Process level 1 operators
// Returned: Nothing
//*********************************************

static void level1(
    VL *value)

{
    VL rval;

    level2(value);						// Evaluate left operand
    if (stopper == ':' && value->kind != VLK_REG)
    {
		stopper = nxtnbcx();
		memset(&rval, 0, sizeof(VL));
		level2(&rval);					// Evaluate right operand
		if (value->kind != VLK_ABS ||
				(rval.kind != VLK_ABS && rval.kind != VLK_ABA))
			errorx();					// Both operands must be absolute
		value->psect = value->val;
		value->val = rval.val;
		value->kind = VLK_ABA;
		value->flags |= rval.flags;
	}
}

//*****************************
// Function: level2 - Process |
// Returned: Nothing
//*****************************

static void level2(
    VL *value)

{
	VL rval;

    level3(value);						// Evaluate left operand
    while (stopper == '|' && value->kind != VLK_REG)
    {
		stopper = nxtnbcx();
		memset(&rval, 0, sizeof(VL));
		level1(&rval);					// Evaluate right operand
		if (value->kind != VLK_ABS || rval.kind != VLK_ABS)
            errorx();					// Both operands must be absolute
		value->val |= rval.val;			// Do inclusive or
        value->flags |= rval.flags;
    }
}

//*****************************
// Function: level3 - Process ^
// Returned: Nothing
//*****************************

static void level3(
    VL *value)

{
    VL rval;

    level4(value);						// Evaluate left operand
    while (stopper == '^' && value->kind != VLK_REG)
    {
		stopper = nxtnbcx();
		memset(&rval, 0, sizeof(VL));
		level4(&rval);					// Evaluate right operand
		if (value->kind != VLK_ABS || rval.kind != VLK_ABS)
            errorx();					// Both operands must be absolute
		value->val ^= rval.val;			// Do exclusive or
        value->flags |= rval.flags;
    }
}

//*****************************
// Function: level4 - Process &
// Returned: Nothing
//*****************************

static void level4(
    VL *value)

{
    VL rval;

    level5(value);						// Evaluate left operand
    while (stopper == '&' && value->kind != VLK_REG)
    {
		stopper = nxtnbcx();
		memset(&rval, 0, sizeof(VL));
		level5(&rval);					// Evaluate right operand
		if (value->kind != VLK_ABS || rval.kind != VLK_ABS)
            errorx();					// Both operands must be absolute
		value->val &= rval.val;			// Do and
        value->flags |= rval.flags;
    }
}

//***********************************
// Function: level5 - Process > and <
// Returned: Nothing
//***********************************

static void level5(
    VL *value)

{
    VL   rval;
    char chr;

    level6(value);						// Evaluate left operand
    while (((chr=stopper) == '<' || chr == '>') && value->kind != VLK_REG)
    {
		stopper = nxtnbcx();
		memset(&rval, 0, sizeof(VL));
		level6(&rval);					// Evaluate right operand
		if (value->kind != VLK_ABS || rval.kind != VLK_ABS)
            errorx();					// Both operands must be absolute
		if (chr == '<')
			value->val <<= rval.val;	// If left shift
		else
			value->val = ((unsigned long)(value->val)) >> rval.val;
										// If right shift
        value->flags |= rval.flags;
    }
}

//***********************************
// Function: level6 - Process + and -
// Returned: Nothing
//***********************************

static void level6(
    VL *value)

{
    VL rval;
    char   chr;

    level7(value);						// Evaluate left operand

///	printf("### after level7, t:%d k:%d sym:%s\n", value->type, value->kind,
///			(value->sym == NULL) ? "<NONE>" : value->sym->sy_name);

    while (((chr=stopper) == '+' || chr == '-') && value->kind != VLK_REG)
    {
		stopper = nxtnbcx();
		memset(&rval, 0, sizeof(VL));
		level7(&rval);					// Evaluate right operand
        if (value->kind >= VLK_REG || rval.kind >= VLK_REG)
        {								// Error if either operand is an
            errorx();					//   address space selector or a
            continue;					//   register
        }
		if (chr == '+')					// If add
		{
			if (value->kind == VLK_ABS) // If left operand is absolute
			{
				value->psect = rval.psect;
				value->kind = rval.kind;
				if (value->sym == NULL)
					value->sym = rval.sym;
			}
			else							// If left operand is relative
			{				    			//   or absolute address or
				if (rval.kind != VLK_ABS)	//   external, right operand
					errorx();	   			//   must be absolute
			}
			value->val += rval.val;
		}
		else							// If subtract
		{
			if (value->flags & VLF_UNDEF) // If left operand is undefined
				;						  //   don't check any more now
			else if (value->kind == VLK_ABS || value->kind == VLK_EXT)
			{							// If left operand absolute or
										//   external
				if (rval.kind != VLK_ABS) // Right operand must be absolute
					errorx();
			}
			else						// If left operand is address
			{
				if ((value->kind == rval.kind && value->psect == rval.psect))
				{
					// Right operand can be address for same psect, difference
					//   is absolute.  Note: We also allow either operand
					//   to be undefined.  It may be a forward refrence to a
					//   valid address.  If not, the error will be reported by
					//   the fixup code.

					value->psect = 0;
					value->kind = VLK_ABS;
				}
				else if (rval.kind == VLK_ABS) // Right operand can be
		    		;						   //   absolute, difference is
				else						   //   an address
					errorx();		 	// Else bad expression
			}
			value->val -= rval.val;
		}
		value->flags |= rval.flags;
	}
}


//***********************************
// Function: level7 - Process * and /
// Returned: Nothing
//***********************************

static void level7(
    VL *value)

{
    VL   rval;
    char chr;

    level8(value);						// Evaluate left operand
    while (((chr=stopper) == '*' || chr == '/') && value->kind != VLK_REG)
    {
		stopper = nxtnbcx();
		memset(&rval, 0, sizeof(VL));
		level8(&rval);					// Evaluate right operand
		if (value->kind != VLK_ABS || rval.kind != VLK_ABS)
            errorx();					// Both operands must be absolute
		if (chr == '*')
			value->val *= rval.val;		// If multiply
		else
		{
			if (rval.val == 0)			// If divide
                errorx();
			else
				value->val /= rval.val;
		}
		value->flags |= rval.flags;
    }
}

//**********************************
// Function: Process unary operators
// Returned: Nothing
//**********************************

static void level8(
    VL *value)

{
    char chr;

    if ((chr=stopper) == '~' || chr == '-' || chr == '!')
    {
		stopper = nxtnbcx();
		level8(value);					// Evaluate operand
        if (chr == '!')					// Want selector part?
        {								// Yes
			if (value->flags & VLF_UNDEF) // Is operand undefined?
			{							// Just make it 0 for now
                value->val = 0;
                value->psect = 0;
                value->kind = VLK_ABS;
			}
            else if (value->kind == VLK_ABA) // Absolute address?
            {							// Yes - just use value of selector
										//   part
                value->val = (unsigned long)(value->psect);
                value->psect = 0;
                value->kind = VLK_ABS;
            }
            else if (value->kind == VLK_OFS) // Address?
            {
                value->val = 0; 		// Yes
                value->kind = VLK_SEL;
                value->psect = getsegnumps(value->psect);
            }
            else if (value->kind == VLK_EXT) // External value?
            {
                value->val = 0;			// Yes
                value->kind = VLK_SLX;
            }
            else if (value->kind == VLK_MSC) // Msect value?
            {
                value->val = 0;			// Yes
                value->kind = VLK_SEL;
                value->psect = getsegnumms(value->psect);
            }
            else if (value->kind == VLK_SEG) // Segment value?
            {
                value->val = 0;			// Yes
                value->kind = VLK_SEL;
            }
            else
			    errflag |= ERR_NA;
        }
        else							// Must be minus or 1's complement
        {
            if (value->kind != VLK_ABS)
                errorx();				// Error if not absolute
            if (chr == '~')
                value->val = ~value->val;
            else
                value->val = -value->val;
        }
    }
    else
		level9(value);
}

//**********************************
// Function: level9 - Process braces
// Returned: Nothing
//**********************************

static void level9(
    VL *value)

{
    if (stopper == '{')					// Start of sub-expression?
    {
		stopper = nxtnbcx();				// Yes
		level1(value);					// Evaluate sub-expression
		if (stopper == '}')				// Must end with right brace
			stopper = nxtnbcx();
		else
			queerr();
    }
    else
    {
		primary(value);					// Get value of atom
		stopper = nxtnb0x(stopper);
    }
}

//************************************
// Function: primary - Fetch next atom
// Returned: Nothing
//************************************

static void primary(
    VL *value)

{
    char *temp;
    char *tpnt;
    SY   *sym;
    int   extflag;
    int   cnt;
	int   tradix;
    char  chr;
    char  char0;
	char  char1;
	char  char2;
	char  char3;

    value->type = VLT_VAL;
    value->kind = VLK_ABS;
    value->psect = 0;
    value->size = 4;
    value->flags = 0;
    chr = stopper;						// Get first character
    if (chr == '\'')					// Single quote?
    {
        cnt = 0;
        value->val = 0;
        while ((chr=nxtcharx()) != '\0' &&
                (chr != '\'' || (chr=nxtcharx()) == '\''))
        {
            value->val |= ((long)chr << cnt);
            cnt += 8;
        }
		stopper = nxtnb0x(chr);
		return;
    }
    if (chr == '"')						// Double quote?
    {
		value->valc1 = nxtcharx() & 0x7F; // Yes
		value->valc0 = nxtcharx() & 0x7F;
		stopper = nxtnbcx();
		return;
    }
    if (isdigit(chr))					// Is it a digit?
    {
		tradix = iradix;

		if (chr == '0')
		{
			if ((chr = nxtcharx()) == 'x' || chr == 'X')
			{
				tradix = 16;
				chr = nxtcharx();
			}
			else
			{
				hldchar = chr;
				chr = '0';
				if (gccflg)
					tradix = 8;
			}
		}
		chr = getadgx(chr);				// Collect digits
		chr = toupper(chr);
		switch(chr)
		{
         case '$':						// Local symbol
			getals();					// Yes - collect local symbol
            if ((sym = looksym()) != NULL) // Find symbol table entry for symbol
            {
                if (sym->sy_flag & SYF_MULT)
                    errflag |= ERR_D;
                value->val = sym->sy_val.val;
                value->psect = sym->sy_psect;
                value->kind = (sym->sy_flag & SYF_ABS)? VLK_ABA: VLK_OFS;
            }
            else
            {
                errflag |= ERR_U;		// Undefined if not in symbol table
                value->flags |= VLF_UNDEF; // Remember that its undefined
                value->val = 0;
            }
			if (value->psect != 0) // Also volatile if relocatable
				value->flags |= VLF_VOLATILE;
            stopper = nxtnbcx();
			if (stopper == '#')			// Was symbol stopped by #?
                errflag |= ERR_A;		// Yes - # following local symbol
            return;						//   is an error!

         case '.':						// Might be floating point number?
            chr = nxtcharx();			// Get following character
            if (isdigit(chr))			// Floating point if digit
            {
                do		// CODE GOES HERE TO HANDLE FLOATING POINT!
                {

                } while (isdigit((chr=nxtcharx())));
                value->val = 0x55555L;
                errflag |= ERR_X;
                break;
            }
            hldchar = chr;				// If not a floating point number
            value->val = getanum((int)tradix); // Get value
            stopper = '.';
            return;

         case 'O':						// Octal number
         case 'Q':
            value->val = getanum(8);	// Yes - get value
			break;

         case 'T':						// Decimal number
            value->val = getanum(10);	// Yes - get value
			break;

         case 'H':						// Hex number
            value->val = getanum(16);	// Yes - get value
			break;

         case 'W':						// Binary number
            value->val = getanum(2);	// Yes - get value
			break;

         default:						// If no radix specified
            value->val = getanum((int)tradix); // Get value
			stopper = chr;
            return;
		}
        stopper = nxtnbcx();
		return;
    }
    if (!getsymx(chr))					// Try to collect symbol
    {
		value->val = 0;					// Not symbol - return 0
		return;
    }
	if (gccflg && (chr=stopper) == '.')
    {
		while (chr == '.' || chksym(chr))
		{
			if (symsize < SYMMAXSZ)		// Is symbol full now?
			{
				++symsize;				// No
				*sympnt++ = chr;		// Store character in symbol
			}
			chr = nxtcharx();			// Get next character
		}
		stopper = nxtnb0x(chr);
    }
    if (symsize == 1 && symbuf.nsym[0] == '$') // Is this the current addr?
    {
        chkpsect();						// Yes - make sure have psect
		value->val = curpsd->pd_ofs;
		value->psect = curpsd->pd_psn;
        value->kind = VLK_OFS;
		value->flags |= VLF_VOLATILE;

///		printf("### have $ on line %d at %08.8X for psect %d\n", lineno,
///				value->val, value->psect);

		return;
    }
    temp = NULL;
    if (prnflg)							// Using special register names?
    {
		if ((symsize >= 3 && symsize <= 5) && symbuf.nsym[0] == '%') // Yes
			temp = &symbuf.nsym[1];
    }
    else
    {
		if (symsize>=2 && symsize<=4)	// No
			temp = &symbuf.nsym[0];
    }
    if (temp)							// Might this be a register name?
    {
        char0 = toupper(temp[0]);
        char1 = toupper(temp[1]);
        char2 = toupper(temp[2]);
        char3 = toupper(temp[3]);
        if (char0 == '%')				// Want auto-size?
        {								// Yes
            if (cursgd && (segatrb & SA_32BIT)) // 32 bit?
                char0 = 'E';			// Yes
            else
            {
                char0 = char1;			// No
                char1 = char2;
                char2 = char3;
                char3 = ' ';
            }
        }
		tpnt = regntbl;
		cnt = REGNUM;
		do
		{
			if (tpnt[0]==char0 && tpnt[1]==char1 && tpnt[2]==char2
					&& tpnt[3] == char3)
			{							// Found it - get value
				value->val = (long)(tpnt[4]);
                value->psect = 0;
                value->size = tpnt[5];
				value->type = VLT_VAL;
                value->kind = VLK_REG;
				if (gccflg && value->val == RG_ST0 && stopper == '(')
				{
		            stopper = nxtnbcx();
					if (stopper >= '0' && stopper <= '7')
					{
						value->val += (stopper & 0x0F);
						if (nxtnbcx() == ')')
						{
							stopper = nxtnbcx();
							return;
						}
					}
					errflag |= ERR_Q;
				}
				return;
			}
			tpnt += 6;					// Not this one - bump table pointer
		} while (--cnt > 0);
    }
    if (stopper == '#')					// Was symbol stopped by #?
    {
		stopper = nxtnbcx();			// Yes - stopper is next character
        if (stopper == '#')
        {
            stopper = nxtnbcx();
            extflag = 2;
        }
        else
            extflag = 1;
    }
    else
		extflag = 0;


	if (gccflg && symsize > 11)			// Check for _F_ kludge
	{
		if (memcmp(symbuf.nsym + symsize - 3, "_F_", 3) == 0)
		{
			symsize -= 3;
			memcpy(symbuf.nsym + symsize, "   ", 3);
			value->flags |= VLF_FFAR;
		}
	}
    sym = findsym(FALSE);				// Search symbol table

///	printf("### SYM: |%s| = %08.8X flgs: %04.4X\n", sym->sy_name,
///			sym->sy_val.val, sym->sy_flag);

    if (prmcnt <= 0)					// From parameter file?
        sym->sy_flag |= SYF_USED;		// No - indicate symbol has been used
    if (sym->sy_flag & SYF_MULT)		// Is it multiply defined?
        errflag |= ERR_D;				// Yes - flag as D error
    if (sym->sy_flag & SYF_UNDEF)		// Undefined symbol?
    {
		if (extflag)					// Yes - followed by # or ##?
		{
			sym->sy_psect = 0;			// Yes - make the symbol external
            sym->sy_flag |= (extflag == 1) ? SYF_EXTERN : SYF_IMPORT;
			sym->sy_flag &= ~SYF_UNDEF;
			value->sym = sym;
			value->val = 0;
			value->kind = VLK_EXT;
			return;
		}
		else
		{								// Not followed by # - its undefined
            value->flags |= VLF_UNDEF;	// Remember that its undefined
			sym->sy_flag |= SYF_UNDEF;
			value->sym = sym;
			value->val = 0;				// Return 0 as value
			value->kind = VLK_ABS;		// Treat it as absolute for now
			return;
		}
    }
    else								// If defined
    {
        if (extflag)
        {
            if (!(sym->sy_flag & (SYF_EXTERN|SYF_IMPORT)))
										// Error if flagged as external but
                errflag |= ERR_M;		//   symbol is not external or
            else						//   imported!
            {
                if (extflag == 2)		// Make sure imported if
                {				  		//   flagged as imported
                    sym->sy_flag |= SYF_IMPORT;
                    sym->sy_flag &= ~SYF_EXTERN;    
                }
            }
        }
    }
    if (sym->sy_flag & (SYF_EXTERN|SYF_IMPORT)) // External or imported?
    {
        value->kind = VLK_EXT;			// Yes
		value->sym = sym;
		value->val = 0;
        value->psect = 0;
    }
    else if (sym->sy_type == SYT_SYM)	// Really a symbol?
    {
        value->val = sym->sy_val.val;
        value->psect = sym->sy_psect;
        value->kind = (sym->sy_flag & (SYF_EXTERN|SYF_IMPORT))? VLK_EXT:
                !(sym->sy_flag & SYF_ADDR)? VLK_ABS:
                (sym->sy_flag & SYF_ABS)? VLK_ABA: VLK_OFS;
        if (sym->sy_flag & SYF_REG)		// Is this value a register?
        {
            value->kind = VLK_REG;		// Yes
            if (extflag)				// Can't have # after register
                errflag |= ERR_A;
        }
		if (value->psect != 0 && (sym->sy_flag & SYF_ABS) == 0)
			value->flags |= VLF_VOLATILE; // Volatile if relocatable
    }
    else								// If not symbol (segment, msect, or
    {									//   psect
        value->val = 0;
        value->psect = sym->sy_psect;
        value->kind = (sym->sy_type == SYT_SEG)? VLK_SEG:
                (sym->sy_type == SYT_MSC)? VLK_MSC: VLK_OFS;
    }
}


//***********************************************************
// Function: getsyq - Collect symbol in symbuf (also collects
//	local symbols)
// Returned: TRUE if symbol collected, FALSE if not a symbol
//***********************************************************

/*
static int getsyq(void)

{
    char chr;

    chr = nxtnbcx();					// Get first character
    if (isdigit(chr))					// Is it a digit?
    {
        chr = getadgx(chr);				// Yes - collect digits
        if (chr != '$')					// Stopped by dollar sign?
        {
            errorq();					// No - indicate Q error
            stopper = chr;
            return (FALSE);
        }
        else
        {
            getals();					// Yes - collect local symbol
            stopper = nxtnbcx();
            return (TRUE);
        }
    }
    else
        return (getsymx(chr));			// If not local symbol
}
*/

//****************************************************************
// Function: getsymx - Collect symbol in symbuf (use this function
//				if already have first character of symbol
// Returned: TRUE if symbol collected, FALSE if not a symbol
//****************************************************************

static int getsymx(
    char chr)
{
    int   cnt;
    char *pnt;

    chr = nxtnb0x(chr);
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
		chr = nxtcharx();				// Get next character
    } while (chksym(chr));
    stopper = chr;						// Store stopper
    return (TRUE);						// Indicate have symbol
}


//***********************************
// Function: getadgx - Collect digits
// Returned: Stopper character
//***********************************

static char getadgx(
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
		chr = nxtcharx();				// Get next character
    } while (isxdigit(chr));
    digcnt = cnt;
    return (chr);
}


static int nxtnbcx(void)
{
    int chr;

    do
    {
		chr = nxtcharx();				// Get next input character
    } while (chr != '\n' && isspace(chr));
    return (chr);
}


static int nxtnb0x(
	int chr)
{
    while (chr != '\n' && isspace(chr))
		chr = nxtcharx();
    return (chr);
}


static int nxtcharx()

{
	int chr;

	if (hldchar != 0)
	{
		chr = hldchar;
		hldchar = 0;
		return (chr);
	}
	if (xinpnt != NULL)
		return (*xinpnt++);
	chr = nxtchar();
	if (xstrcnt < XSTRMAX)
	{
		xstrcnt++;
		*xstrpnt++ = chr;
	}
	else
		errorx();
	return (chr);
}

