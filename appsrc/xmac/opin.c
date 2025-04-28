//******************************************
// Functions to handle opcode input for XMAC
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
#include <xcmalloc.h>
#include <xos.h>
#include "xmac.h"

#define LX_BB  0
#define LX_W   1
#define LX_L   2

extern char regindx[];
extern char regbits[];
extern char regseg[];
extern int  opcjmpilx;
extern int  opcjmpiwx;


static void icallf2(CL *clp);


// Dispatch table for type of first operand for MOV instruction

void (*movdsp1[])(OPTBL *, CL *) =
{   movreg1,							// OP_REG
    (void (*)(OPTBL *, CL *))adrerr,	// OP_IMM
    movmem1,							// OP_MEM
    movmem1,							// OP_CM1
    movmem1								// OP_CM2
};


//*******************************************
// Function: imovins - Handle MOV instruction
// Returned: Nothing
//*******************************************

void imovins(
	OPTBL *opt,
	CL    *clp)

{
    if ((gccflg) ? (getopr1(&opds, opt, RB_DAT|RB_SEG|RB_CNTL) &&
			getopr(&opdd, opt, RB_DAT|RB_SEG|RB_CNTL)) :
    		(getopr1(&opdd, opt, RB_DAT|RB_SEG|RB_CNTL) &&
			getopr(&opds, opt, RB_DAT|RB_SEG|RB_CNTL)))
		(*movdsp1[opdd.op_type])(opt, clp); // Dispatch on type of destination
}											//   operand


// Dispatch table for destination operand of move instruction when source
//   operand is a register

void (*movregdsp[])(OPTBL *, CL *clp) =
{   movregreg,					// OP_REG
    movregimm,					// OP_IMM
    movregmem,					// OP_MEM
    movregmem,					// OP_CM1
    movregmem					// OP_CM2
};


//************************************************
// Function: movreg1 - Handle MOV instruction when
//				destination operand is a register
// Returned: Nothing
//************************************************

void movreg1(
	OPTBL *opt,
	CL    *clp)


{
    (*movregdsp[opds.op_type])(opt, clp); // Dispatch on type of source operand
}


//***************************************************
// Function: movregreg - Handle MOV instruction when
//				destination operand is a register and
//				source  operand is also a register
// Returned: Nothing
//***************************************************

void movregreg(
	OPTBL *opt,
	CL    *clp)

{
    if (opdd.op_val.val <= RG_ESP && opds.op_val.val <= RG_ESP)
    {									// If have two data registers
        opcdout(opt->ot_val1 | 0x02, &opds, opt, clp); // Output opcode byte
		clp->byte2 = opds.op_modrm +	// Followed by mod-r/m byte
				(regindx[(int)(opdd.op_val.val)] << 3);
		clp->size++;
	}
    else if (opdd.op_val.val <= RG_ESP) // Is destination a data
    {								 	//   register?
        if (opds.op_val.val <= RG_GS)	/// Yes - is source a segment
        {								//   register?
            opcdout(0x8C, &opdd, opt, clp);	// Yes - output opcode byte
            oprdout(regindx[(int)(opds.op_val.val)], &opdd, clp);
        }
        else							// Source must be a control register
            cntlreg(0x00, opds.op_val.val, opdd.op_val.val, clp);
    }
    else if (opds.op_val.val <= RG_ESP) // Is source a data
    {									//   register?
        if (opdd.op_val.val <= RG_GS)	// Yes - is destination a
        {								//   segment register?
            opcdout(0x8E, &opdd, opt, clp);	// Yes - output opcode byte
            oprdout(regindx[(int)(opds.op_val.val)], &opdd, clp);
        }
        else							// Destination must be a control
										//   register
            cntlreg(0x02, opdd.op_val.val, opds.op_val.val, clp);
    }
    else
        adrerr();
}


//**************************************************
// Function: cntlreg - Handle transfers to or from
//				control register for MOV instruction
// Returned: Nothing
//**************************************************

void cntlreg(
	int dbit,
	int creg,
	int dreg,
	CL *clp)

{
    if (creg > RG_DR7 || ptype < P_80386) // Processor must be 386 or 486
    {
        adrerr();
        return;
    }
    clp->byte1 = 0x0F;
	clp->byte2 = ((creg <= RG_CR4) ? 0x20 : (creg <= RG_DR7) ? 0x21 : 0x24) |
			dbit;
    clp->byte3 = 0xC0 | regindx[creg] | regindx[dreg];
	clp->size += 3;
}


//*********************************************************
// Function: movaregimm - Handle MOV instruction when first
//					(destination) operand is a register and
//					second (source) operand is immediate
// Returned: Nothing
//*********************************************************

void movregimm(
	OPTBL *opt,
	CL    *clp)

{
    if (opdd.op_val.val <= RG_ESP) // Data register?
    {
        opcdout(opt->ot_val3 | regindx[(int)(opdd.op_val.val)],
                &opdd, opt, clp);
        valueout(&opds, opt, clp);
    }
    else
        adrerr();						// No - error
}


//*********************************************************
// Function: movregmem - Handle MOV instruction when first
//				(destination) operand is a register and
//				second (source) operand is a memory address
// Returned: Nothing
//*********************************************************

void movregmem(
	OPTBL *opt,
	CL    *clp)

{
    movrmmr(0x02, &opdd, &opds, opt, clp);
}


// Dispatch table for second operand of MOV instruction when first operand
//   is a memory address

void (*movmemdsp[])(OPTBL *, CL *clp) =
{   movmemreg,							// OP_REG
    movmemimm,							// OP_IMM
    (void (*)(OPTBL *, CL *))adrerr,	// OP_MEM
    (void (*)(OPTBL *, CL *))adrerr,	// OP_CM1
    (void (*)(OPTBL *, CL *))adrerr		// OP_CM2
};


//*********************************************************
// Function: movmem1 - Handle MOV instruction when first
//				(destination) operand is a memory reference
// Returned: Nothing
//*********************************************************

void movmem1(
	OPTBL *opt,
	CL    *clp)

{
    (*movmemdsp[opds.op_type])(opt, clp); // Dispatch on type of second operand
}


//********************************************************
// Function: movmemreg - Handle MOV instruction when first
//				(destination) operand is a memory address
//				and second (source) operand is a register
// Returned: Nothing
//********************************************************

void movmemreg(
	OPTBL *opt,
	CL    *clp)

{
    movrmmr(0x00, &opds, &opdd, opt, clp);
}


//********************************************************
// Function: movmemimm - Handle MOV instruction when first
//				(destination) operand is a memory address
//				and second (source) operand is immediate
// Returned: Nothing
//********************************************************

void movmemimm(
	OPTBL *opt,
	CL    *clp)

{
    opcdout(opt->ot_val2, &opdd, opt, clp);
    oprdout(0x00, &opdd, clp);
    valueout(&opds, opt, clp);
}


//***********************************************
// Function: movrmmr - Handle MOV instruction for
//				reg-mem or mem-reg transfer
// Returned: Nothing
//***********************************************

void movrmmr(
	int    dbit,
	OP    *ropd,
	OP    *mopd,
	OPTBL *opt,
	CL    *clp)

