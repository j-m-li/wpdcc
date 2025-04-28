//*******************************
// Parameter definitions for XMAC
//*******************************
// Written by John Goltz
//*******************************

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

#define FALSE 0
#define TRUE  1

#define HASHSIZE  128			// Size of the symbol hash table
#define INCLMAX   6				// Maximum include nesting level
#define INCLFSSZ  100			// Maximum length for include file
								//   specification
#define SYMMAXSZ  128			// Maximum length for symbol name
#define DIGMAXSZ  20			// Maximum number of digits in a number
#define IFSTRSZ   60			// Maximum string length for .IF compares
#define ALBSIZE   4000			// Size of alpha listing buffer
#define BLKSIZE   (sizeof(SY))	// Size of a memory block
#define LSTPGSZ   50			// Page length (excluding header) for listing
#define TITLESZ   200			// Maximum length for title text
#define SBTTLSZ   300			// Maximum length for subtitle text
///#define ETBLSIZE  8				// Size of error message pointer table
#define CONDMAX   20			// Maximum nesting level for conditionals
#define ASCIIMAX  2800			// Maximum length of an ASCII string
#define SPCEXPSZ  100			// Maximum length of special expressions
								//   (.START, .STACK, and .DEBUG)
#define XSTRMAX   300

#ifndef offsetof
#define offsetof(strt, label) (int)&(((strt *)0)->label)
#endif

// Define listing types

#define LT_NONE   0
#define LT_NORMAL 1
#define LT_CREF   2
#define LT_DEBUG  3

// Define value types for binary output

#define VALUE_1_S 1			// 1 byte unsigned value
#define VALUE_1_U 2			// 1 byte signed value
#define VALUE_2_S 3			// 2 byte unsigned value
#define VALUE_2_U 4			// 2 byte signed value
#define VALUE_4_S 5			// 4 byte unsigned value
#define VALUE_4_U 6			// 4 byte signed value
#define VALUE_8   7			// 8 byte value
#define VALUE_2_A 8			// 2 byte far address
#define VALUE_4_A 9			// 4 byte far address
#define VALUE_8_A 10		// 8 byte far address

// Define error flag bits

#define ERR_Q  0x80000000	// Questionable line
#define ERR_V  0x40000000	// Value error
#define ERR_U  0x20000000	// Undefined symbol (ignored if seen during
							//   initial assembly pass)
#define ERR_U2 0x10000000	// Undefined symbol (never ignored)
#define ERR_A  0x08000000	// Addressing error
#define ERR_T  0x04000000	// Value truncated
#define ERR_R  0x02000000	// Address out of range
#define ERR_M  0x01000000	// Multipley defined symbol
#define ERR_O  0x00800000	// Illegal opcode
#define ERR_C  0x00400000	// Attribute conflict
#define ERR_X  0x00300000	// Illegal expression
#define ERR_D  0x00100000	// Reference to multipley defined symbol
#define ERR_K  0x00080000	// Illegal angle bracket usage
#define ERR_S  0x00040000	// Operation size error
#define ERR_E  0x00020000	// .ERROR pseudo-op
#define ERR_I  0x00010000	// Invalid segment or msect name
#define ERR_DN 0x00008000	// Different segment or msect name
#define ERR_CE 0x00004000	// Conditional error
#define ERR_CX 0x00002000	// Conditional nesting too deep
#define ERR_CO 0x00001000	// Open conditional at end of assembly
#define ERR_UM 0x00000800	// Open macro or repeat definition at end of
							//   assembly
#define ERR_NA 0x00000400	// Value is not an address
#define ERR_NP 0x00000200	// No psect for code or data
#define ERR_IN 0x00000100	// Illegal .INCLUD or .REQUIR file name
#define ERR_XX 0x00000080	// No error (used to force listing for .PRINT

// Define processor type values (stored in ptype)

#define P_8086  2
#define P_80186 3
#define P_80286 4
#define P_80386 5
#define P_80486 6
#define P_80586 7

// Define bits for pbits and ot_proc bytes

#define B_80586 0x20
#define B_80486 0x10
#define B_80386 0x08
#define B_80286 0x04
#define B_80186 0x02
#define B_8086  0x01

// Define REL file section types

#define SC_EOF    0			// End of file
#define SC_INDEX  1			// Library index section
#define SC_START  2			// Start of module section
#define SC_SEG    3			// Segment definition section
#define SC_MSECT  4			// Msect definition section
#define SC_PSECT  5			// Psect definition section
#define SC_DATA   6			// Data section
#define SC_INTERN 7			// Internal symbol definition section
#define SC_LOCAL  8			// Local symbol definition section
#define SC_EXPORT 9			// Exported symbol definition section
#define SC_STRADR 10		// Starting address section
#define SC_DEBUG  11		// Debugger address section
#define SC_STACK  12		// Initial stack address section
#define SC_FILREQ 13		// File request section
#define SC_EXTERN 14		// External symbol definition section
#define SC_SHARED 15		// Shared library definition section
#define SC_IMPORT 16		// Imported symbol definition section

// Define polish operators

