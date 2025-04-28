//*****************************************
// OPINF.C Op-code input functions for XMAC
//*****************************************
// Written by John Goltz
//*****************************************

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
#include <xos.h>
#include "xmac.h"

extern struct op opd2;

// Table which gives value for mod-r/m and sib bytes as function of register

uchar regsngx[] =
{   0,			// Register AL (8 bit)
    0,			// Register AH (8 bit)
    0,			// Register BL (8 bit)
    0,			// Register BH (8 bit)
    0,			// Register CL (8 bit)
    0,			// Register CH (8 bit)
    0,			// Register DL (8 bit)
    0,			// Register DH (8 bit)
    0,			// Register AX (16 bit)
    0x07,		// Register BX (16 bit)
    0,			// Register CX (16 bit)
    0,			// Register DX (16 bit)
    0x04,		// Register SI (16 bit)
    0x05,		// Register DI (16 bit)
    0x06		// Register BP (16 bit)
};

// Table which gives value for register value for instruction bytes as
//   function of register value

uchar regindx[] =
{   0x00,		// Register AL (8 bit)
    0x04,		// Register AH (8 bit)
    0x03,		// Register BL (8 bit)
    0x07,		// Register BH (8 bit)
    0x01,		// Register CL (8 bit)
    0x05,		// Register CH (8 bit)
    0x02,		// Register DL (8 bit)
    0x06,		// Register DH (8 bit)
    0x00,		// Register AX (16 bit)
    0x03,		// Register BX (16 bit)
    0x01,		// Register CX (16 bit)
    0x02,		// Register DX (16 bit)
    0x06,		// Register SI (16 bit)
    0x07,		// Register DI (16 bit)
    0x05,		// Register BP (16 bit)
    0x04,		// Register SP (16 bit)
    0x00,		// Register EAX (32 bit)
    0x03,		// Register EBX (32 bit)
    0x01,		// Register ECX (32 bit)
    0x02,		// Register EDX (32 bit)
    0x06,		// Register ESI (32 bit)
    0x07,		// Register EDI (32 bit)
    0x05,		// Register EBP (32 bit)
    0x04,		// Register ESP (32 bit)
    0x08,		// Register CS (16 bit)
    0x18,		// Register DS (16 bit)
    0x10,		// Register SS (16 bit)
    0x00,		// Register ES (16 bit)
    0x20,		// Register FS (16 bit)
    0x28,		// Register GS (16 bit)
    0,			// Register PC (16 or 32 bit)
    0,			// Register FR (16 or 32 bit, flag register)
    0,			// Register LDTR (16 bit, system level)
    0,			// Register MSW (16 or 32 bit, system level)
    0,			// Register TR (16 bit, system level)
    0,			// Register GDTR (48 bit, system level)
    0,			// Register IDTR (48 bit, system level)
    0x00,		// Register CR0 (32 bit, system level)
    0x10,		// Register CR2 (32 bit, system level)
    0x18,		// Register CR3 (32 bit, system level)
    0x20,		// Register CR4 (32 bit, system level)
    0x00,		// Register DR0 (32 bit, system level)
    0x08,		// Register DR1 (32 bit, system level)
    0x10,		// Register DR2 (32 bit, system level)
    0x18,		// Register DR3 (32 bit, system level)
    0x30,		// Register DR6 (32 bit, system level)
    0x38,		// Register DR7 (32 bit, system level)
    0x30,		// Register TR6 (32 bit, system level)
    0x38,		// Register TR7 (32 bit, system level)
    0x00,		// Register ST0 (FPU TOS)
    0x01,		// Register ST1 (FPU TOS-1)
    0x02,		// Register ST2 (FPU TOS-2)
    0x03,		// Register ST3 (FPU TOS-3)
    0x04,		// Register ST4 (FPU TOS-4)
    0x05,		// Register ST5 (FPU TOS-5)
    0x06,		// Register ST6 (FPU TOS-6)
    0x07		// Register ST7 (FPU TOS-7)
};

// Table of segment prefix bytes

uchar regseg[] =
{   0x2E,		// Register CS (16 bit)
    0x3E,		// Register DS (16 bit)
    0x36,		// Register SS (16 bit)
    0x26,		// Register ES (16 bit)
    0x64,		// Register FS (16 bit)
    0x65		// Register GS (16 bit)
};

// Table of valid register bits