{
    if (ropd->op_val.val <= RG_ESP) // Data register?
    {
        if (mopd->op_type == OP_MEM && (ropd->op_val.val == RG_AL ||
                ropd->op_val.val == RG_AX ||
                ropd->op_val.val == RG_EAX))
        {
            opcdout(opt->ot_val4 ^ dbit, mopd, opt, clp);
            storevalue(&clp->val1, (mopd->op_nab != 4) ? VALUE_2_U : VALUE_4_U,
					&mopd->op_val, mopd->exprsn);
			clp->size += mopd->op_nab;
        }
        else
        {
            opcdout(opt->ot_val1 | dbit, mopd, opt, clp); // Output opcode byte
            oprdout(regindx[(int)(ropd->op_val.val)]<<3, mopd, clp);
        }								// Followed by operand byte(s)
    }
    else if (ropd->op_val.val <= RG_GS) // Segment register?
    {
        if (ropd->op_val.val > RG_ES && ptype < P_80386)
            adrerr();
        else
        {
            opcdout(0x8C | dbit, mopd, opt, clp);
            oprdout(regindx[(int)(ropd->op_val.val)], mopd, clp);
        }
    }
    else
        adrerr();
}


//***********************************************************************
// Function: ioneoprf - Handle single operand floating point instructions
//				which expect a floating point register as operand  -
//				operand must be a single floating point register (ST or
//				ST0 - ST7)
// Returned: Nothing
//***********************************************************************

void ioneoprf(
	OPTBL *opt,
	CL    *clp)

{
    stopper = nxtnb0(stopper);			// Make sure have non-blank character
    if (stopper == '\0' || stopper == '}' || stopper == ';') // Have operand?
    {
        clp->byte1 = opt->ot_val1;		// No - operand - default is ST0
        clp->byte2 = opt->ot_val2;
		clp->size = 2;
        return;
    }
    if (getopr(&opdd, opt, RB_FLOAT) && opdd.op_type == OP_REG)
    {
        clp->byte1 = opt->ot_val1;
        clp->byte2 = opt->ot_val2 + regindx[(int)(opdd.op_val.val)];
		clp->size = 2;
        return;
    }
    adrerr();
}


//************************************************************
// Function: ioneoprw - Handle general single operand floating
//				point instructions with an initial wait
// Returned: Nothing
//************************************************************

void ioneoprw(
	OPTBL *opt,
	CL    *clp)

{
    ioneopr(opt, clp);					// Almost like normal instruction
	clp->byte3 = clp->byte2;
	clp->byte1 = clp->byte1;
    clp->byte1 = 0x9B;					// Output WAIT opcode first
	clp->size++;
}


//********************************************************************
// Function: itwooprf - Handle two operand floating point instructions
// Returned: Nothing
//********************************************************************

// These instructions take zero, one, or two floating point registers as
//   operands. If two operands are given, one of them must be ST0.  If one
//   operand is given, it can be any floating point register (this is
//   equivilent to ST0, STn).  If no operand is given, this is equivilent to
//   ST0, ST1. Also if no operand is given, the no-push form of the opcode
//   means the push form most of the time! (FADD is the same as FADDP ST0,
//   ST1, not FADD ST0, ST1, but FCOM is the same as FCOM ST0, ST1!!!!)  If
//   the instruction uses the direction bit, either operand can be ST0.  The
//   direction bit is set if the first operand is not ST0.  If the instruction
//   does not use the direction bit, the first operand must be ST0 if two
//   operands are given.

// Use of the opcode table:
//	ot_val1 = First byte value (normal value + 4 if direction bit is used)
//	ot_val2 = Second byte value
//	ot_val3 = Alternate first byte (used if no operands)
//	ot_val4 = Alternate second byte (used if GCC mode)

void itwooprf(
	OPTBL *opt,
	CL    *clp)

{
	int regs;
    int regd;

    stopper = nxtnb0(stopper);			// Make sure have non-blank character
    if (stopper == '\0' || stopper == '}' || stopper == ';') // Have operand?
    {
        clp->byte1 = opt->ot_val3;		// No - operand - default is ST0, ST1
        clp->byte2 = ((gccflg) ? opt->ot_val4 : opt->ot_val2) + 1;
		clp->size = 2;
        return;
    }
    if (getopr(&opdd, opt, RB_FLOAT) && opdd.op_type == OP_REG)
    {
        if (stopper != ',')				// Have second operand?
        {
            clp->byte1 = opt->ot_val1 - 4; // No
            clp->byte2 = ((gccflg) ? opt->ot_val4 : opt->ot_val2) +
					regindx[(int)(opdd.op_val.val)];
			clp->size = 2;
            return;
        }
        regd = (int)opdd.op_val.val;	// Yes - save first register
        if (getopr(&opdd, opt, RB_FLOAT) && opdd.op_type == OP_REG)
        {
			if (gccflg)
			{
				regs = regd;
				regd = opdd.op_val.val;
			}
			else
				regs = opdd.op_val.val;
            if (regd == RG_ST0 && (opt->ot_flag & OF_DB))
										// If destination operand is ST0, then
            {							//   source operand can be any FPU
										//   register - instruction must use
										//   the direction bit
                clp->byte1 = opt->ot_val1 - 4;
                clp->byte2 = ((gccflg) ? opt->ot_val4 : opt->ot_val2) +
						regindx[regs];
				clp->size = 2;
                return;
            }
            if (regs == RG_ST0)			// If destination operand is not ST0,
			{							//   source operand must be ST0
                clp->byte1 = opt->ot_val1;
                clp->byte2 = ((gccflg) ? opt->ot_val4 : opt->ot_val2) +
						regindx[regd];
				clp->size = 2;
                return;
            }
        }
    }
    adrerr();
}


//********************************************************
// Function: ifstsw - Handle FNSTSW and FSTSW instructions
// Returned: Nothing
//********************************************************

void ifstsw(
	OPTBL *opt,
	CL    *clp)

{
    if (getopr(&opdd, opt, RB_DAT))
    {
        if (opdd.op_type == OP_REG && opdd.op_val.val == RG_AX)
        {
            if (opt->ot_val1)
                clp->byte1 = 0x9B;
            clp->byte2 = 0xDF;
			clp->byte3 = 0xE0;
        }
        else if (opdd.op_type >= OP_MEM)
        {
			if (opt->ot_val1)
				clp->byte0 = 0x9B;
			opcdout(0xDD, &opdd, opt, clp);
			oprdout(0x38, &opdd, clp);
        }
        else
            adrerr();
    }

}


//***************************************************************
// Function: ioneopr - Handle general single operand instructions
// Returned: Nothing
//***************************************************************

void ioneopr(
	OPTBL *opt,
	CL    *clp)

{
    if (getopr(&opdd, opt, RB_DAT))
    {
        if ((opdd.op_type == OP_REG && (opt->ot_flag & OF_RS)) ||
                (opdd.op_type >= OP_MEM && (opt->ot_flag & OF_MS)))
        {
            opcdout(opt->ot_val1, &opdd, opt, clp);
            oprdout(opt->ot_val4, &opdd, clp);
        }
        else
            adrerr();
    }
}


// Dispatch table for type of first operand for general two operand
//   instructions

void (*op2dsp1[])(OPTBL *, CL *) =
{   op2reg1,							// OP_REG
    (void (*)(OPTBL *, CL *))adrerr,	// OP_IMM
    op2mem1,							// OP_MEM
    op2mem1,							// OP_CM1
    op2mem1								// OP_CM2
};


//************************************************************
// Function: itwoopr - Handle general two operand instructions
// Returned: Nothing
//************************************************************

void itwoopr(
	OPTBL *opt,
	CL    *clp)

{
    if ((gccflg) ? (getopr1(&opds, opt, RB_DAT) && getopr(&opdd, opt, RB_DAT)) :
    		(getopr1(&opdd, opt, RB_DAT) && getopr(&opds, opt, RB_DAT)))
        (*op2dsp1[opdd.op_type])(opt, clp); // Dispatch on type of destination
}											//   operand