#define PO_ADD        0x80	// Add
#define PO_SUB        0x81	// Subtract
#define PO_MUL        0x82	// Multiply
#define PO_DIV        0x83	// Divide
#define PO_SHF        0x84	// Shift
#define PO_AND        0x85	// And
#define PO_IOR        0x86	// Inclusive or
#define PO_XOR        0x87	// Exclusive or
#define PO_COM        0x88	// 1s complement
#define PO_NEG        0x89	// Negate (2s complement)
#define PO_PSELCP     0x92	// Push selector, current psect
#define PO_PSELAP     0x93	// Push selector, any psect
#define PO_PSELAM     0x94	// Push selector, any msect
#define PO_PSELAS     0x95	// Push selector, any segment
#define PO_POFSAM     0x96	// Push offset, any msect
#define PO_PSELSYM    0x97	// Push selector, external symbol
#define PO_STRAW      0x98	// Store word address
#define PO_STRAL      0x99	// Store long address
#define PO_STRAQ      0x00	// Store quad address (SOON!)
#define PO_STRUB      0x9A	// Store unsigned byte
#define PO_STRUW      0x9B	// Store unsigned word
#define PO_STRUL      0x9C	// Store unsigned long
#define PO_STRSB      0x9D	// Store signed byte
#define PO_STRSW      0x9E	// Store signed word
#define PO_STRSL      0x9F	// Store signed long
#define PO_STRQ       0x00	// Atore quad (SOON!)
#define PO_PVAL       0xA0	// Push absolute value
#define PO_PVALREL    0xA8	// Push absolute value, relative
#define PO_POFSCP     0xB0	// Push relocated value, current psect
#define PO_POFSAP     0xC0	// Push relocated value, any psect
#define PO_POFSRELAP  0xC8	// Push relocated value, relative, any psect
#define PO_POFSSYM    0xF0	// Push symbol offset value
#define PO_POFSRELSYM 0xF8	// Push symbol offset value, relative

// Define operand types

#define OP_REG   0		// Register
#define OP_IMM   1		// Immediate value
#define OP_MEM   2		// Simple memory operand
#define OP_CM1   3		// Memory operand (single addressing byte)
#define OP_CM2   4		// Memory operand (two addressing bytes)

// Define register values

#define RG_AL   0		// Register AL (8 bit)
#define RG_AH   1		// Register AH (8 bit)
#define RG_BL   2		// Register BL (8 bit)
#define RG_BH   3		// Register BH (8 bit)
#define RG_CL   4		// Register CL (8 bit)
#define RG_CH   5		// Register CH (8 bit)
#define RG_DL   6		// Register DL (8 bit)
#define RG_DH   7		// Register DH (8 bit)
#define RG_AX   8		// Register AX (16 bit)
#define RG_BX   9		// Register BX (16 bit)
#define RG_CX   10		// Register CX (16 bit)
#define RG_DX   11		// Register DX (16 bit)
#define RG_SI   12		// Register SI (16 bit)
#define RG_DI   13		// Register DI (16 bit)
#define RG_BP   14		// Register BP (16 bit)
#define RG_SP   15		// Register SP (16 bit)
#define RG_EAX  16		// Register EAX (32 bit)
#define RG_EBX  17		// Register EBX (32 bit)
#define RG_ECX  18		// Register ECX (32 bit)
#define RG_EDX  19		// Register EDX (32 bit)
#define RG_ESI  20		// Register ESI (32 bit)
#define RG_EDI  21		// Register EDI (32 bit)
#define RG_EBP  22		// Register EBP (32 bit)
#define RG_ESP  23		// Register ESP (32 bit)
#define RG_CS   24		// Register CS (16 bit)
#define RG_DS   25		// Register DS (16 bit)
#define RG_SS   26		// Register SS (16 bit)
#define RG_ES   27		// Register ES (16 bit)
#define RG_FS   28		// Register FS (16 bit)
#define RG_GS   29		// Register GS (16 bit)
#define RG_PC   30		// Register PC (16 or 32 bit)
#define RG_FR   31		// Register FR (16 or 32 bit, flag register)
#define RG_LDTR 32		// Register LDTR (16 bit, system level)
#define RG_MSW  33		// Register MSW (16 or 32 bit, system level)
#define RG_TR   34		// Register TR (16 bit, system level)
#define RG_GDTR 35		// Register GDTR (48 bit, system level)
#define RG_IDTR 36		// Register IDTR (48 bit, system level)
#define RG_CR0  37		// Register CR0 (32 bit, system level)
#define RG_CR2  38		// Register CR2 (32 bit, system level)
#define RG_CR3  39		// Register CR3 (32 bit, system level)
#define RG_CR4  40		// Register CR4 (32 bit, system level)
#define RG_DR0  41		// Register DR0 (32 bit, system level)
#define RG_DR1  42		// Register DR1 (32 bit, system level)
#define RG_DR2  43		// Register DR2 (32 bit, system level)
#define RG_DR3  44		// Register DR3 (32 bit, system level)
#define RG_DR6  45		// Register DR6 (32 bit, system level)
#define RG_DR7  46		// Register DR7 (32 bit, system level)
#define RG_TR6  47		// Register TR6 (32 bit, system level)
#define RG_TR7  48		// Register TR7 (32 bit, system level)
#define RG_ST0  49		// Register ST0 (FPU TOS)
#define RG_ST1  50		// Register ST1 (FPU TOS - 1)
#define RG_ST2  51		// Register ST2 (FPU TOS - 2)
#define RG_ST3  52		// Register ST3 (FPU TOS - 3)
#define RG_ST4  53		// Register ST4 (FPU TOS - 4)
#define RG_ST5  54		// Register ST5 (FPU TOS - 5)
#define RG_ST6  55		// Register ST6 (FPU TOS - 6)
#define RG_ST7  56		// Register ST7 (FPU TOS - 7)

// Define bits for the regbits table