uchar regbits[] =
{   
    RB_DAT,					// Register AL (8 bit)
    RB_DAT,					// Register AH (8 bit)
    RB_DAT,					// Register BL (8 bit)
    RB_DAT,					// Register BH (8 bit)
    RB_DAT,					// Register CL (8 bit)
    RB_DAT,					// Register CH (8 bit)
    RB_DAT,					// Register DL (8 bit)
    RB_DAT,					// Register DH (8 bit)
    RB_DAT,					// Register AX (16 bit)
    RB_DAT|RB_F16I,			// Register BX (16 bit)
    RB_DAT,					// Register CX (16 bit)
    RB_DAT,					// Register DX (16 bit)
    RB_DAT|RB_F16I,			// Register SI (16 bit)
    RB_DAT|RB_F16I,			// Register DI (16 bit)
    RB_DAT|RB_F16I,			// Register BP (16 bit)
    RB_DAT,					// Register SP (16 bit)
    RB_DAT|RB_F32I|RB_S32I,	// Register EAX (32 bit)
    RB_DAT|RB_F32I|RB_S32I,	// Register EBX (32 bit)
    RB_DAT|RB_F32I|RB_S32I,	// Register ECX (32 bit)
    RB_DAT|RB_F32I|RB_S32I,	// Register EDX (32 bit)
    RB_DAT|RB_F32I|RB_S32I,	// Register ESI (32 bit)
    RB_DAT|RB_F32I|RB_S32I,	// Register EDI (32 bit)
    RB_DAT|RB_F32I|RB_S32I,	// Register EBP (32 bit)
    RB_DAT|RB_F32I,			// Register ESP (32 bit)
    RB_SEG,					// Register CS (16 bit)
    RB_SEG,					// Register DS (16 bit)
    RB_SEG,					// Register SS (16 bit)
    RB_SEG,					// Register ES (16 bit)
    RB_SEG,					// Register FS (16 bit)
    RB_SEG,					// Register GS (16 bit)
    0,						// Register PC (16 or 32 bit)
    0,						// Register FR (16 or 32 bit, flag register)
    0,						// Register LDTR (16 bit, system level)
    0,						// Register MSW (16 or 32 bit, system level)
    0,						// Register TR (16 bit, system level)
    0,						// Register GDTR (48 bit, system level)
    0,						// Register IDTR (48 bit, system level)
    RB_CNTL,				// Register CR0 (32 bit, system level)
    RB_CNTL,				// Register CR2 (32 bit, system level)
    RB_CNTL,				// Register CR3 (32 bit, system level)
    RB_CNTL,				// Register DR0 (32 bit, system level)
    RB_CNTL,				// Register DR1 (32 bit, system level)
    RB_CNTL,				// Register DR2 (32 bit, system level)
    RB_CNTL,				// Register DR3 (32 bit, system level)
    RB_CNTL,				// Register DR6 (32 bit, system level)
    RB_CNTL,				// Register DR7 (32 bit, system level)
    RB_CNTL,				// Register TR6 (32 bit, system level)
    RB_CNTL,				// Register TR7 (32 bit, system level)
    RB_FLOAT,				// Register ST0 (FPU TOS)
    RB_FLOAT,				// Register ST1 (FPU TOS-1)
    RB_FLOAT,				// Register ST2 (FPU TOS-2)
    RB_FLOAT,				// Register ST3 (FPU TOS-3)
    RB_FLOAT,				// Register ST4 (FPU TOS-4)
    RB_FLOAT,				// Register ST5 (FPU TOS-5)
    RB_FLOAT,				// Register ST6 (FPU TOS-6)
    RB_FLOAT				// Register ST7 (FPU TOS-7)
};


static int  getoprm(OP *opd, OPTBL *opt);
static int  indxexp(OP *opd, int size);
static int  regexp(VL *val);


//******************************************************
// Function: getopr1 - Get first operand for instruction
//				(must be stopped by comma)
// Returned: TRUE if valid operand, FALSE if not valid
//******************************************************

int getopr1(
	OP    *opd,					// Operand data
	OPTBL *opt,
	int    rmask)

{
    if (getopr(opd, opt, rmask))		// Get operand
    {   
		if (stopper == ',')				// Stopped by comma?
			return (TRUE);				// Yes
		else
        {
			queerr();					// No
            return (FALSE);
        }
    }
    else
		return (FALSE);
}


//****************************************************
// Function: getopr - Get operand for instruction
// Returned: TRUE if valid operand, FALSE if not valid
//****************************************************

int getopr(
	OP    *opd,					// Operand data
	OPTBL *opt,
	int    rmask)