// Dispatch table for second operand of two operand instruction when first
//   operand is a register

void (*op2regdsp[])(OPTBL *, CL *) =
{   op2regreg,					// OP_REG
    op2regimm,					// OP_IMM
    op2regmem,					// OP_MEM
    op2regmem,					// OP_CM1
    op2regmem					// OP_CM2
};


//********************************************************
// Function: op2reg1 - Handle two operand instruction when
//				first (destination) operand is a register
// Returned: Nothing
//********************************************************

void op2reg1(
	OPTBL *opt,
	CL    *clp)

{
    if (opt->ot_flag & OF_RD)
        (*op2regdsp[opds.op_type])(opt, clp); // Dispatch on type of 2nd operand
    else
        adrerr();
}


//****************************************************************
// Function: op2regreg - Handle two operand instruction when first
//				(destination) operand is a register and second
//				(source) operand is also a register
// Returned: Nothing
//****************************************************************

void op2regreg(
	OPTBL *opt,
	CL    *clp)

{
///	clp->size = 1;
    if (!(opt->ot_flag & OF_RS))		// Can the source be a register?
        adrerr();						// No - fail
    else
    {
        if (opt->ot_flag & OF_RF)		// Is the first operand the register?
        {				// Yes
            opcdout(opt->ot_val1, &opdd, opt, clp); // Store opcode byte
            clp->byte2 = (opds.op_modrm +
                    (regindx[(int)(opdd.op_val.val)]<<3));
			clp->size++;
        }
        else
        {
            if (opt->ot_flag & OF_DB)	// Does it use a direction bit?
            {							// Yes - output it with the direction
										//   bit set
                opcdout(opt->ot_val1 | 0x02, &opdd, opt, clp); // Store opcode
                clp->byte2 = (opds.op_modrm + // Followed by mod-r/m byte
                        (regindx[(int)(opdd.op_val.val)]<<3));
				clp->size++;
            }
            else						// If does not use direction bit
            {
                opcdout(opt->ot_val1, &opds, opt, clp); // Store opcode byte
                clp->byte2 = (opdd.op_modrm + // Followed by mod-r/m byte
                        (regindx[(int)(opds.op_val.val)]<<3));
				clp->size++;
            }
        }
    }
}


//*****************************************************
// Function: op2regimm - Handle two operand instruction
//				when destination operand is a register
//				and source operand is immediate
// Returned: Nothing
//*****************************************************

void op2regimm(
	OPTBL *opt,
	CL    *clp)

{
	if (opds.op_val.type == VLT_VAL && opds.op_val.kind == VLK_ABS &&
			(opds.op_val.flags & VLF_UNDEF) == 0 && opds.op_val.val < 0x80 &&
			opds.op_val.val >= -0x80)
	{
		opds.op_val.type = VLT_BYTE;	// Use 8 bit literal value if we can
		opds.op_val.size = 1;
	}
    if (opt->ot_val4 && (opdd.op_val.val == RG_AL ||
            (((opdd.op_val.val == RG_AX) ||
            (opdd.op_val.val == RG_EAX)) && 
            (!(opt->ot_flag & OF_SB) || (opds.op_val.size > 1)))))
										// Is destination AL, AX, or EAX and
										//   we have a special form for this
										//   and we want to use it? (we do
										//   not use the special form for
										//   ADDL EAX, #n.B since it is
    {									//   longer than the normal form!
        opcdout(opt->ot_val4, &opdd, opt, clp); // Yes
        valueout(&opds, opt, clp);
    }
    else
        op2memimm(opt, clp);			// No - just like memory immediate
}


//**********************************************************
// Function: op2regmem - Handle two operand instruction when
//			first (destination) operand is a register and
//			second (source) operand is a memory address
// Returned: Nothing
//**********************************************************

void op2regmem(
	OPTBL *opt,
	CL    *clp)

{
    if (!(opt->ot_flag & OF_MS))		// Is a memory source legal here?
    {
        adrerr();						// No - error
        return;
    }
    opcdout(opt->ot_val1 | ((opt->ot_flag & OF_DB)? 2: 0), &opds, opt, clp);
										// Yes - output opcode byte
    oprdout(regindx[(int)(opdd.op_val.val)]<<3, &opds, clp);
										// Followed by operand byte(s)
}

// Dispatch table for second operand of two operand instruction when first
//   operand is a memory address

void (*op2memdsp[])(OPTBL *, CL *) =
{   op2memreg,							// OP_REG
    op2memimm,							// OP_IMM
    (void (*)(OPTBL *, CL *))adrerr,	// OP_MEM
    (void (*)(OPTBL *, CL *))adrerr,	// OP_CM1
    (void (*)(OPTBL *, CL *))adrerr		// OP_CM2
};


//**************************************************************
// Function: op2mem1 - Handle two operand instruction when first
//				(destination) operand is a memory reference
// Returned: Nothing
//**************************************************************

void op2mem1(
	OPTBL *opt,
	CL    *clp)

{
    if (!(opt->ot_flag & OF_MD))		// Can destination be memory?
    {
        adrerr();						// No - fail
        return;
    }
    (*op2memdsp[(int)(opds.op_type)])(opt, clp); // Dispatch on type of second
}												 //   operand


//*************************************************************
// Function: op2memreg - Handle two operand instruction when
//				first (destination) operand is a memory address
//				and second (source) operand is a register
// Returned: Nothing
//*************************************************************

void op2memreg(
	OPTBL *opt,
	CL    *clp)

{
    if (!(opt->ot_flag & OF_MD))		// Is memory destination legal here?
    {
        adrerr();						// No - error
        return;
    }
    opcdout(opt->ot_val1, &opdd, opt, clp);	// Yes - output opcode byte
    oprdout(regindx[(int)(opds.op_val.val)]<<3, &opdd, clp);
}										// Followed by operand byte(s)


//**********************************************************
// Function: op2memimm - Handle two operand instruction
//				when destination operand is a memory address
//				and source operand is immediate
// Returned: Nothing
//**********************************************************


void op2memimm(
	OPTBL *opt,
	CL    *clp)

{
    if (!opt->ot_val2)					// Is immediate mode value legal?
    {
        adrerr();						// No - error
        return;
    }
    if (opt->ot_flag & OF_BI)			// Does this instruction always get a
    {									//   single byte immediate value?
        opcdout(opt->ot_val2, &opdd, opt, clp); // Yes
        oprdout(opt->ot_val3, &opdd, clp);
		storevalue(&clp->val2, VALUE_1_U, &opds.op_val, opds.exprsn);
		clp->size++;
    }
    else if (opt->ot_flag & OF_SB)		// Does this instruction use the
    {									//   size bit?
		if (opds.op_val.type == VLT_VAL && opds.op_val.kind == VLK_ABS &&
				(opds.op_val.flags & VLF_UNDEF) == 0 &&
				opds.op_val.val < 0x80 && opds.op_val.val >= -0x80)
		{
			opds.op_val.type = VLT_BYTE; // Use 8 bit literal value if we can
			opds.op_val.size = 1;
		}
		opcdout(opt->ot_val2 | ((opds.op_val.size == 1) ? 2 : 0), // Yes
                &opdd, opt, clp);
        oprdout(opt->ot_val3, &opdd, clp);
        valuesout(&opds, opt, clp);
    }
    else								// Does not use size bit
    {
        opcdout(opt->ot_val2, &opdd, opt, clp);
        oprdout(opt->ot_val3, &opdd, clp);
        valueout(&opds, opt, clp);
    }
}


//***********************************************
// Function: istrins - Handle string instructions
// Returned: Nothing
//***********************************************