#define RB_DAT   0x80		// Register is valid as general data operand
#define RB_F16I  0x40		// Register is valid as first 16 bit index
#define RB_F32I  0x20		// Register is valid as first 32 bit index
#define RB_S32I  0x10		// Register is valid as second 32 bit index
#define RB_SEG   0x08		// Register is a segment register
#define RB_CNTL  0x04		// Register is a processor control register
#define RB_FLOAT 0x02		// Register is an FPU register

typedef struct _cl    CL;
typedef struct pd     PD;
typedef struct pn     PN;
typedef struct md     MD;
typedef struct sd     SD;
typedef struct sn     SN;
typedef struct _sy    SY;
typedef struct ab     AB;
typedef struct sb     SB;
typedef struct fb     FB;
typedef struct _vl    VL;
typedef struct mn     MN;
typedef struct op     OP;
typedef struct optbl  OPTBL;
typedef struct _pstbl PSTBL;
typedef struct ic     IC;
typedef struct body   BODY;

// Define type for general pointer

typedef union
{   long  *l;
    short *s;
    char  *c;
} genpnt;

// Define type for general location

typedef union
{   long  l;
    short s;
    char  c;
} genloc;

// Define structure for local symbol value

struct locsym
{   int   ls_flag;		// Always 0 to indicate local symbol
    long  ls_blk;		// Local symbol block number (lsbnum)
    long  ls_val;		// Local symbol value
};

// Define structure for pseudo-op attributes data

struct atributes
{   char  at_name[6];
    void (*at_func)(void *);
};

// Define structure for symbol table entry (WARNING: Order of items through
//   sy_flag must match corresponding items in the segment, msect, and psect
//   name blocks!)

struct _sy
{   SY    *sy_hash;				// Hash list link
    SY    *sy_sort;				// Pointer for heapsort
    char   sy_name[SYMMAXSZ];	// Symbol name
    union
    {   long         val;		// Value
        struct ab   *ab;
        struct body *body;
    }      sy_val;
    int    sy_psect;			// Psect number
    char   sy_type;				// Symbol table entry type
    char   sy_xxxx;				// Not used
    int    sy_flag;				// Flag bits
    int    sy_symnum;			// Symbol number
    struct rf *sy_ref;			// Pointer to start of reference list
};

// Define structure for general argument lists

struct ab
{   struct   ab *ab_next;	// Pointer to next argument list
    char     ab_data[BLKSIZE-2*sizeof(char *)];
    struct   ab *ab_link;	// Link to next block in this list
};

// Define structure for repeat control block (ICB)

struct ic
{   IC   *ic_link;		// Link to next level ICB
    char *ic_body;		// Pointer to beginning of body
    char *ic_cbp;		// Current block pointer
    char *ic_sbp;		// Saved block pointer
    int   ic_cbc;		// Current block count
    int   ic_sbc;		// Saved block count
    union
    {   AB   *arg;		// Pointer to beginning of argument list
		char *argc;		// Pointer to .IRPC argument string
		long  cnt;		// Repeat counter
    }     ic_;
    char  ic_type;		// Block type
    char  ic_gac;		// Generated argument maximum number
};

// Define values for ic_type

#define ICT_MAC    0	// Macro
#define ICT_REPT   1	// Repeat
#define ICT_IRP    2	// IRP
#define ICT_IRPC   3	// IRPC
#define ICT_STATIC 4	// Static text

// Define values for special inserted characters

#define CC_IMA  0x80	// Insert macro argument
#define CC_IMX  0x81	// Insert macro argument with generated
						//   symbol if null
#define CC_G1S  0x82	// Insert first character of generated
						//   symbol
#define CC_G2S  0x83	// Insert second character of generated
						//   symbol
#define CC_G3S  0x84	// Insert third character of generated symbol
#define CC_G4S  0x85	// Insert fourth character of generated
						//   symbol
#define CC_IIA  0x86	// Insert IRP argument
#define CC_ICA  0x87	// Insert IRPC argument
#define CC_EMB  0x88	// End of macro body
#define CC_ERB  0x89	// End of repeat body
#define CC_EIB  0x8A	// End of IRP body
#define CC_ECB  0x8B	// End of IRPC body
#define CC_EAG  0x8C	// End of macro or IRP argument
#define CC_EST  0x8D	// End of static text
  #define CCSTR_EST "\x8D"

// Define structure for body block

struct body
{   char   bb_data[BLKSIZE-sizeof(char *)]; // Data
    struct body *bb_link;	// Pointer to next body block
};

// Define structure for reference list initial block

struct rf
{   union				// Pointer
    {   int  *ps;
		int **pps;
    }   rf_pnt;
    int rf_cnt;			// Item count
    int rf_data[2];		// First data pair
};

// Define structure for the .REQUIR list blocks

struct reqblk
{   struct reqblk *next;
    int    size;
    char   name[4];
};

// Define structure for op-code table entry

struct optbl
{   char   ot_name[6];			// Name of op-code (less first character)
    void (*ot_dsp)(OPTBL *tbl, CL *clp); // Pointer to routine for type
    uchar  ot_val1;
    uchar  ot_val2;
    uchar  ot_val3;
    uchar  ot_val4;
    uchar  ot_size;
    uchar  ot_proc;
    ushort ot_xxx;
    long   ot_flag;
};

// Define structure for pseudo-op table entry

struct _pstbl
{   char  pt_name[8];
    void (*pt_dsp)(void);
};

struct cptbl
{   char  pt_name[6];
    int (*pt_dsp)(void);
};

// Define structure for if table entry

struct iftbl
{   char  if_name[3];
    void (*if_dsp)(void);
};

// Define bits for ot_flag value

#define OF_BO 0x4000	// 8-bit relative operand value allowed
#define OF_RD 0x2000	// Two operand instruction can have register as
						//   destination