{
    char chr;

	opd->op_val.val = 0;
    opd->op_apfx = 0;					// Assume no address size prefix
										//   needed
    opd->op_seg = 0;					// Assume no segment override
										//   prefix needed
    opd->op_sib = 0x0100;				// Assume no sib byte needed
	opd->exprsn[0] = 0;
    chr = nxtnbc();						// Get first character
    if (eolflg || chr == ';')			// Null operand?
    {
        queerr();						// Yes - fail
        return (FALSE);
    }
    if (chr == inxbgn)					// Index without offset?
        return (indxexp(opd, -1));		// Get index expression
    if (chr == litchr)					// Immediate value?
    {
        exprsn(&opd->op_val);			// Evaluate expression
		strcpy(opd->exprsn, xstrbufr);
        if (opd->op_val.kind != VLK_REG && // Simple value?
                (opd->op_val.type == VLT_VAL ||
				opd->op_val.type == VLT_BYTE))
        {
            opd->op_type = OP_IMM;		// Yes
            opd->op_size = 0;
            return (TRUE);
        }
        else							// Not simple value - give error
        {
            adrerr();
            return (FALSE);
        }
    }
    else
    {
        hldchar = chr;

        exprsn(&opd->op_val);			// Get general expression
		strcpy(opd->exprsn, xstrbufr);
        if (opd->op_val.kind == VLK_REG) // Is it a register?
        {
            if (ptype < P_80386 && (opd->op_val.val == RG_FS ||
                    opd->op_val.val == RG_GS))
            {
                adrerr();
                return (FALSE);
            }
            if (stopper == ':')			// Yes - is it a segment prefix?
            {
                if (!(regbits[(int)(opd->op_val.val)] & RB_SEG))
                {						// Yes - is it a valid one?
                    adrerr();			// Not valid
                    return (FALSE);
                }
                opd->op_seg = regseg[(int)(opd->op_val.val) - RG_CS];
										// Store segment prefix byte to use
                chr = nxtnbc();			// Yes - get following character
                if (chr == inxbgn)		// Index without offset?
                    return (indxexp(opd, -1)); // Yes - get index expression
                hldchar = chr;
                exprsn(&opd->op_val);	// Get following expression
				strcpy(opd->exprsn, xstrbufr);
                if (opd->op_val.kind == VLK_REG) // Cannot be register
                {
                    adrerr();
                    return (FALSE);
                }
                return (getoprm(opd, opt)); // Get memory operand
            }
            if (!(regbits[(int)(opd->op_val.val)] & rmask))
            {							// Valid register?
                adrerr();				// No
                return (FALSE);
            }
            if ((opd == &opds) && (opt->ot_flag & (OF_BS|OF_WS)))
            {							// Is this source operand for
										//   instruction with special size
										//   requirements?
                if (((opt->ot_flag & OF_BS) ? 1 : 2) != opd->op_val.size)
                {						// Yes - must be byte or word
                    sizerr();
                    return (FALSE);
                }
            }
            else if (regbits[(int)(opd->op_val.val)] & RB_SEG)
            {							// Segment register?
                if (opd->op_val.size == 1) // Yes - word or long is OK
                {
                    sizerr();			// But byte is bad!
                    return (FALSE);
                }
            }
            else if ((opt->ot_size && opt->ot_size != opd->op_val.size) ||
                    ((opt->ot_flag & OF_SS) &&
                    ((opd->op_val.size == 1) ||
						(stack32 ^ (opd->op_val.size == 4)))))
            {							// Is it the right size?
                sizerr();				// No - error
                return (FALSE);
            }
            opd->op_modrm = regindx[(int)(opd->op_val.val)] | 0xC0;
            opd->op_type = OP_REG;
            opd->op_nab = 0;			// No address bytes
            opd->op_asize = 0;
            return (TRUE);
        }
		return (getoprm(opd, opt));
    }
}


//***********************************************
// Function: getoprm - Get memory address operand
// Returned: TRUE if OK, FALSE if error
//***********************************************

int getoprm(
    OP    *opd,					// Operand data
    OPTBL *opt)