// If this is a repeated string instruction, the repeat prefix is output
//   first so this can be used together with a segment prefix on processors
//   which don't handle multiple prefixes quite right!

void istrins(
	OPTBL *opt,
	CL    *clp)

{
    char source;
    char destination;
    char both;
    char wide;

    source = ((opt->ot_flag & OF_MS) != 0);
    destination = ((opt->ot_flag & OF_MD) != 0);
    both = source & destination;
    opdd.op_seg = 0;
    opds.op_seg = 0;
	if (!eolflg)
	{
    	if (!(both ? ((gccflg) ?
					(getopr1(&opds, opt, 0) && getopr(&opdd, opt, 0)) :
					(getopr1(&opdd, opt, 0) && getopr(&opds, opt, 0))) :
				getopr(destination? &opdd: &opds, opt, 0)))
			return;
	}
	else
	{
		if (segatrb & SA_32BIT)
		{
			opds.op_asize = 4;
			opdd.op_asize = 4;
			opds.op_modrm = 6;
			opdd.op_modrm = 7;
		}
		else
		{
			opds.op_asize = 2;
			opdd.op_asize = 2;
			opds.op_modrm = 4;
			opdd.op_modrm = 5;
		}
	}
	if (both)
	{
		wide = (opdd.op_asize == 4);
		if (wide != (opds.op_asize == 4))
		{
			adrerr();
			return;
		}
	}
	else
		wide = ((source? opds.op_asize: opdd.op_asize) == 4);
	if ((source && opds.op_modrm != ((wide) ? 6 : 4)) ||
			(destination && (opdd.op_modrm != ((wide)? 7 : 5)) || opdd.op_seg))
	{
		adrerr();
		return;
	}
	if (opt->ot_val2)				// Need repeat prefix byte?
	{
		clp->repeat = opt->ot_val2;	// Yes
		clp->size++;
	}
	if (wide ^ ((segatrb & SA_32BIT) != 0))
	{								// Need address size prefix byte?
		clp->flags |= CLF_AS;		// Yes
		clp->size++;
	}
	if (opt->ot_size > 1)			// Might this instruction need a size
										//   prefix?
	{								// Yes - does it really need one?
		if ((opt->ot_size == 2) ^ ((segatrb & SA_32BIT) == 0))
		{
			clp->flags |= CLF_DS;	// Yes
			clp->size++;
		}
	}
	if ((clp->seg = opds.op_seg) != 0) // Store segment prefix byte, if any
		clp->size++;
	clp->byte1 = opt->ot_val1;		// Store opcode byte
	clp->size++;
}


//**************************************************
// Function: ientrins - Handle the ENTER instruction
// Returned: Nothing
//**************************************************

void ientrins(
	OPTBL *opt,
	CL    *clp)

{
    if (getopr(&opdd, opt, 0))			// Get first operand
    {
        if (opdd.op_type == OP_MEM)
        {
            if (stopper == ',')			// Have second operand?
            {
                if (getopr(&opds, opt, 0)) // Yes - get it
                {
                    if (opds.op_type != OP_MEM)
                    {
                        adrerr();
                        return;
                    }
                }
                else
				{
					queerr();
                    return;
				}
            }
            else
			{
				opds.op_val.val = 0;
				opds.op_val.type = VLT_VAL;
				opds.op_val.kind = VLK_ABS;
				opds.op_type = OP_MEM;

///				queerr();
///             return;
			}

			clp->size = 4;
            if (stack32 ^ ((segatrb & SA_32BIT) != 0))
			{							// Need operand size prefix?
				clp->flags |= CLF_DS;	// Yes
				clp->size++;
			}
            clp->byte1 = 0xC8;
			storevalue(&clp->val1, VALUE_2_U, &opdd.op_val, opdd.exprsn);
			storevalue(&clp->val2, VALUE_1_U, &opds.op_val, opds.exprsn);
        }
        else
            adrerr();
    }
}


//**********************************************
// Function: iimul - Handle the IMUL instruction
// Returned: Nothing
//**********************************************

void iimul(
	OPTBL *opt,
	CL    *clp)

{
	int rtn;

    if (!getopr(&opdd, opt, RB_DAT))	// Get first operand
        return;
    if (stopper != ',')					// Single operand?
    {									// Yes
        opcdout(opt->ot_val1, &opdd, opt, clp);
        oprdout(0x20, &opdd, clp);
		return;
    }

	// Here if more than one operand

	if (gccflg)							// Get second operand
	{
		opds = opdd;
		rtn = getopr(&opdd, opt, RB_DAT);
	}
	else
		rtn = getopr(&opds, opt, RB_DAT);
	if (!rtn)
		return;

	// Cannot be byte and must be for 80286 or above

	if (opt->ot_size == 1 || ptype < P_80286)
	{
		adrerr();
		return;
	}
	if (stopper != ',')					// Only two operands?
	{
		if (opds.op_type == OP_IMM)	// Yes - is source immediate?
			iimul3op(&opdd, &opdd, &opds, opt, clp); // Yes - really a version
													 //   of the 3 operand form
		else
		{
			if (opdd.op_type != OP_REG || ptype < P_80386)
			{							// General 2 operand form is only
				adrerr();				//   available on the 80386
                return;
			}
			clp->byte0 = 0x0F;			// Store prefix byte
			clp->size++;
			opcdout(0xAF, &opds, opt, clp); // Store 1st opcode byte
			oprdout(regindx[(int)(opdd.op_val.val)]<<3, &opds, clp);
		}
		return;
	}

	// Here if have three operands

	if (gccflg)							// Get third operand
	{
		opdi = opds;
		opds = opdd;
		rtn = getopr(&opdd, opt, RB_DAT);
	}
	else
		rtn = getopr(&opdi, opt, 0);
	if (!rtn)
		return;
	if (opdi.op_type != OP_IMM) 		// Must be immediate
	{
		adrerr();
		return;
	}
	iimul3op(&opdd, &opds, &opdi, opt, clp);
}


//*********************************************
// Function: iimul3op - Generate code for three
//				operand multiply instruction
// Returned: Nothing
//*********************************************

void iimul3op(
	OP    *dopd,
	OP    *sopd,
	OP    *iopd,
	OPTBL *opt,
	CL    *clp)

{
	if (iopd->op_val.type == VLT_VAL && iopd->op_val.kind == VLK_ABS &&
			(iopd->op_val.flags & VLF_UNDEF) == 0 && iopd->op_val.val < 0x80 &&
			iopd->op_val.val >= -0x80)
	{
		iopd->op_val.type = VLT_BYTE;	// Use 8 bit literal value
		iopd->op_val.size = 1;			//   if we can
	}
    opcdout(0x69 | ((iopd->op_val.size == 1) ? 2 : 0), sopd, opt, clp);
    oprdout(regindx[(int)(dopd->op_val.val)] << 3, sopd, clp);
										// Followed by operand byte(s)
    valuesout(iopd, opt, clp);			// Followed by immediate value
}


//******************************************
// Function: iinout - Handle IO instructions
// Returned: Nothing
//******************************************

void iinout(
	OPTBL *opt,
	CL    *clp)

{
	VL val;

    stopper = nxtnbc();					// Get first character
    if (stopper == inxbgn)				// Might it be [DX]?
    {
        getatom(&val);					// Yes - get expression
        if (stopper != inxend || val.kind != VLK_REG || val.val != RG_DX)
        {
            adrerr();
            return;
        }
        stopper = nxtnbc();
        iinoutop(opt->ot_val2, opt, clp);
    }
    else
    {
        hldchar = stopper;
        exprsn(&val);					// Evalulate port number expression
        iinoutop(opt->ot_val1, opt, clp);
		storevalue(&clp->val1, VALUE_1_U, &val, &xstrbufr);
		clp->size++;
    }
}