#define OF_RF 0x1000	// First operand is register operand
#define OF_BS 0x0800	// Source operand is a byte
#define OF_WS 0x0400	// Source operand is a word
#define OF_SS 0x0200	// Need operand prefix for stack item size selection
#define OF_MS 0x0100	// Two operand instruction can have memory field as
						//   source
#define OF_B3 0x0080	// Branch instruction has 80386 only 16 bit form
#define OF_RS 0x0040	// Source memory field can be register
#define OF_MD 0x0020	// Two operand instruction can have memory field as
						//   destination
#define OF_XP 0x0010	// 30386 prefix byte needed
#define OF_NS 0x0008	// Do not use operand size prefix byte
#define OF_BI 0x0004	// Immediate value is always single byte
#define OF_DB 0x0002	// Instruction uses direction bit
#define OF_SB 0x0001	// Instruction uses size bit

// Define values for sy_type

#define SYT_SYM  1		// Symbol or macro name
#define SYT_PSC  2		// Psect
#define SYT_MSC  3		// Msect
#define SYT_SEG  4		// Segment

// Define flag bits for sy_flag

#define SYF_EXPORT 0x1000	// Exported symbol
#define SYF_IMPORT 0x0800	// Imported symbol
#define SYF_LABEL  0x0400	// Label
#define SYF_EXTERN 0x0200	// External symbol
#define SYF_UNDEF  0x0100	// Undefined

// Following 4 bits must match the symbol flag bits in the OBJ file
//   symbol table entry

#define SYF_ABS    0x0080	// Absolute symbol
#define SYF_ENTRY  0x0040	// Entry symbol
#define SYF_ADDR   0x0020	// Address
#define SYF_SUP    0x0010	// Suppressed

#define SYF_INTERN 0x0008	// Internal symbol
#define SYF_MULT   0x0004	// Multipily defined
#define SYF_USED   0x0002	// Used symbol
#define SYF_REG    0x0001	// Register

// Define union for symbol buffer

union sybf
{   char   nsym[SYMMAXSZ + 4]; // Normal symbol
    struct locsym lsym;		// Local symbol
};

// Define structure for value of expression

// WARNING: This is byte order dependent - this definition is for
//   low order byte first machines!!

struct _vl
{   union
	{	long val;
		struct
		{   uchar valc0;
			uchar valc1;
			uchar valc2;
			uchar valc3;
		};
    };					// Numeric value
    SY   *sym;			// Pointer to symbol table entry for value
    int   psect;		// Psect number or selector for value
    char  type;			// Value type
    char  size;			// Value size
    char  kind;			// Value kind
    uchar flags;		// Flag bits
};

// Define value types

#define VLT_VAL   0		// General value
#define VLT_BYTE  1		// Byte value
#define VLT_WORD  2		// Word value
#define VLT_LONG  3		// Long value
#define VLT_QUAD  4		// Quad value

// Define value kinds

#define VLK_OFS 0		// Offset value
#define VLK_ABS 1		// Absolute value
#define VLK_ABA 2		// Absolute address
#define VLK_EXT 3		// External value
#define VLK_MSC 4		// Msect value
#define VLK_SEG 5		// Segment value
#define VLK_REG 6		// Register name
#define VLK_SEL 7		// Selector value
#define VLK_SLX 8		// Selector of external value

// Define bits for flags in VL

#define VLF_FFAR     0x10	// Far value (_F_ suffix on symbol which is a
							//   kludge added to let GCC generate XOS svcs!)
#define VLF_FAR      0x08	// Far value (includes selector)
#define VLF_REL      0x04	// Relative value
#define VLF_UNDEF    0x02	// Value is undefined
#define VLF_VOLATILE 0x01	// Value is volative (depends on one or more
							//   undefined or relocatable symbols)

// Define structure for operand data returned by getopr

// WARNING: This is byte order dependent - this definition is for
//   low order byte first machines!!

struct op
{   VL     op_val;			// Numeric value of operand
    uchar  op_type;			// Operand type
    uchar  op_size;			// Operand size
    uchar  op_modrm;		// Value for mod-r/m byte
    uchar  op_reg;			// Operand register value
    uchar  op_apfx;			// Address size prefix byte value
    uchar  op_nab;			// Number of address bytes needed
    uchar  op_seg;			// Segment prefix byte value
    uchar  op_asize;		// Address size
    ushort op_sib;			// Value for sib byte
	char   exprsn[XSTRMAX];	// Expression string for volatile expressions
};

// Define the code list element

typedef struct
{	uchar  type;		// Binary output type (VALUE_n_x, n is size, x is
						//   U for unsigned or S for signed)
	uchar  forward;		// TRUE if forward jump
	ushort xxx;
	VL     value;		// Value structure
	char  *exprsn;		// Expression string (if volatile)
} CLVL;