{
    opd->op_size = 0;					// Memory operands have indefinate
										//   data size
    if (stopper != inxbgn)				// Indexed?
    {
        opd->op_type = OP_MEM;			// No - have simple memory operand
        if (opd->op_val.type == VLT_LONG)
        {								// 32 bit address specified?
            if (ptype < P_80386)		// Yes - for 386 or 486?
            {
                adrerr();				// No - error
                return (FALSE);
            }
            opd->op_modrm = 0x05;		// Yes - OK
            opd->op_nab = 4;
            opd->op_asize = 4;
            if (!(segatrb & SA_32BIT))	// 32 bit segment?
                opd->op_apfx = 0x67;	// No - need address size prefix
            return (TRUE);
        }
        if (opd->op_val.type == VLT_WORD || opd->op_val.type == VLT_BYTE)
        {								// 16 or 8 bit address specified?
            if (opd->op_val.type == VLT_WORD ||
                    (opt->ot_flag & OF_BO)) // 8 bit?
            {
                opd->op_modrm = 0x06;	// Yes
                opd->op_nab = 2;
                opd->op_asize = 2;
                if (segatrb & SA_32BIT)	// 32 bit segment?
                    opd->op_apfx = 0x67; // Yes - need address size prefix
                return (TRUE);
            }
        }
        if (opd->op_val.type == VLT_VAL) // No size specified?
        {
            if (segatrb & SA_32BIT)		// Yes - 32 bit segment?
            {
                opd->op_modrm = 0x05;	// Yes - make it 32 bits
                opd->op_nab = 4;
                opd->op_asize = 4;
            }
            else						// 16 bit segment - make it 16 bits
            {
                opd->op_modrm = 0x06;
                opd->op_nab = 2;
                opd->op_asize = 2;
            }
            return (TRUE);
        }
        adrerr();
        return (FALSE);
    }

	// Here if indexed
	
	if (opd->op_val.type == VLT_VAL && opd->op_val.kind == VLK_ABS &&
			(opd->op_val.flags & VLF_UNDEF) == 0 &&
			opd->op_val.val < 0x80 && opd->op_val.val >= -0x80)
										// Size not specified and absolute and
										//   not undefined and value will fit
	{									//   in 8 bits?
		opd->op_val.type = VLT_BYTE; 	// Yes - make it a byte value
		opd->op_val.size = 1;
	}
	return (indxexp(opd, (opd->op_val.type == VLT_VAL) ? 0 : opd->op_val.size));
}


//**********************************************************
// Function: indxexp = Evaluate index expression for operand
// Returned: TRUE if OK, FALSE if error
//**********************************************************

static int indxexp(
    OP *opd,					// Operand data
    int size)					// Size of offset - -1 means no offset, 0