// Function to output opcode byte for IN or OUT instructions

void iinoutop(
	int    byte,
	OPTBL *opt,
	CL    *clp)

{
    if (opt->ot_size > 1 &&
            ((opt->ot_size == 2) ^ ((segatrb & SA_32BIT) == 0)))
	{
		clp->flags |= CLF_DS;
		clp->size++;
	}
    clp->byte1 = byte;
	clp->size++;
}


//****************************************************
// Function: iincdec - Handle INC and DEC instructions
// Returned: Nothing
//****************************************************

void iincdec(
	OPTBL *opt,
	CL    *clp)

{
    if (!getopr(&opdd, opt, RB_DAT))	// Get operand
    {
	    adrerr();
		return;
	}
	if (opdd.op_type == OP_REG && opt->ot_size != 1)
		opcdout(opt->ot_val2 + regindx[(int)(opdd.op_val.val)], &opdd,
				opt, clp);
	else
	{
		opcdout(opt->ot_val1, &opdd, opt, clp);
		oprdout(opt->ot_val4, &opdd, clp);
	}
}


//**************************************************************
// Function: iclear - Handle CLR instruction (this is not really
//				an instruction, it generates XOR reg,reg!)
// Returned: Nothing
//**************************************************************

void iclear(
	OPTBL *opt,
	CL    *clp)

{
    if (!getopr(&opdd, opt, RB_DAT) || !opdd.op_type == OP_REG)
	{
		adrerr();
		return;
	}
	opcdout(opt->ot_val1, &opdd, opt, clp);
	oprdout(0xC0 + (regindx[(int)(opdd.op_val.val)] << 3), &opdd, clp);
}


//**********************************************************
// Function: ipushpop - Handle the PUSH and POP instructions
// Returned: Nothing
//**********************************************************

// optbl fields are used as follows:
//   ot_val1 - First opcode byte if general memory operand
//   ot_val2 - Opcode byte if general register operand
//   ot_val3 - Opcode byte if short form segment register operand
//   ot_val4 - Second byte if general memory operand

void ipushpop(
	OPTBL *opt,
	CL    *clp)

{
    if (getopr(&opdd, opt, RB_DAT|RB_SEG)) // Get operand
    {
        if (opdd.op_type == OP_REG)
        {
            if (opdd.op_size == 1)
                sizerr();
            else
            {
                if (opdd.op_val.val < RG_CS) // Is this a segment register?
                    opcdout(regindx[(int)(opdd.op_val.val)] +
                            opt->ot_val2, &opdd, opt, clp);
										// No - general register
                else if (opdd.op_val.val < RG_FS) // Yes - is it a 80386 segment reg?
                    opcdout(regindx[(int)(opdd.op_val.val)] +
                            opt->ot_val3, &opdd, opt, clp);
										// No - non-80386 segment register
                else
                {						// Yes - this is a 80386 segment reg
                    if (ptype >= P_80386) // Yes - is this a 386 or 486?
                    {					// Yes - need size prefix?
                        if ((opt->ot_size == 2) ^ ((segatrb & SA_32BIT) == 0))
						{
							clp->flags |= CLF_DS; // Yes
							clp->size++;
						}
                        clp->byte0 = 0x0F;
                        clp->byte1 = ((opt->ot_val3 & 0x01) | 0x80) |
                                regindx[(int)(opdd.op_val.val)];
						clp->size += 2;
                    }
                    else
                        adrerr();		// Not 80386 - error
                }
            }
        }
        else if (opdd.op_type == OP_IMM) // If immediate operand
        {
            if ((unsigned char)(opt->ot_val1) == 0xFF && ptype >= P_80186)
            {							// Must be PUSH and at least a 80186
				if (opdd.op_val.type == VLT_VAL &&
						opdd.op_val.kind == VLK_ABS &&
						(opdd.op_val.flags & VLF_UNDEF) == 0 &&
						opdd.op_val.val < 0x80 && opdd.op_val.val >= -0x80)
				{
					opdd.op_val.type = VLT_BYTE; // Use 8 bit literal value
					opdd.op_val.size = 1;		 //   if we can
				}
                opcdout((opdd.op_val.size == 1) ? 0x6A: 0x68, &opdd, opt, clp);
                valuesout(&opdd, opt, clp);
            }
            else
                adrerr();
        }
        else							// If memory operand
        {
            opcdout(opt->ot_val1, &opdd, opt, clp);
            oprdout(opt->ot_val4, &opdd, clp);
        }
    }
}

// Function to handle the XCHG instruction

void ixchg(
	OPTBL *opt,
	CL    *clp)

{
    if ((gccflg) ? (getopr1(&opdd, opt, RB_DAT) && getopr(&opds, opt, RB_DAT)) :
    		(getopr1(&opdd, opt, RB_DAT) && getopr(&opds, opt, RB_DAT)))
    {
        if (opdd.op_type != OP_REG && opds.op_type != OP_REG)
        {
            adrerr();
            return;
        }
        if (opdd.op_type == OP_REG && opds.op_type == OP_REG)
        {
            if (opdd.op_val.val == RG_AX ||
                    opdd.op_val.val == RG_EAX)
            {
                opcdout(0x90 | regindx[(int)(opds.op_val.val)],
                        &opds, opt, clp);
                return;
            }
            if (opds.op_val.val == RG_AX || opds.op_val.val == RG_EAX)
            {
                opcdout(0x90 | regindx[(int)(opdd.op_val.val)],
                        &opdd, opt, clp);
                return;
            }
        }
        if (opdd.op_type == OP_REG)
        {
            opcdout(opt->ot_val1, &opds, opt, clp); // Output opcode byte
            oprdout(regindx[(int)(opdd.op_val.val)] << 3, &opds, clp);
        }								// Followed by operand byte(s)
        else
        {
            opcdout(opt->ot_val1, &opdd, opt, clp); // Yes - output opcode byte
            oprdout(regindx[(int)(opds.op_val.val)] << 3, &opdd, clp);
        }								// Followed by operand byte(s)
    }
}


//*******************************************
// Function: icalli - Handle CALL instruction
// Returned: Nothing
//*******************************************

// If in GCC mode, this may really be a CALLI instruction

void icalli(
	OPTBL *opt,
	CL    *clp)

{
	if (gccflg && stopper == '*')		// Is this a GCC CALLI instruction
	{
		nxtnbc();						// Yes - discard the *
		ioneopr(&opctjmpx[1], clp);		// Just like a CALLI
    }
	else
		ijump(opt, clp);				// Normal CALL
}


//******************************************
// Function: ijumpi - Handle JMP instruction
// Returned: Nothing
//******************************************

// If in GCC mode, this may really be a JMPIx instruction

void ijumpi(
	OPTBL *opt,
	CL    *clp)

{
	if (gccflg && stopper == '*')		// Is this a GCC JMPIx or CALLI
	{									//   instruction?
		nxtnbc();						// Yes - discard the *
		opctjmpx[0].ot_size = 0;

	    if (getopr(&opdd, &opctjmpx[0], RB_DAT))
	    {
			if (opdd.op_val.size == 2 || opdd.op_val.size == 4)
				opctjmpx[0].ot_size = opdd.op_val.size;

			if (opdd.op_type == OP_REG || opdd.op_type >= OP_MEM)
			{
				opcdout(opctjmpx[0].ot_val1, &opdd, &opctjmpx[0], clp);
				oprdout(opctjmpx[0].ot_val4, &opdd, clp);
			}
		}
		else
			adrerr();
    }
	else
		ijump(opt, clp);				// Normal JMP
}