struct _cl
{	CL    *next;				// Next code list element
	CL    *psnext;				// Next code list element in psect								//   psect
	uchar  type;				// Instruction type
	uchar  flags;
#ifdef _MSC_VER
	ushort xxxx;
#else
	ushort xxx;
#endif
	long   size;				// Instruction size (bytes)
	PD    *psd;
	long   lsbnum;				// Local symbol block number
	long   offset;				// Offset in psect
	long   errflag;
	union
	{	struct					// Used for CLTYPE_CODE
		{	uchar  repeat;		//   Repeat prefix
			uchar  seg;			//   Segment prefix
			uchar  xxx[2];

			ushort byte0;		//   Op-code prefix byte
			ushort byte1;		//   First op-code byte
			ushort byte2;		//   Second op-code byte
			ushort byte3;		//   Third op-code byte

			CLVL   val1;		//   First operand value
			CLVL   val2;		//   Second operand value
		};
		struct					// Used for CLTYPE_MOD
		{	int mval;			// Mod value
			int mofs;			// Mod offset
			int fill;			// Fill value
			int limit;			// Limit value
		};
#ifndef _MSC_VER
		struct					// Used for CLTYPE_DATA and CLTYPE_OFFSET
		{	CLVL vald;			//   Data value
		};
#endif
		struct					// Used for CLTYPE_SYMBOL
		{	CLVL vald;
			SY  *ssym;
		};
		SY  *lsym;				// Used for CLTYPE_LABEL
		struct					// Used for CLTYPE_TEXT
		{	ushort pagenum;		//   Page number
			ushort linenum;		//   Line number on page
			long   lineno;		//   Line number in source file
			char  *srcfile;		//   Source file
			char   source[1];	//   Source text buffer
		};
		struct					// Used for CLTYPE_ASCII
		{	char  adata[1];
		};
	};
};

#define CL_TEXT_SZ   (offsetof(CL, source) + 1)
#define CL_DUMMY_SZ  (offsetof(CL, errflag) + 4)
#define CL_LABEL_SZ  (offsetof(CL, lsym) + 4)
#define CL_SYMBOL_SZ (offsetof(CL, ssym) + 4)
#define CL_BLK_SZ    (offsetof(CL, errflag) + 4)
#define CL_OFFSET_SZ (offsetof(CL, vald) + sizeof(CLVL))
#define CL_DATA_SZ   (offsetof(CL, vald) + sizeof(CLVL))
#define CL_ASCII_SZ  (offsetof(CL, adata) + 1)
#define CL_CODE_SZ   (sizeof(CL))
#define CL_MOD_SZ    (offsetof(CL, limit) + 4)

// Define values for type in CL

#define CLT_TEXT    1	// Text
#define CLT_DUMMY   2
#define CLT_SBTTL   3
#define CLT_PSECT   4
#define CLT_MSECT   5
#define CLT_SEG     6
#define CLT_LABEL   7	// Label
#define CLT_SYMBOL  8	// Defined volatile symbol
#define CLT_BLK     9	// Block
#define CLT_DATA    10	// Data
#define CLT_ASCII   11	// ASCII string
#define CLT_MOD     12	// Modulus
#define CLT_FOFFSET 13	// Fixed offset value
#define CLT_FIXED   14	// Fixed length instruction
#define CLT_VAR     15	// Variable length instruction
#define CLT_ADDR    16	// Address value
#define CLT_VALUE   17	// Non-address value
#define CLT_VOFFSET 18	// Volatile offset value

// Define values for flags in CL

#define CLF_NLST 0x80	// Do not list line unless error
#define CLF_AS   0x20	// Need address size prefix
#define CLF_DS   0x10	// Need data size prefix

// Define structure of the segment data block

struct sd
{   struct sd *sd_next;		// Pointer to next segment data block
    struct sn *sd_nba;		// Pointer to segment name block
    int    sd_sgn;			// Segment number
    char   sd_atrb;			// Segment attributes
    char   sd_flag;			// Segment flag bits
    char   sd_type;			// Segment type
    char   sd_priv;			// Priviledge level
    struct sd *sd_linked;	// Pointer to linked segment
    long   sd_mod;			// Physical address modulus
    long   sd_addr;			// Physical address
    int    sd_select;		// Segment selector value
};

// Define bits for sd_atrb

#define SA_LARGE  0x80		// Large segment
#define SA_32BIT  0x40		// 32 bit segment
#define SA_CONF   0x20		// Conformable segment
#define SA_READ   0x10		// Readable segment
#define SA_WRITE  0x08		// Writable segment

// Define bits for sd_flag

#define SF_EXTEND 0x80		// Entendable segment
#define SF_LINKED 0x10		// Linked segment
#define SF_ADDR   0x08		// Address specified for segment

// Define values for sd_type

#define ST_DATA  1
#define ST_CODE  2
#define ST_STACK 3
#define ST_COMB  4

// Define structure of the msect data block

struct md
{   struct md *md_next;		// Pointer to next msect data block
    struct mn *md_nba;		// Pointer to msect name block
    int    md_msn;			// Msect number
    char   md_atrb;			// Msect attributes
    char   md_flag;			// Msect flag bits
    char   md_xxx[2];
    int    md_sgn;			// Segment number
    long   md_max;			// Maximum address space needed
    long   md_addr;			// Msect address
    long   md_mod;			// Msect modulus
    struct sd *md_sdb;		// Address of data block for segment
};

// Define bits for md_atrb

#define MA_READ  0x10		// Readable msect
#define MA_WRITE 0x08		// Writable msect

// Define bits for md_flag

#define MF_ADDR  0x08		// Address specified

// Define structure of the psect data block

struct pd
{   PD  *pd_next;			// Pointer to next psect data block
    PN  *pd_nba;			// Pointer to psect name block
    int  pd_psn;			// Psect number
    char pd_atrb;			// Psect attributes
    char pd_flag;			// Psect flag bits
    char pd_xxx[2];
    int  pd_msn;			// Msect number
    long pd_ofs;			// Current offset in psect
    long pd_tsize;			// Total size of psect
    long pd_lsize;			// Loaded size of psect
    long pd_address;		// Address for psect
	long pd_mod;			// Modulus for psect
    MD  *pd_mdb;			// Address of data block for msect
	CL  *clhead;
	CL **cllast;
	CL  *clvarhead;
	CL **clvarlast;
};

