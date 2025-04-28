// Define XWP file header format

typedef struct
{	char  label[4];				// File label ("\0xFFXWP")
	ushort hdrlen;				// Length of header data (does not include
								//   this and the label value)
    ushort majver;				// Major version number
	ushort minver;				// Minor version number
	ushort prglen;				// Length of the program
	uchar  reserved[10];		// Reserved bytes (must be include)
} XWPHDR;

#define XWPMAJOR 0
#define XWPMINOR 1

#define OP_EXIT    0x00			// Exit
#define OP_CMP     0x01			// Compare
#define OP_CMPN    0x02			// Compare/count
#define OP_CMPI    0x03			// Compare, ignore case
#define OP_CMPNI   0x04			// Compare/count, ignore case
#define OP_CALL    0x05			// Call defined function
#define OP_CALLI   0x06			// Call internal function
#define OP_LVRTN   0x07			// Leave, return
#define OP_LVRTNV  0x08			// Leave, return with value
#define OP_RTN     0x09			// Return
#define OP_RTNV    0x0A			// Return with value
#define OP_ADJSTK  0x0B			// Adjust stack
#define OP_ENTER   0x0C			// Enter

#define OP_PUSH0N  0x10			// Push small (4-bit) numeric value
#define OP_PUSH1N  0x20			// Push 1-byte (12-bit) numeric value
#define OP_PUSH2N  0x30			// Push 2-byte (16-bit scaled) numeric value
#define OP_PUSH3N  0x40			// Push 3-byte (24-bit scaled) numeric value
#define OP_PUSH4N  0x50			// Push 4-byte (32-bit scaled) numeric value
#define OP_PUSH6N  0x60			// Push 6-byte (48-bit scaled) numeric value
#define OP_PUSH8M  0x70			// Push 8-byte (64-bit scaled) numeric value

#define OP_POPG    0x80			// Pop global variable
#define OP_POPGX   0x81			// Pop global variable indexed
#define OP_POP2MG  0x82			// Pop global variable to memory
#define OP_POP2MGX 0x83			// Pop global variable to memory indexed
#define OP_POPL    0x84			// Pop local variable
#define OP_POPLX   0x85			// Pop local variable indexed
#define OP_POP2ML  0x86			// Pop local variable to memory
#define OP_POP2MLX 0x87			// Pop local variable to memory indexed

#define OP_LOADG   0x88			// Push global variable
#define OP_LOADGX  0x89			// Push global variable indexed
#define OP_LOADL   0x8A			// Push local variable
#define OP_LOADLX  0x8B			// Push local variable indexed
#define OP_ONSTK   0x8C			// Leave value on stack after POPxxx or CMPxxx
#define OP_PUSHCC  0x8D			// Push condition code

#define OP_GETSIZE 0x90			// Get variable size
#define OP_GETTYPE 0x91			// Get variable type

#define OP_PUSHSS  0xA0			// Push short string value (4-bit count)
#define OP_PUSHLS  0xB0			// Push Long string value (12-bit count)

#define OP_JMPCCMIN 0xC0
#define OP_JMPE    0xC0			// Jump if equal
#define OP_JMPNE   0xC1			// Jump if not equal
#define OP_JMPG    0xC2			// Jump if greater than
#define OP_JMPLE   0xC3			// Jump if less than or equal
#define OP_JMPL    0xC4			// Jump if less than
#define OP_JMPGE   0xC5			// Jump if greater tan or equal
#define OP_JMPCCMAX 0xC5
#define OP_JMP     0xC6			// Jump always


#define OP_NOT     0xD0			// Logical compliment
#define OP_COM     0xD1			// 1's compliment
#define OP_NEG     0xD2			// 2's compliment
#define OP_AND     0xD3			// AND
#define OP_OR      0xD4			// OR
#define OP_XOR     0xD5			// XOR
#define OP_ADD     0xD6			// ADD
#define OP_SUB     0xD7			// SUB
#define OP_RSUB    0xD8			// Reverse SUB
#define OP_MUL     0xD9			// MUL
#define OP_DIV     0xDA			// DIV
#define OP_RDIV    0xDB			// Reverse DIV
#define OP_REM     0xDC			// Remainder
#define OP_RREM    0xDD			// Reverse remainder
#define OP_SHFTL   0xDE			// Left shift
#define OP_SHFTR   0xDF			// Right shift

#define OP_SWAP    0xE0			// Swap TOS and TOS-1
#define OP_DUP     0xE1			// Duplicate TOS
#define OP_CAT     0xE2			// Concatenate strings

#define OP_PUSHZN  0xF0			// Push zero numeric value
#define OP_PUSHNN  0xF1			// Push null numeric value
#define OP_PUSHNS  0xF2			// Push null string
#define OP_PUSHVS  0xF3			// Push very long string value (16-bit count)
#define OP_PUSHXS  0xF4			// Push extra long string value (24-bit count)
#define OP_PUSHD   0xF5			// Push date-time value (11-bytes)
#define OP_PUSHDMN 0xF6			// Push minimum date-time value
#define OP_PUSHDMX 0XF7			// Push maximum date-time value

// Define internal function indexes

#define IF_PRINT      1
#define IF_PRINTF     2
#define IF_SPRINTF    3
#define IF_GETGETARG  4
#define IF_GETPOSTARG 5
#define IF_SESSION    6
#define IF_DUMPVAR    7
#define IF_INVINQ     8
#define IF_INVGET     9