//***************************************************
// Function: ijump - Handle general jump instructions
// Returned: Nothing
//***************************************************

// Usage of the optable fields are as follows:
//   ot_val1 - First opcode byte for 8 bit displacement form (0 if not
//		    available)
//   ot_val2 - Second opcode byte for 8 bit displacement forms (0 if only
//		    one opcode byte is needed)
//   ot_val3 - First opcode byte for 16 or 32 bit displacement forms (0 if
//		    not available)
//   ot_val4 - Second opcode byte for 16 or 32 bit displacement forms (0 if
//		    only one opcode byte is needed)

void ijump(
	OPTBL *opt,
	CL    *clp)

{
	char jtype;

	if (getopr(&opdd, opt, 0))			// Get operand
	{
		if (gccflg && opt->ot_dsp == icalli && (opdd.op_val.flags & VLF_FFAR))
		{								// If in GCC mode and the address is
			icallf2(clp);				//   FAR, make this a CALLF! This is
			return;						//   a kludge to allow GCC to generate
		}								//   XOS svcs even though it does not
										//   know about the large model!!
		opdd.op_val.flags |= VLF_REL;
        if (opdd.op_type == OP_MEM)
		{
			if (opdd.op_val.type == VLT_VAL) // Size specified?
			{
				if (opdd.op_val.kind < VLK_EXT && // No
						(opdd.op_val.psect == 0 ||
						opdd.op_val.psect == curpsd->pd_psn))
										// External type value or reference to
										//   different psect?
				{						// No - we also get here if this is a
										//   forward reference and we don't
										//   know the psect yet. We take care
										//   of this case when expanding fields.
					if (opt->ot_val1 != 0 && opt->ot_val3 != 0 &&
							(ptype >= P_80386 || !(opt->ot_flag & OF_B3)))
										// No - have both 8-bit 16/32-bit form?
					{					// Yes - make it variable if need to
						if (segatrb & SA_32BIT) // 32-bit segment?
							opdd.op_val.type = VLT_LONG; // Yes - use 32 bits
						else
							opdd.op_val.type = VLT_WORD; // No - use 16 bits

						opdd.op_val.size = 1; // But make the size 1 for now!
						clp->type = CLT_VAR; // Indicate variable length
                	}
                	else				// Don't have both 8-bit and 16/32-bit
					{					//   forms - use the one we have
						if (opt->ot_val1 != 0)
						{
	                    	opdd.op_val.type = VLT_BYTE;
							opdd.op_val.size = 1;

						}
						else
						{
							if (segatrb & SA_32BIT) // 32-bit segment?
							{
								opdd.op_val.type = VLT_LONG; // Yes - use
								opdd.op_val.size = 4;		 //   32 bits
							}
							else
							{
								opdd.op_val.type = VLT_WORD; // No - use
								opdd.op_val.size = 2;		 //   16 bits
							}
						}
                	}
				}
				else					// If external or imported value
				{
					if (segatrb & SA_32BIT) // 32-bit segment?
					{
						opdd.op_val.type = VLT_LONG; // Yes - use 32 bits
						opdd.op_val.size = 4;
					}
					else
					{
						opdd.op_val.type = VLT_WORD; // No - use 16 bits
						opdd.op_val.size = 2;
					}
				}
            }
			if (opdd.op_val.type == VLT_BYTE)
			{
				if (opt->ot_val1 == 0)
				{
					adrerr();
					return;
				}
            }
            else
            {
                if (opt->ot_val3 == 0)
                {
                    adrerr();
                    return;
                }
            }
            if (opt->ot_flag & OF_SS)	// Is this a CALLxx?
            {							// Yes - must make width match stack
										//   size! - ignore operand size!!
				clp->type = CLT_FIXED;
                jtype = (stack32) ? VLT_LONG : VLT_WORD;
                if ((stack32 ^ ((segatrb & SA_32BIT) != 0)))
				{						// Need operand size prefix?
                    clp->flags |= CLF_DS; // Yes
					clp->size++;
				}
            }
            else
            {
				jtype = opdd.op_val.type;
				if ((jtype != VLT_BYTE) && // Need size prefix?
                        ((jtype == VLT_WORD) ^ ((segatrb & SA_32BIT) == 0)))
                {
                    if (ptype >= P_80386) // Yes - can we do this?
					{
                        clp->flags |= CLF_AS; // Yes
						clp->size++;
					}
                    else
                    {
                        adrerr();
                        return;
                    }
                }
			}
			if (opdd.op_val.size == 1)	// If byte offset or variable size
            {
                clp->byte1 = opt->ot_val1;
                if (opt->ot_val2 != 0)
				{
					clp->byte2 = opt->ot_val2;
					clp->size++;
				}
				storevalue(&clp->val1, VALUE_1_S, &opdd.op_val, opdd.exprsn);
				clp->size += 2;


/*				if (clp->type == CLT_VAR &&
						(opdd.op_val.flags & VLF_UNDEF) == 0)
				{
					diff = opdd.op_val.val + clp->size - curpsd->pd_ofs;
					if (diff <= 0x2A && diff >= -0x2A)
						clp->type = CLT_FIXED;
					else if (diff >= 0x80 || diff < -0x80)
						expandjump(clp);
				}
*/
            }
            else
            {
                clp->byte1 = opt->ot_val3;
                if (opt->ot_val4 != 0)
				{
					clp->byte2 = opt->ot_val4;
					clp->size++;
				}
                if (jtype == VLT_WORD)
                {
                    storevalue(&clp->val1, VALUE_2_U, &opdd.op_val,
							opdd.exprsn);
					clp->size += 3;
                }
                else
                {
                    storevalue(&clp->val1, VALUE_4_U, &opdd.op_val,
							opdd.exprsn);
					clp->size += 5;
                }
            }
        }
        else
            adrerr();
    }
}


//*******************************************
// Function: iloop - Handle loop instructions
// Returned: Nothing
//*******************************************

// Usage of the optable fields are as follows:
//   ot_val1 - Opcode byte

void iloop(
	OPTBL *opt,
	CL    *clp)

{
    if (getopr(&opdd, opt, RB_DAT))		// Get first operand
    {
        if (opdd.op_type == OP_REG && (opdd.op_val.val == RG_CX
                || opdd.op_val.val == RG_ECX))
        {
            if (getopr(&opds, opt, 0))	// Get second operand
            {
                if (opds.op_type == OP_MEM)
                {
					opds.op_val.type = VLT_BYTE; // Always use 8 bit form
					opds.op_val.flags |= VLF_REL;
                    opds.op_val.size = 1;
                    if ((opdd.op_val.val == RG_CX)
							^ ((segatrb & SA_32BIT) == 0))
                    {					// Need address size prefix?
                        if (ptype >= P_80386) // Yes - can we do this?
						{
							clp->flags |= CLF_AS; // Yes
							clp->size++;
						}
                        else
                        {
                            adrerr();
                            return;
                        }
                    }
                    clp->byte1 = opt->ot_val1;
					storevalue(&clp->val1, VALUE_1_S, &opds.op_val,
							opds.exprsn);
                }
            }
			clp->size += 2;
        }
        else
            adrerr();
    }
}


//***********************************************
// Function: ijumpf - Handle the JMPF instruction
// Returned: Nothing
//***********************************************

void ijumpf(
	OPTBL *opt,
	CL    *clp)