// Define bits for pd_atrb

#define PA_OVER  0x40		// Overlayed psect

// Define bits for pd_flag

#define PF_ADDR  0x08		// Address specified

// Define structure for segment name block

struct sn
{   SN  *sn_hash;			// Hash list link
    SN  *sn_sort;
    char sn_name[SYMMAXSZ];	// Psect name
    SD  *sn_dba;			// Pointer to psect data block
    int  sn_sgn;			// Segment number
    char sn_type;			// Symbol table entry type (always = SYT_SEG)
    char sn_xxx[3];
    int  sn_flag;			// Flag bits
};

// Define structure for msect name block

struct mn
{   struct mn *mn_hash;			// Hash list link
    struct mn *mn_sort;
    char   mn_name[SYMMAXSZ];	// Psect name
    struct md *mn_dba;			// Pointer to msect data block
    int    mn_msn;				// Msect number
    char   mn_type;				// Symbol table entry type (always = SYT_MSC)
    char   mn_xxx[3];
    int    mn_flag;				// Flag bits
};

// Define structure for psect name block

struct pn
{   struct pn *pn_hash;			// Hash list link
    struct pn *pn_sort;
    char   pn_name[SYMMAXSZ];	// Psect name
    struct pd *pn_dba;			// Pointer to psect data block
    int    pn_psn;				// Psect number
    char   pn_type;				// Symbol table entry type (always = SYT_PSC)
    char   pn_xxx[3];
    int    pn_flag;				// Flag bits
};

// Define structure for block on memory free list

struct fb
{   struct fb *fb_next;		// Link to next free list block
    char   fb_xxxx[BLKSIZE - sizeof(struct fb *)];
};

// Define structure for source block

struct sb
{   struct sb *sb_next;		// Link to next source block
    char   sb_fsp[2];		// File specification
};
#define SB_fsp sizeof(struct sb *) // Offset for file specification in sb

// Global variables

extern OP     opdd;
extern OP     opds;
extern OP     opdi;
extern struct  sd *sghead;		// Pointer to head of segment data block list

extern char   xstrbufr[];
extern int    xstrcnt;
extern char  *sympnt;			// Pointer to symbuf
extern int    version;
extern int    editnum;
extern OPTBL  opctjmpx[];
extern struct md *curmsd;
extern struct sd *sghead;
extern struct sd *sgtail;
extern struct md *mshead;
extern struct md *mstail;
extern struct pd *curpsd;
extern struct pd *pshead;
extern struct pd *pstail;
extern struct sd *cursgd;
extern struct sb *firstsrc;
extern struct sb *thissrc;
extern struct sb *lastsrc;
extern IC    *inspnt;
extern struct ic *lstinp;
extern struct ab *alspnt;
extern struct reqblk *reqhead;
extern struct reqblk *reqtail;
extern int    srclvl;
extern FILE  *srcfp[];
extern char  *srcfsp[];
extern long   savlineno[];
extern long   savlinesz[];
extern long   errflag;
extern char  *errmsgtbl[];
extern int    condlvl;
extern int    falselvl;
extern char   condtbl[];
extern int    totalerr;
extern int    mltcnt;
extern int    undcnt;

extern VL     strval;
extern CL    *strclp;
extern char   strexp[SPCEXPSZ];

extern VL     dbgval;
extern CL    *dbgclp;
extern char   dbgexp[SPCEXPSZ];

extern VL     stkval;
extern CL    *stkclp;
extern char   stkexp[SPCEXPSZ];

extern long   iradix;
extern long   lineno;
extern long   linesz;
extern long   lstseq;
extern int    pagnum;
extern int    pagcnm;
extern int    lincnt;
extern long   lsbnum;
extern int    gnacnt;
extern char  *albpnt;
extern int    albcnt;
extern int    symsize;
extern int    digcnt;
extern int    symnum;
extern struct fb *freelist;
extern SY    *hashtbl[];
extern SY    *symhead;
extern SY    *machead;
extern char  *outfsp;
extern FILE  *outfp;
extern long   sectsize;
extern int    symnum;
extern int    dbkcnt;
extern char  *dbkpnt;
extern char   dbkbuf[];
extern int    nlscnt;
extern int    crfcnt;
extern int    lstpos;
extern char  *lstfsp;
extern FILE  *lstfp;
extern char   errltrs[];
extern char   timebfr[30];
extern union  sybf symbuf;
extern CL    *clhead;
extern CL   **cllast;
extern char   digbuf[];
extern char   albbuf[];
extern char   hldchar;
extern char   stopper;
extern char   catchr;
extern char   lstmode;
extern char   ptype;
extern char   pbits;
extern char   lsymflg;
extern char   gsymflg;
extern char   listtype;
extern char   segatrb;
extern char   nopar;
extern char   sleflg;
extern char   test;
extern char   lststd;
extern char   eofflg;
extern char   ttlflg;
extern int    prmcnt;
extern char   bolflg;
extern char   eolflg;
extern char   eofreq;
extern char   ffflg;
extern char   notnull;
extern uchar  adjusted;
extern uchar  prnflg;
extern uchar  dtxflg;
extern uchar  havesection;
extern char   fhdflg;
extern char   tocflg;
extern char   tchflg;
extern char   bnoflg;
extern char   bexflg;
extern uchar  gccflg;
extern long   savesymflg;
extern char   litchr;
extern char   inxbgn;
extern char   inxend;
extern char   inxsep1;
extern char   inxsep2;
extern char   stack32;
extern char   prgname[];
extern char   hdrbfr[];
extern char   hdrstb[];
extern char   errqmsg[];
extern char   errvmsg[];
extern char   errumsg[];
extern char   erramsg[];
extern char   errtmsg[];
extern char   errvmsg[];
extern char   errpmsg[];
extern char   errmmsg[];
extern char   erromsg[];
extern char   errcmsg[];
extern char   errxmsg[];
extern char   errdmsg[];
extern char   errkmsg[];
extern char   errsmsg[];
extern char   erremsg[];
extern char   errimsg[];
extern char   errinmsg[];
extern char   errdnmsg[];
extern char   errcemsg[];
extern char   errcxmsg[];
extern char   errcomsg[];
extern char   errnpmsg[];
extern char   errummsg[];
extern char   errnamsg[];