{
	VL    val;
    int   freg;
    int   sreg;
	char  chr;
	uchar single;

    opd->op_type = OP_CM1;				// Assume will need only the mod-r/m
										//   byte

	if (opd->op_val.kind == VLK_ABS && opd->op_val.val == 0 &&
			(opd->op_val.flags & VLF_UNDEF) == 0)
		size = -1;
	single = FALSE;
	if (gccflg)
	{
		if ((chr = nxtnbc()) == ',')	// Have null first register?
			single = TRUE;
		else
			hldchar = chr;
	}
	if (!regexp(&val))					// Get first register
		return (FALSE);
	if (val.size == 2)					// 16 bit register?
	{
		if (!(regbits[(int)(val.val)] & RB_F16I) || size == 4)
		{
			adrerr();
			return (FALSE);
		}
		opd->op_nab = (size == -1) ? 0 : ((size == 1) ? 1 : 2);
		if (segatrb & SA_32BIT)			// 32 bit segment?
			opd->op_apfx = 0x67;		// Yes - need address size prefix
		if (stopper == inxend)			// Simple single index?
		{
			stopper = nxtnbc();			// Yes - get real stopper
			if (size == -1 && val.val == RG_BP)
			{							// Trying to use BP without offset?
				size = 1;				// Yes - fix it up
				opd->op_val.val = 0;
				opd->op_val.type = VLT_BYTE;
				opd->op_val.kind = VLK_ABS;
			}
            opd->op_modrm = regsngx[(int)(val.val)];
            opd->op_asize = 2;
            if (size != -1)
                opd->op_modrm |= (size == 1)? 0x40: 0x80;
            return (TRUE);
        }
        if (stopper == inxsep1)			// Double index?
        {
            freg = val.val;				// Yes
            if (freg != RG_BX && freg != RG_BP && freg != RG_DI &&
                    freg != RG_SI)		// Valid first register?
            {
                adrerr();				// No - fail
                return (FALSE);
            }
            if (!regexp(&val))			// Get second register
                return (FALSE);
            if (stopper != inxend)
            {
                adrerr();
                return (FALSE);
            }
            if (freg == RG_BX || freg == RG_BP) // Was BX or BP first?
            {							// Yes - DI or SI must be second
                sreg = val.val;			// sreg is DI or SI
                if (sreg != RG_SI && sreg != RG_DI)
                {
                    adrerr();
                    return (FALSE);
                }
            }
            else						// DI or SI was first, so BX or PB
            {							//   must be second
                sreg = freg;			// Make sreg DI or SI
                freg = val.val;			// freg is BX or BP
                if (freg != RG_BX && freg != RG_BP)
                {
                    adrerr();
                    return (FALSE);
                }
            }
            stopper = nxtnbc();			// Get actual stopper
            opd->op_modrm = (freg == RG_BX)?
                    ((sreg==RG_SI)? 0x00: 0x01): ((sreg==RG_SI)? 0x02: 0x03);
            opd->op_asize = 2;
            if (size != -1)
                opd->op_modrm |= (size == 1) ? 0x40 : 0x80;
            return (TRUE);
        }
        adrerr();						// Stopper not + or ] - error
        return (FALSE);
    }
    else								// 32 bit register
    {
		if (!(regbits[val.val] & RB_F32I) || size == 2 ||
				ptype < P_80386)		// 16 bit offset?
		{
			adrerr();					// Can't do that!
			return (FALSE);
		}
		opd->op_nab = (size == -1) ? 0 : ((size == 1) ? 1 : 4);
		opd->op_asize = 4;
		if (!(segatrb & SA_32BIT))		// 16 bit segment?
			opd->op_apfx = 0x67;		// Yes - need address size prefix
        if (size == -1 && val.val == RG_EBP)
		{
			size = 1;
			opd->op_nab = 1;
			opd->op_val.val = 0;
			opd->op_val.type = VLT_BYTE;
			opd->op_val.kind = VLK_ABS;
		}
		if (stopper == inxend)			// Simple single index?
		{
			stopper = nxtnbc();
			if (val.val == RG_ESP)		// Trying to use ESP
			{
				opd->op_modrm = 0x04;	// Yes - this requires an sib too
				opd->op_sib = 0x24;
			}
			else
				opd->op_modrm = regindx[(int)(val.val)];
			if (size != -1)
				opd->op_modrm |= ((size == 1) ? 0x40 : 0x80);
			return (TRUE);
		}
		if ((!gccflg || single) && stopper == inxsep2) // Scaled index only?
		{
			// This a somewhat strange special case. Only a 4-byte offset
			//   is allowed. That is encoded as if there was a base register
			//   of EBP with no offset! (base = 5, mod = 0)

			freg = val.val;
			if (!getscale(opd))
				return (FALSE);
			stopper = nxtnbc();
			opd->op_sib |= (regindx[freg] << 3);
			opd->op_sib |= 0x05;
			if (size != 4)
			{
				if (size == -1)
				{
					opd->op_val.val = 0;
					opd->op_val.type = VLT_BYTE;
					opd->op_val.kind = VLK_ABS;
				}
				opd->op_nab = 4;
			}
			opd->op_modrm = 0x04;
			return (TRUE);
		}
		if (stopper == inxsep1)			// Double index?
		{
			freg = val.val;				// Yes
			opd->op_type = OP_CM2;		// This needs the sib byte too
			if (!regexp(&val))			// Get second register
				return (FALSE);
			if ((stopper != inxend && stopper != inxsep2)
					|| !(regbits[(int)(val.val)] & RB_S32I))
			{
				adrerr();
				return (FALSE);
			}
            sreg = val.val;
            if (stopper == inxsep2)		// Scale factor next?
            {
				if (!getscale(opd))
					return (FALSE);
			}
			else						// No scale factor - assume 1
				opd->op_sib = 0x00;
            stopper = nxtnbc();
            opd->op_sib |= (regindx[sreg] << 3);
            opd->op_sib |= regindx[freg];
            opd->op_modrm = (size == -1) ? 0x04 : ((size == 1) ? 0x44 : 0x84);
            return (TRUE);
		}
		adrerr();
		return (FALSE);
	}
}


//*************************************
// Function: regexp - Get register
// Returned: TRUE if OK, FALSE if error
//*************************************

static int regexp(
	VL *val)

{
    exprsn(val);						// Evaluate expression
    if (val->kind == VLK_REG)			// Is it a register?
        return (TRUE);					// Yes
    else
    {
        adrerr();						// No
        return (FALSE);
    }
}


//**************************************
// Function: getscale - Get scale factor
// Returned: TRUE if OK, FALSE if error
//**************************************

int getscale(
    struct op *opd)				// Operand data

{
	VL val;

    getabs(&val);						// Get absolute value next
    if (val.val == 1)
        opd->op_sib = 0x00;
    else if (val.val == 2)
        opd->op_sib = 0x40;
    else if (val.val == 4)
        opd->op_sib = 0x80;
    else if (val.val == 8)
        opd->op_sib = 0xC0;
    else
    {
         adrerr();
         return (FALSE);
    }
    if (stopper != inxend)
    {
        adrerr();
        return (FALSE);
    }
    return (TRUE);
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