{
	uchar offset16;

    if (!getopr(&opdd, opt, RB_DAT))	// Get operand
	{
		adrerr();
		return;
	}
	if (opdd.op_type == OP_MEM)
	{
		if (opdd.op_val.type == VLT_VAL) // Size specified?
		{
			if (segatrb & SA_32BIT)		// No - 32 bit segment?
			{
				opdd.op_val.type = VLT_LONG; // Yes - use 32 bits
				opdd.op_val.size = 4;
			}
			else
			{
				opdd.op_val.type = VLT_WORD; // No - use 16 bits
				opdd.op_val.size = 2;
			}
		}
		offset16 = (opdd.op_val.type != VLT_LONG);
		if (offset16 ^ ((segatrb & SA_32BIT) == 0))
		{
			if (ptype >= P_80386)		// Yes - can we do this?
			{
				clp->flags |= CLF_DS;	// Yes
				clp->size++;
			}
			else
			{
				adrerr();
				return;
			}
		}
		opdd.op_val.flags |= VLF_FAR;
		clp->byte1 = 0xEA;
		storevalue(&clp->val1, (offset16) ? VALUE_2_A : VALUE_4_A,
				&opdd.op_val, opdd.exprsn);
		clp->size += ((offset16) ? 5 : 7);
	}
}


//************************************************
// Function: icallf - Handle the CALLF instruction
// Returned: Nothing
//************************************************

void icallf(
	OPTBL *opt,
	CL    *clp)

{
    if (getopr(&opdd, opt, RB_DAT))		// Get operand
		icallf2(clp);
}


void icallf2(
	CL *clp)    

{
    if (opdd.op_type == OP_MEM)
	{
		clp->val1.value.flags |= VLF_FAR;
		if ((stack32 ^ ((segatrb & SA_32BIT) != 0)))
		{							// Need operand size prefix?
			clp->flags |= CLF_DS; // Yes
			clp->size++;
		}
		clp->byte1 = 0x9A;
		storevalue(&clp->val1, (stack32) ? VALUE_4_A : VALUE_2_A,
				&opdd.op_val, opdd.exprsn);
		clp->val1.value.flags |= VLF_FAR;
		clp->size += ((stack32) ? 7 : 5);
	}
	else
		adrerr();
}


//*******************************************
// Function: iintins - Handle INT instruction
// Returned: Nothing
//*******************************************

void iintins(
	OPTBL *opt,
	CL    *clp)

{
    if (getopr(&opdd, opt, 0))			// Get operand
    {   
        if (opdd.op_type == OP_MEM && opdd.op_val.type == VLT_VAL)
        {
			clp->byte1 = 0xCD;
			storevalue(&clp->val1, VALUE_1_U, &opdd.op_val, opdd.exprsn);
			clp->size = 2;
        }
        else
            adrerr();
    }
}


//********************************************************
// Function: ishrot - Handle shift and rotate instructions
// Returned: Nothing
//********************************************************

void ishrot(
	OPTBL *opt,
	CL    *clp)

{
	int savesize;
	int rtn;

	savesize = opt->ot_size;
	if (gccflg)
	{
		opt->ot_size = 0;
	    rtn = getopr(&opds, opt, RB_DAT); // Get shift amount
		opt->ot_size = savesize;
		if (!rtn)
			return;
		if (stopper != 0 && stopper != ',')
		{
			errflag |= ERR_Q;
			return;
		}
		if (stopper != ',')				// Single operand form?
		{
			opcdout(opt->ot_val1, &opds, opt, clp);
			oprdout(opt->ot_val4, &opds, clp);
			return;
		}

		// Here if have second (destination) argument

		if (!getopr(&opdd, opt, RB_DAT)) // Get destination
			return;
	}
	else
	{
	    if (!getopr(&opdd, opt, RB_DAT)) // Get destination operand
			return;
		if (stopper != ',')				// Single operand form?
		{
			opcdout(opt->ot_val1, &opdd, opt, clp); // Yes
			oprdout(opt->ot_val4, &opdd, clp);
			return;
		}
		opt->ot_size = 0;
	    rtn = getopr(&opds, opt, RB_DAT); // Get shift amount
		opt->ot_size = savesize;
		if (!rtn)
			return;
	}

	// Here if have two operand form

	if (ptype < P_80286)
	{
		adrerr();
		return;
	}
	if (opds.op_type == OP_IMM)			// Immediate shift amount?
	{

		if (opds.op_val.kind == VLK_ABS &&	// Is it 1?
				(opds.op_val.flags & VLF_UNDEF) == 0 && opds.op_val.val == 1)
		{
			opcdout(opt->ot_val1, &opdd, opt, clp);
			oprdout(opt->ot_val4, &opdd, clp);
		}
		else
		{
			opcdout(opt->ot_val3, &opdd, opt, clp);
			oprdout(opt->ot_val4, &opdd, clp);
			storevalue(&clp->val2, VALUE_1_U, &opds.op_val, xstrbufr);
			clp->size++;
		}
	}
	else
	{
		if (opds.op_type != OP_REG || opds.op_val.val != RG_CL)
		{
			adrerr();
			return;
		}
		opcdout(opt->ot_val2, &opdd, opt, clp); // Output opcode byte
		oprdout(opt->ot_val4, &opdd, clp);
	}
}


//***************************************************************
// Function: idblshf - Handle double precision shift instructions
// Returned: Nothing
//***************************************************************

void idblshf(
	OPTBL *opt,
	CL    *clp)
{
	VL   val;
	char bufr[XSTRMAX + 1];

	if (gccflg)
	{
		if (stopper != litchr)			// Immediate value?
		{
			getatom(&val);				// No - must be CL
            if (val.kind != VLK_REG || val.val != RG_CL)
            {
                adrerr();
                return;
            }
		    if (!getopr1(&opds, opt, RB_DAT) ||	// Get following operands
        		    !getopr(&opdd, opt, RB_DAT))
				return;
            opcdout(opt->ot_val1, &opdd, opt, clp); // Output opcode byte
            oprdout(regindx[(int)(opds.op_val.val)] << 3, &opdd, clp);
		}
		else
		{
			nxtnbc();					// Discard the $ or #
            exprsn(&val);				// Get immediate value
			strcpy(bufr, xstrbufr);
			if (stopper != ',')
			{
				errflag |= ERR_Q;
				return;
			}
		    if (!getopr1(&opds, opt, RB_DAT) ||	// Get following operands
        		    !getopr(&opdd, opt, RB_DAT))
				return;
            opcdout(opt->ot_val2, &opdd, opt, clp);
            oprdout(regindx[(int)(opds.op_val.val)] << 3, &opdd, clp);
			storevalue(&clp->val2, VALUE_1_U, &val, bufr);
			clp->size++;
        }
	}
	else								// If not GCC mode
	{
	    if (getopr1(&opdd, opt, RB_DAT) &&	// Get first and second operands
    	        getopr1(&opds, opt, RB_DAT))
    	{
        	stopper = nxtnbc();
        	if (stopper != litchr)		// Immediate value?
        	{
				hldchar = stopper;
        	    getatom(&val);			// No - must be CL (can't call
										//   getopr here because size may
										//   not match!
        	    if (val.kind != VLK_REG || val.val != RG_CL)
            	{
                	adrerr();
					return;
				}
				opcdout(opt->ot_val1, &opdd, opt, clp); // Output opcode byte
				oprdout(regindx[(int)(opds.op_val.val)] << 3, &opdd, clp);
			}
			else
			{
				exprsn(&val);			// Get immediate value
				opcdout(opt->ot_val2, &opdd, opt, clp);
				oprdout(regindx[(int)(opds.op_val.val)] << 3, &opdd, clp);
				storevalue(&clp->val2, VALUE_1_U, &val, &xstrbufr);
				clp->size++;
			}
        }
    }
}