///extern char  *errtbl[];
///extern int    errcnt;
///extern char **errpnt;

// Function prototypes

void   absbyte(long value);
void   absword(long value);
void   adrerr(void);
void   advanceline(void);
void   asmline(void);
void   assemble(void);
void   badopcode(void);
int    blkifs(void);
int    chkendx(void);
int    chkend(char chr);

void   chkhead(void);

void   chkpsect(void);
void   chkrel(VL *value);
int    chksym(char chr);
int    chksys(char chr);
long   clfixvalues(int jmpfixup, int chktrunc);
CL    *clget(int size);
void   clshow(void);
void   clinsert(CL *clp);
void   clstoretext(void);
int    cmpifs(void);
void   cntlreg(int dbit, int creg, int dreg, CL *clp);
int    compsym(SY *sym1, SY *sym2);
void   copytext(char *buffer ,int size);
int    cpendc(void);
int    cpif(void);
int    cpiff(void);
int    cpift(void);
int    cpiftf(void);
char  *defspec(char *fsp, char *none, char *ext);
void   dobinary(void);
void   dolisting();
void   eatifs(void);
void   endins(void);
void   endsect(void);
SY    *entersym(void);
void   entint(int flags, int clear);
void   errorc(void);
void   errorq(void);
void   errorx(void);
long   expandjump(CL *clp);
void   exprsn(VL *val);
void   exprsnwo(VL *val, char *inpnt);
SY    *findsym(int def);
void   finish(void);
void   finlisting(void);
void   general(VL *value, char store);
void   getabs(VL *val);
char   getadg(char chr);
void   getals(void);
long   getanum(int radix);
AB    *getargs(void);
void   getatom(VL *value);
FB    *getblock(void);
void   getconcat(void);
void   getdmy(char chr, int genflg);
SB    *getfsp(char *arg, int offset, char *dftext);
int    getifx(void);
int    getinx(void);
SY    *getnxs(void);
int    getopr(OP *opd, OPTBL *opt, int rmask);
int    getopr1(OP *opd, OPTBL *opt, int rmask);
int    getscale(OP *opd);
int    getsegnumms(int msect);
int    getsegnumps(int psect);
int    getsrc(void);
int    getsym(char chr);
int    getsyq(void);
void   givblock(struct fb *block);
void   givlist(struct ab *block);
void   givprm(void);
int    havearg(char *);
void   helpprint(char *helpstring, int state, int newline);
void   hndlatr(struct atributes *tbl, int size, struct pd *block);
void   icallf(OPTBL *opt, CL *clp);
void   icalli(OPTBL *opt, CL *clp);
void   iclear(OPTBL *opt, CL *clp);
void   idblshf(OPTBL *opt, CL *clp);
void   ientrins(OPTBL *opt, CL *clp);
void   ifb(void);
void   ifdef(void);
void   ifdif(void);
int    ifend(char chr);
void   ifeq(void);
void   iffalse(void);
void   ifge(void);
void   ifgt(void);
void   ifidn(void);
void   ifle(void);
void   iflt(void);
void   ifnb(void);
void   ifndf(void);
void   ifne(void);
void   ifstsw(OPTBL *opt, CL *clp);
void   iftrue(void);
void   iimul(OPTBL *opt, CL *clp);
void   iimul3op(OP *dopd, OP *sopd, OP *iopd, OPTBL *opt, CL *clp);
void   iincdec(OPTBL *opt, CL *clp);
void   iinout(OPTBL *opt, CL *clp);
void   iinoutop(int byte, OPTBL *opt, CL *clp);
void   iintins(OPTBL *opt, CL *clp);
void   ijump(OPTBL *opt, CL *clp);
void   ijumpf(OPTBL *opt, CL *clp);
void   ijumpi(OPTBL *opt, CL *clp);
void   iloop(OPTBL *opt, CL *clp);
void   imovins(OPTBL *opt, CL *lcp);
int    incond(void);
void   insbody(int type, struct body *body, struct ab *argval);
void   invmac(SY *sym);
void   ionebyte(OPTBL *opt, CL *clp);
void   ionebytx(OPTBL *opt, CL *clp);
void   ioneopr(OPTBL *opt, CL *clp);
void   ioneoprf(OPTBL *opt, CL *clp);
void   ioneoprw(OPTBL *opt, CL *clp);
void   ipushpop(OPTBL *opt, CL *clp);
void   iret(OPTBL *opt, CL *clp);
void   irpdmy(void);
void   ishrot(OPTBL *opt, CL *clp);
void   istrins(OPTBL *opt, CL *clp);
void   ithrbyte(OPTBL *opt, CL *clp);
void   itwobyte(OPTBL *opt, CL *clp);
void   itwoopr(OPTBL *opt, CL *clp);
void   itwooprf(OPTBL *opt, CL *clp);
void   ixchg(OPTBL *opt, CL *clp);
void   listent(SY *sym, int mac, int endflag);
void   listesym(int cnt, char *fmt, int flag);
void   listinit(void);
void   listline(char *line, long err);
void   listnxl(long);
void   liststr(char *str, long err);
void   listtoc(void);
SY    *looksym(void);
int    macarg(void);
int    macnest(void);
void   matraddr(struct md *msd);
void   matrmax(struct md *msd);
void   matrmod(struct md *msd);
void   matrread(struct md *msd);
void   matrwrite(struct md *msd);
void   movmem1(OPTBL *opt, CL *clp);
void   movmemimm(OPTBL *opt, CL *clp);
void   movmemreg(OPTBL *opt, CL *clp);
void   movrmmr(int dbit, OP *ropd, OP *mopd, OPTBL *opt, CL *clp);
void   movreg1(OPTBL *opt, CL *clp);
void   movregimm(OPTBL *opt, CL *clp);
void   movregmem(OPTBL *opt, CL *clp);
void   movregreg(OPTBL *opt, CL *clp);
SY    *nchsty(void);
void   nextpage(void);
int    nextref(void);
char   nxthrtn(long value);
char   nxtnb0(int chr);
char   nxtnbc(void);
char   nxtchar(void);
char   nxtcrtn(char chr);
void   op2mem1(OPTBL *opt, CL *clp);
void   op2memimm(OPTBL *opt, CL *clp);
void   op2memreg(OPTBL *opt, CL *clp);
void   op2reg1(OPTBL *opt, CL *clp);
void   op2regimm(OPTBL *opt, CL *clp);
void   op2regmem(OPTBL *opt, CL *clp);
void   op2regreg(OPTBL *opt, CL *clp);
void   opcdout(long value, OP *opd, OPTBL *opt, CL *clp);
void   oprdout(long extra, OP *opd, CL *clp);
void   pp2align(void);
void   paddrw(void);
void   paddrl(void);
void   psection(void);
void   pseg(void);
void   pseg16(void);
void   pseg32(void);
void   palmex(void);
void   pascii(void);
void   pascil(void);
void   pasciu(void);
void   pasciz(void);
void   patraddr(struct pd *psd);
void   patrmod(struct pd *psd);
void   patrover(struct pd *psd);
void   pblkb(void);
void   pblkw(void);
void   pblkl(void);
void   pblmex(void);
void   pcot(void);
void   pcref(void);
void   pdebug(void);
void   peerror(void);
void   pendc(void);
void   pentry(void);
void   persym(void);
void   peven(void);
void   pexpb(void);
void   pexpw(void);
void   pexpl(void);
void   pexport(void);
void   pextern(void);
void   pfile(void);
void   pident(void);
void   pif(void);
void   piff(void);
void   pift(void);
void   pimport(void);
void   pinclud(void);
void   pintern(void);
void   pirp(void);
void   pirpc(void);
void   plbex(void);
void   plcomm(void);
void   plist(void);
void   plnkseg(void);
void   ploc(void);
void   plsb(void);
void   plsym(void);
void   pmacro(void);
void   pmsect(void);
void   pmod(void);
void   pnchar(void);
void   pncot(void);
void   pncref(void);
void   pngsym(void);
void   pnlbex(void);
void   pnlist(void);
void   pnlmex(void);
void   pnlsym(void);
void   pnrname(void);
void   podd(void);
void   ppage(void);
void   pparm(void);
void   pprint(void);
void   pprname(void);
void   pproc(void);
void   ppsect(void);
void   pquad(void);
void   pradix(void);
void   prept(void);
void   prequir(void);
void   psbttl(void);
void   psleb128(void);
void   pslmex(void);
void   pstack(void);
void   pstart(void);
void   pstk16(void);
void   pstk32(void);
void   pstype(void);
void   ptitle(void);
void   puleb128(void);
void   putaddrl(VL *value);
void   putaddrw(VL *value);
void   putbyte(int type, VL *value);
void   putlong(int type, VL *value);
void   putsel(VL *value);
void   putword(int type, VL *value);
int    queerr(void);
void   recalcaddr(CL *clp, VL *vlp, char *exp);
void   refent(long word, struct rf *ref);
void   refsym(SY *sym, int def);
int    reptnest(void);
void   resetlst(void);
int    resolvemod(int oldmod, int newmod);
void   satr16b(struct sd *sgd);
void   satr32b(struct sd *sgd);
void   satrext(struct sd *sgd);
void   satrcode(struct sd *sgd);
void   satrcomb(struct sd *sgd);
void   satrconf(struct sd *sgd);
void   satrdata(struct sd *sgd);
void   satrlarge(struct sd *sgd);
void   satraddr(struct sd *sgd);
void   satrmod(struct sd *sgd);
void   satrpriv(struct sd *sgd);
void   satrread(struct sd *sgd);
void   satrselect(struct sd *sgd);
void   satrstack(struct sd *sgd);
void   satrwrite(struct sd *sgd);
void   setbody(void);
int    setref(SY *sym);
void   simpleexp(VL *val, char *inpnt);
void   sizerr(void);
void   skipcond(void);
void   skiprest(void);
void   strsymvalue(SY *sym, VL *vlp);
SY    *sorttbl(SY *sym);
SY    *srchtbl(char *sym, void *tbl, int tblsize, int strsize, int entsize);
void   storevalue(CLVL *val, int type, VL *vl, char *exprsn);
void   strsect(char type);
void   tblhead(void);
void   tblnxtl(void);
void   valueout(OP *opd, OPTBL *opt, CL *clp);
void   valuesout(OP *opd, OPTBL *opt, CL *clp);
void   wrtblk(void);