//**************************************************
// Function: iret - Handle RET and RETF instructions
// Returned: Nothing
//**************************************************

void iret(
	OPTBL *opt,
	CL    *clp)

{
	VL val;

    if (((opt->ot_size)? (opt->ot_size == 4): stack32) ^
            ((segatrb & SA_32BIT) != 0)) // Need operand size prefix?
	{
        clp->flags |= CLF_DS;			// Yes - store it
		clp->size++;
	}
    if (ifend(stopper))					// Have operand?
	{
        clp->byte1 = opt->ot_val1;		// No - store value for opcode
		clp->size++;
	}
    else
    {
		if (stopper == litchr)			// Is it given as a literal?
			nxtnbc();					// Yes - ignore that
        exprsn(&val);					// Get fixup amount
        clp->byte1 = opt->ot_val2;		// Store opcode
        storevalue(&clp->val1, VALUE_2_U, &val, xstrbufr);
		clp->size += 3;					// Store fixup amount (always 16 bits)
	}
}

// Function to handle one byte instructions with no operands

void ionebyte(
	OPTBL *opt,
	CL    *clp)

{
    clp->byte1 = opt->ot_val1;			// Store value for opcode
	clp->size++;
}

// Function to handle one byte instructions with no operands which may require
//   an operand size prefix

void ionebytx(
	OPTBL *opt,
	CL    *clp)

{
	clp->size = 1;
    if (((opt->ot_size)? (opt->ot_size == 4): stack32) ^
            ((segatrb & SA_32BIT) != 0)) // Need operand size prefix?
	{
        clp->flags |= CLF_DS;			// Yes
		clp->size++;
	}
    clp->byte1 = opt->ot_val1;			// Store value for opcode
}


//*******************************************************************
// Function: itwobyte - Handle two byte instructions with no operands
// Returned: Nothing
//*******************************************************************

// Use of the opcode table:
//	ot_val1 = First byte value
//	ot_val2 = Second byte value
//	ot_val3 = Not used
//	ot_val4 = Alternate second byte (used if GCC mode)

void itwobyte(
	OPTBL *opt,
	CL    *clp)
{
    clp->byte1 = opt->ot_val1;			// Store first opcode byte
    clp->byte2 = (gccflg) ? opt->ot_val4 : opt->ot_val2; // Store second opcode
	clp->size = 2;										 //   byte
}


//*********************************************************************
// Function: ithrbyte - Handle three byte instructions with no operands
// REturned: Nothing
//*********************************************************************

void ithrbyte(
	OPTBL *opt,
	CL    *clp)
{
    clp->byte1 = opt->ot_val1;			// Store first byte of opcode
    clp->byte2 = opt->ot_val2;			// Store second byte of opcode
	clp->byte3 = opt->ot_val3;			// Store third byte of opcode
	clp->size = 3;
}


//*********************************************
// Function: opcdout - Store single opcode byte
// Returned: Nothing
//*********************************************

void opcdout(
    long   value,
    OP    *opd,
    OPTBL *opt,
	CL    *clp)
{
    if (opd->op_seg)					// Need segment override prefix?
	{
        clp->seg = opd->op_seg;			// Yes
		clp->size++;
	}
    if (opd->op_apfx)					// Need address size prefix?
	{
		clp->flags |= CLF_AS;			// Yes
		clp->size++;
	}
    if (opt->ot_size > 1 && !(opt->ot_flag & OF_NS))
										// Might this instruction need an
										//   operand size prefix?
    {									// Yes - does it really need one?

//      if ((opt->ot_size == 2) ^ !(segatrb & SA_32BIT))
//	Above genrates bad code under TC 2.0!

        if ((opt->ot_size == 2) ^ ((segatrb & SA_32BIT) == 0))
		{
			clp->flags |= CLF_DS;		// Yes
			clp->size++;
		}
    }
    else if ((opt->ot_flag & OF_SS) &&
			(stack32 ^ ((segatrb & SA_32BIT) != 0)))
	{
		clp->flags |= CLF_DS;
		clp->size++;
	}
    if (opt->ot_flag & OF_XP)			// Need 30386 prefix?
	{
		clp->byte0 = 0x0F;				// Yes
		clp->size++;
	}
	clp->byte1 = value;					// Store opcode byte
	clp->size++;
}


//***********************************
// Function: oprdout - Output operand
// Returned: Nothing
//***********************************

void oprdout(
    long extra,
    OP  *opd,
	CL  *clp)

{
    clp->byte2 = opd->op_modrm | extra;	// Store first operand byte
	clp->size++;
    if ((opd->op_sib & 0xFF00) != 0x0100) // Need sib byte?
	{
        clp->byte3 = opd->op_sib;		// Yes
		clp->size++;
	}
	if (opd->op_nab > 0)
	{
		storevalue(&clp->val1, (opd->op_nab == 1) ? VALUE_1_S :
				(opd->op_nab == 2) ? VALUE_2_U : VALUE_4_U, &opd->op_val,
				opd->exprsn);
		clp->size += opd->op_nab;
	}

///	printf("### S:%d CT:%d T:%d K:%d F:%04.4X V:%08.8X P:%04.4X %08.8X\n", clp->size,
///			clp->type, clp->val1.value.type, clp->val1.value.kind,
///			clp->val1.value.flags, clp->val1.value.val,
///			clp->val1.value.psect, clp);
}


//*******************************************************
// Function: valuesout - Store data value for instruction
//				 which uses the S bit
// Returned: Nothing
//*******************************************************

void valuesout(
    OP    *opd,
    OPTBL *opt,
	CL    *clp)

{
	int size;
	int type;

	if (opt->ot_size == 1)
	{
		type = VALUE_1_U;
		size = 1;
	}
	else if (opd->op_val.size == 1)
	{
		type = VALUE_1_S;
		size = 1;
	}
	else if (opt->ot_size == 2)
	{
		type = VALUE_2_U;
		size = 2;
	}
	else
	{
		type = VALUE_4_U;
		size = 4;
	}
	storevalue(&clp->val2, type, &opd->op_val, opd->exprsn);
	clp->size += size;
}


//*******************************************************
// Function: valueout - Output data value for instruction
//				which does not use the S bit
// Returned: Nothing
//*******************************************************

void valueout(
    OP    *opd,
    OPTBL *opt,
	CL    *clp)

{
	storevalue(&clp->val2, (opt->ot_size == 1) ? VALUE_1_U :
			(opt->ot_size == 2) ? VALUE_2_U : VALUE_4_U, &opd->op_val,
			opd->exprsn);
	clp->size += opt->ot_size;
}



void storevalue(
	CLVL *val,
	int   type,
	VL   *vl,
	char *exprsn)

{
	int len;

	val->type = type;
	val->value = *vl;
	if (vl->flags & (VLF_VOLATILE | VLF_UNDEF))
	{
		len = strlen(exprsn);
		val->exprsn = (char *)getspace(len + 1);
		memcpy(val->exprsn, exprsn, len + 1);
	}

///	printf("### exp: %X", val->exprsn);
///	if (val->exprsn != NULL)
///		printf(" |%s|\n", val->exprsn);
///	else
///		printf("\n");
}


//********************************************
// Function: addrerr - Report addressing error
// Returned: Nothng
//********************************************

void adrerr(void)

{
    skiprest();
    if (!(errflag & ~(ERR_U|ERR_V)))
		errflag |= ERR_A;
}


//*************************************
// Function: sizerr - Report size error
// Returned: Nothing
//*************************************

void sizerr(void)

{
    skiprest();
    errflag |= ERR_S;
}
