//------------------------------------------------------------------------------
//
//  XOS.H - Standard XOS I/O header
//
//  Edit History:
//  -------------
//  09/05/92(brn) - Added header to include file
//   6Jun95 (fpj) - Resynchronized with XOS.PAR.
//
//------------------------------------------------------------------------------

// ++++
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

#ifndef _XOS_H_
#define _XOS_H_

#include <_prolog.h>

#ifndef _XOSTIME_H_
#include <xostime.h>
#endif

// Define general limit values

#define FILESPCSIZE 256

// Define some ASCII characters

#define EOT	0x04		// End of text
#define BEL	0x07		// Bell
#define BSP	0x08		// Backspace
#define HT 	0x09		// Horizontal tab
#define LF 	0x0A		// Line feed
#define FF 	0x0C		// Form feed
#define CR 	0x0D		// Carriage return
#define SFO	0x0E		// Shift out
#define SFI	0x0F		// Shift in
#define XON	0x11		// XON (^Q)
#define XOF	0x13		// XOF (^S)
#define SYN	0x16		// Sync
#define SUB	0x1A		// SUB (used as EOF flag in files)
#define ESC	0x1B		// ESC
#define SPA	0x20		// Space
#define LA 	0x3C		// Left angle
#define RA 	0x3E		// Right angle
#define LB 	0x7B		// Left brace
#define RB 	0x7D		// Right brace
#define DEL	0x7F		// Delete (Rub-out)

// Define global selectors which user programs can use

#define GS_UCODE 0x0018		// User code segment
#define GS_UDATA 0x0020		// User data segment

#define SEGMAX 32t			// Maximum number of user segments

// Define offset in the real mode data area

#define rmda_base 	0x0EC000
#define rmda_stack	0x0ECFE0  // Real mode stack
#define rmda_vmESP	0x0ECFF0  // Current protected mode stack pointer
#define rmda_vmSS 	0x0ECFF4
#define rmda_pmESP	0x0ECFF8  // Current real mode stack pointer
#define rmda_pmSS 	0x0ECFFC

// Define segment types and associated bits

#define ST_TOP      0x80000000  // Search backwards to allocate segment

#define ST_32RODATA 	1	// 32 bit read only data segment
#define ST_32RWDATA 	2	// 32 bit read/write data segment
#define ST_32STACK  	3	// 32 bit stack segment
#define ST_32NXOCODE	4	// 32 bit execute only normal code segment
#define ST_32NXRCODE	5	// 32 bit execute/read normal code segment
#define ST_32CXOCODE	6	// 32 bit execute only conformable code segment
#define ST_32CXRCODE	7	// 32 bit execute/read conformable code segment
#define ST_16RODATA 	9	// 16 bit read only data segment
#define ST_16RWDATA 	10	// 16 bit read/write data segment
#define ST_16STACK  	11	// 16 bit stack segment
#define ST_16NXOCODE	12	// 16 bit execute only normal code segment
#define ST_16NXRCODE	13	// 16 bit execute/read normal code segment
#define ST_16CXOCODE	14	// 16 bit execute only conformable code segment
#define ST_16CXRCODE	15	// 16 bit execute/read conformable code segment
#define ST_MAX      	15

// Define page type bits

#define PG_VIRTUAL	0x08	// Virtual page
#define PG_EXECUTE	0x04	// Executable page (not supported)
#define PG_WRITE  	0x02	// Writable page
#define PG_READ   	0x01	// Readable (always set)

// Define termination codes (positive values indicate process has been stopped,
//   negative values indicate process has been killed)

#define TC_EXIT   	1	// Normal exit
#define TC_KILL   	2	// Killed
#define TC_PIPE   	3	// No input available from pipe
#define TC_NOVECT 	4	// No user vector setup for condition
#define TC_BDUSTK 	5	// Bad user stack address
#define TC_NOMEM  	6	// No memory available
#define TC_UNIMOP 	7	// Unimplemented operation
#define TC_RUNFAIL	8	// RUN failure
#define TC_CTRLC    9	// Cntl-C
#define TC_SUBTSK 	11	// Sub-task error
#define TC_MEMCRPD	12	// DOS memory allocation data corrupted
#define TC_DPMI   	13	// Fatal DPMI error
#define TC_NODOS  	14	// DOS environment not set up
#define TC_DOSBADE	15	// Invalid error code returned for DOS function
#define TC_ACCT     16	// Accounting violation
#define TC_BDCRIT   17	// Bad CRIT structure

// Define software interrupt numbers

// Define signal vector numbers - There are 255 possible signal vectors (1
//   through 255). The first 471 vectors are predefined, mainly for various
//   processor traps. These vectors cannot be generated by softare except by
//   the defined condtions. The defintions correspond to the IA-32 trap numbers
//   except that the divide error trap (which is 0 in IA-32) is redefined.

// Vectors 0x01 through 0x1F are processor traps/faults. These mostly match
//  the IA-32 definitions.

#define VECT_DEBUG   0x01	// Debug trap
#define VECT_DIVERR  0x02	// Divide error
#define VECT_BRKPNT  0x03	// Breakpoint
#define VECT_INTO    0x04	// INTO instruction trap
#define VECT_BOUND   0x05	// BOUND instruction trap
#define VECT_ILLINS  0x06	// Illegal instruction trap
#define VECT_FPPNAVL 0x07	// Floating pointer processor not available trap
#define VECT_DBLEXP  0x08	// Double exception
#define VECT_FPPSOVR 0x09	// Floating point processor segment overrun
#define VECT_ITSS    0x0A	// Invalid task state segment
#define VECT_SEGNP   0x0B	// Segment not present
#define VECT_STKERR  0x0C	// Stack error
#define VECT_PROT    0x0D	// Memory protection error
#define VECT_PAGEFLT 0x0E	// Page fault
#define VECT_FPUERR  0x10	// Floating point processor error
#define VECT_ALNCHK  0x11	// Alignment check
#define VECT_MCHCHK  0x12	// Machine check
#define VECT_SIMDFX  0x13	// SIMD floating point exception

// Vectors 0x0F and 0x14 through 0x1F are reserved and should not be used.

// Vectors 0x20 through 0x2F are XOS traps/faults/signals.

#define VECT_EXIT    0x20	// Process termination trap
#define VECT_PTERM   0x21	// Child process terminated signal
#define VECT_TTERM   0x22	// Thread terminated signal
#define VECT_CNTC    0x23	// Control C signal
#define VECT_CNTP    0x24	// Control P signal
#define VECT_HNGUP   0x25	// Controlling terminal hung up signal
#define VECT_ABORT   0x26	// Process aborted trap
#define VECT_SETENV  0x2F	// Request to set environment string

#define VECT_MINUSER 0x30	// Minimum user vector

// Vectors 0x25 through 0x2F are reserved and should not be used.

// Define offsets in the buffer filled in by the UF_GTCFG svcUtility function

#define cf_totmem	0	// (4) Total memory in system (pages)
#define cf_avlmem	4	// (4) Available memory in system (pages)
#define cf_hrddsk	8	// (2) Number of hard disk units
#define cf_flpdsk	10	// (2) Number of floppy disk units
#define cf_serprt	12	// (2) Number of serial ports
#define cf_parprt	14	// (2) Number of parallel ports
#define cf_SIZE  	16

// Define standard device handles

#define DH_STDIN 	1	// Standard input device
#define DH_STDOUT	2	// Standard output device
#define DH_STDERR	3	// Standard error output device
#define DH_STDPRN	4	// Standard listing output device
#define DH_STDTRM	5	// Controlling terminal for session (cannot be
						//  redirected)
#define DH_FV    	6	// First variable device handle

// Define offsets in the block which specifies a device characteristics value

#define dcv_type   	0	// (1) Description byte
#define dcv_vallen 	1	// (1) Length of value field
#define dcv_name   	2	// (8) Name
#define dcv_infopnt	10	// (8) Address of information buffer
#define dcv_ibfrlen	18	// (2) Size of the information buffer
#define dcv_istrlen	20	// (2) Length of the information string
						// Note:  Following offsets are defined assuming
						//	that no information pointer is present -
						//	if there is an information pointer, add
						//	12t to each offset
#define dcv_value  	10	// (8) Numeric value or address of string value
						//  buffer
#define dcv_vbfrlen	18	// (2) Size of the string value buffer
#define dcv_vstrlen	20	// (2) Length of the returned string value
#define dcv_SIZE   	22	// Total size of a long string value entry

// Define offsets in the block which specifies an add unit characteristics value

#define aucv_type  	0	// (1) Description byte
#define aucv_name  	1	// (7) Name
#define aucv_value 	8	// (8) Numeric value or address of string value
						//	 buffer
#define aucv_valsz 	16	// (2) Size of the string value buffer
#define aucv_vallen	18	// (2) Length of the returned string value

// Define offsets in the queued IO functions argument block

// Define queued IO argument block structure (QAB)

#ifndef _MSC_VER
#pragma pack (4)
#endif

typedef struct
{	ushort func;		// Function
	ushort status;		// Returned status
	long   error;		// Error code (returned by all functions)
	long   amount;		// Amount transfered or items processed
	long   handle;		// Device handle (returned by open, input for all
						//  others)
	char   vector;		// Vector for interrupt
	char   rsvd[3];		// Reserved
	long   option;		// Options or command
	long   count;		// Count for transfer
	char  *buffer1;		// Address of first data buffer
	void  *buffer2;		// Address of second data buffer
	void  *parm;		// Address of parameter area
} QAB;

////#pragma pack (1)

// Define values for qab_func for the svcIoQueue system service

#define QFNC_WAIT     	0x8000	// Wait until function is complete
#define QFNC_DIO      	0x4000	// Force direct IO if possible
#define QFNC_QIO    	0x2000	// Force queued IO if possible
#define QFNC_OOBD     	0x0400	// Out-of-band data (output functions only)
#define QFNC_POLL     	0x0200	// Poll (output functions for half-duplex
								//   devices only)
#define QFNC_OPEN     	1		// Open device/file
#define	QFNC_DEVPARM  	2		// Device parameter function
#define QFNC_NEXTFILE   3		// Next file
#define QFNC_DEVCHAR  	4		// Device characteristics
#define QFNC_DELETE   	5		// Delete file
#define QFNC_RENAME   	6		// Rename file
#define QFNC_PATH       8		// Path function
#define QFNC_CLASSFUNC	9		// Class functions
#define QFNC_INBLOCK  	12		// Input block
#define QFNC_OUTBLOCK 	14		// Output block
#define QFNC_TRMFUNC  	17		// Terminal functions
#define QFNC_SPECIAL  	19		// Special device functions
#define QFNC_LABEL    	20		// Get device label
#define QFNC_COMMIT   	21		// Commit data
#define QFNC_CLOSE    	22		// Close file

// Define values for qab_func for the svcIoRun system service

#define RFNC_WAIT     	0x8000	// Wait until function is complete
#define RFNC_RUN      	1		// Run program
#define RFNC_LOAD     	2		// Load overlay

// Define bits used in qab_status

#define QSTS_DONE  	0x8000		// Operation is complete
#define QSTS_ACTIVE	0x4000		// Operation is active
#define QSTS_WAIT  	0x2000		// Should wait for device (XOS internal use)
#define QSTS_REDO  	0x1000		// Operation should be re-done (XOS internal
								//   use)
#define QSTS_FINAL 	0x0020		// Final input
#define QSTS_EXCPTN	0x0010		// Exception event has occured
#define QSTS_OOBD  	0x0008		// Out-of-band input data
#define QSTS_CANCEL	0x0004		// Operation canceled before started
#define QSTS_ABORT 	0x0002		// Operation aborted
#define QSTS_TRUNC 	0x0001		// Data truncated

// Define bits for qab_option for the QFNC_PATH function

#define POPT_SETPATH 0x0001		// Set path from qab_buffer1 string
#define POPT_GETPATH 0x0002		// Get path to qab_buffer1 buffer (qab_count
								//   specifies size of the available buffer)
#define POPT_GETDEV  0x0004		// Get device name
#define POPT_LENGTH  0x0010		// Get lenth for returned path only

// Define bits for logical name definitions

#define LGNMB_ROOTED 0x04		// Rooted
#define LGNMB_SUBSTD 0x02		// Substituted
#define LGNMB_MULT   0x01		// Multiple definitions

// Each parameter consists of a two byte header followed by a 0 to 32 byte value

// Define values for the first parameter header byte - this byte specifies the
//   format of the parameter value and also contains the system's error
//   indication - these values also apply for the first device characteristics
//   header byte

#define PAR_SET    	0x80	// Set (use) value of this parameter
#define PAR_GET    	0x40	// Get (return) value of this parameter
							//   Both PAR_SET and PAR_GET clear indicates
							//   the end of the parameter list
#define PAR_ERROR  	0x20	// Error in this parameter
#define PAR_INFO   	0x10	// Return parameter information
#define PAR_REP    	0x0F	// Value representation
#define   REP_DECV   	1	//   Decimal value
#define   REP_HEXV   	2	//   Hex value
#define   REP_N         2
#define   REP_OCTV   	3	//   Octal value
#define   REP_BINV   	4	//   Binary value
#define   REP_DECB   	5	//   Decimal bytes
#define   REP_HEXB   	6	//   Hex bytes
#define   REP_OCTB   	7	//   Octal bytes
#define   REP_VERN   	8	//   Version number (8 bits . 8 bits . 16 bits)
#define   REP_TIME   	9	//   Time value
#define   REP_DATE   	10	//   Date value
#define   REP_DT     	11	//   Date/time value
#define   REP_DATAB  	12	//   Data bytes
#define   REP_TEXT   	13	//   Text bytes
#define   REP_DATAS  	14	//   Data string
#define   REP_STR    	15	//   String value
#define   REP_S         15

// The second header byte specifies the length of the value in the parameter list
//   in bytes - a value of 0FFh initicates a long string value (8 byte pointer
//   followed by 2 byte buffer length and 2 byte string length - buffer length is
//   used by system, string length field set by system if IOPAR_GET specified

#define SIZE_LNGSTR	0x0FF	// Long string value

// Define values for the 3rd and 4th parameter header bytes - these byte specify
//   the parameter being set or obtained - values below 8000h are common to all
//   devices, values above 128t are device dependent and may only be used
//   following a IOPAR_CLASS parameter with PAR_SET set which specifies the
//   actual device class (this is intended to make sure that the intended
//   function is really done!)

#define IOPAR_GENBASE     0x0000
#define IOPAR_FILOPTN     0x0001 //  s4  File option bits
#define IOPAR_FILSPEC     0x0002 // g S  File specification string
#define IOPAR_DEVSTS      0x0003 // g 4  Device status
#define IOPAR_UNITNUM     0x0004 // g 4  Unit number
#define IOPAR_GLBID       0x0005 // g 16 Global device ID
#define IOPAR_DELAY       0x0006 //  s4  IO delay value (fractional days)
#define IOPAR_TIMEOUT     0x0007 //  s4  IO time-out value (fractional days)
#define IOPAR_INPSTS      0x0008 // g 1  Device input ready status
#define IOPAR_OUTSTS      0x0009 // g 1  Device output ready status
#define IOPAR_INPQLMT     0x000A // gs1  Input queue limit
#define IOPAR_OUTQLMT     0x000B // gs1  Output queue limit
#define IOPAR_SIGVECT1    0x000C // gs1  First signal vector
#define IOPAR_SIGVECT2    0x000D // gs1  Second signal vector
#define IOPAR_SIGDATA     0x000E // gs4  Signal data
#define IOPAR_NUMOPEN     0x000F // g 2  Number of times device is open
#define IOPAR_BUFRLMT     0x0010 // gs2  Internal buffering limit

// Define file system IO parameters

#define IOPAR_FILBASE     0x0100
#define IOPAR_DIRHNDL     0x0101 //  s2  Directory handle for search
#define IOPAR_SRCATTR     0x0102 //  s2  File attributes for search
#define IOPAR_FILATTR     0x0103 // gs2  File attributes
#define IOPAR_DIRPOS      0x0104 // gs4  Directory position
#define IOPAR_ABSPOS      0x0105 // gs4  Absolute pos. in file
#define IOPAR_RELPOS      0x0106 // gs4  Relative pos. in file (returns abs.)
#define IOPAR_EOFPOS      0x0107 // gs4  Pos. in file rel. to EOF (returns abs.)
#define IOPAR_VBOF        0x0108 // gs4  Virtual beginning of file position
#define IOPAR_LENGTH      0x0109 // gs4  Written length of file (bytes)
#define IOPAR_REQALLOC    0x010A // gs4  Request file space allocation (bytes)
#define IOPAR_RQRALLOC    0x010B // gs4  Require file space allocation (bytes)
#define IOPAR_GRPSIZE     0x010C // gs4  Allocation group size (bytes)
#define IOPAR_ADATE       0x010D // gs8  Last access date/time
#define IOPAR_CDATE       0x010E // gs8  Creation date/time
#define IOPAR_MDATE       0x010F // gs8  Modify date/time
#define IOPAR_PROT        0x0110 // gs4  File protection
#define IOPAR_OWNER       0x0111 // gsS  Owner name
#define IOPAR_USER        0x0112 // gsS  User name for access
#define IOPAR_SETLOCK     0x0113 // gs8  Set file lock
#define IOPAR_CLRLOCK     0x0114 // gs8  Clear file lock
#define IOPAR_SHRPARMS    0x0118 // gs4  DOS file sharing parameter values
#define IOPAR_ACSNETWK    0x0119 //  s0  Use network access field

// Define terminal IO parameters

#define IOPAR_TRMBASE     0x0200
#define IOPAR_TRMSINPMODE 0x0201 // gs4  Set input modes
#define IOPAR_TRMCINPMODE 0x0202 // gs4  Clear input modes
#define IOPAR_TRMSOUTMODE 0x0203 // gs4  Set output modes
#define IOPAR_TRMCOUTMODE 0x0204 // gs4  Clear output modes
#define IOPAR_TRMBFRLIMIT 0x0205 //  s4  Input buffer limit value
#define IOPAR_TRMCLRBUFR  0x0206 //  s1  Clear buffer(s)
#define IOPAR_TRMCURTYPE  0x0207 // gs2  Cursor type
#define IOPAR_TRMCURPOS   0x0208 // gs4  Cursor position
#define IOPAR_TRMDISPAGE  0x0209 // gs1  Display page
#define IOPAR_TRMSPSTATUS 0x020A // g 2  Serial port status
#define IOPAR_TRMSPBREAK  0x020B // gs1  Break control
#define IOPAR_TRMSPMODEM  0x020C //  s1  Modem control
#define IOPAR_TRMSETDFC   0x020D
#define IOPAR_TRMCLRDFC   0x020E
#define IOPAR_TRMLSTDFC   0x020F
#define IOPAR_TRMALLDFC   0x0210
#define IOPAR_TRMCPVECT   0x0211 // gs2  Control-P signal vector
#define IOPAR_TRMCPDATA   0x0212 // gs2  Control-P signal data
#define IOPAR_TRMMCVECT   0x0213 // gs2  Modem control signal vector
#define IOPAR_TRMMCDATA   0x0214 // gs2  Modem control signal data

// Define disk IO parameters

#define IOPAR_DSKBASE     0x0300
#define IOPAR_DSKFSTYPE   0x0301 // g 1  File system type
#define IOPAR_DSKSECTSIZE 0x0302 // g 2  Sector size
#define IOPAR_DSKCLSSIZE  0x0303 // g 1  Cluster size
#define IOPAR_DSKTTLSPACE 0x0304 // g 4  Total space (in clusters)
#define IOPAR_DSKAVLSPACE 0x0305 // g 4  Available space (in clusters)
#define IOPAR_DSKNUMHEAD  0x0306 // g 1  Number of heads
#define IOPAR_DSKNUMSECT  0x0307 // g 1  Number of sectors
#define IOPAR_DSKNUMCYLN  0x0308 // g 1  Number of cylinders
#define IOPAR_DSKBLOCK    0x0309 // g 4  Get last disk block accessed

// Define tape IO parameters

#define IOPAR_TAPBASE     0x0400
#define IOPAR_TAPRECMIN   0x0401 // g 4  Minimum record length
#define IOPAR_TAPRECMAX   0x0402 // g 4  Maximum record length
#define IOPAR_TAPRECLEN   0x0403 // gs4  Current fixed record length
#define IOPAR_TAPDENSITY  0x0404 // gs4  Tape density
#define IOPAR_TAPGAPLEN   0x0405 // gs1  Gap length
#define IOPAR_TAPBFRMODE  0x0406 // gs1  Buffering mode

// Define network IO parameters

#define IOPAR_NETBASE     0x0500
#define IOPAR_NETSUBUMASK 0x0501 // g 4  Sub-unit bit mask
#define IOPAR_NETPROTOCOL 0x0502 // gs2  Protocol
#define IOPAR_NETLCLPORT  0x0503 // gs2  Local port number
#define IOPAR_NETRMTHWAS  0x0504 // gs8  Remote hardware address (send)
#define IOPAR_NETRMTHWAR  0x0505 // g 8  Remote hardware address (receive)
#define IOPAR_NETRMTNETAS 0x0506 // gs4  Remote network address (send)
#define IOPAR_NETRMTNETAR 0x0507 // g 4  Remote network address (receive)
#define IOPAR_NETRMTPORTS 0x0508 // gs2  Remote port number (send)
#define IOPAR_NETRMTPORTR 0x0509 // g 2  Remote port number (receive)
#define IOPAR_NETDSTNAME  0x050A // gsS  Destination name
#define IOPAR_NETSMODE    0x050B // gs4  Set network mode bits
#define IOPAR_NETCMODE    0x050C // gs4  Clear network mode bits
#define IOPAR_NETRCVWIN   0x050D // gs4  Receive window size
#define IOPAR_NETLCLNETA  0x050E // g 4  Local network address
#define IOPAR_NETKATIME   0x050F // gs1  Keep-alive time interval (seconds)
#define IOPAR_NETCONLIMIT 0x0510 // gs4  Incoming connection queue limit
#define IOPAR_NETCONHNDL  0x0511 // gs4  Incoming connection handle
#define IOPAR_NETPKTMAX   0x0512 // gs4  Maximum packet size
#define IOPAR_NETLCLHWA   0x0513 // gs8  Local hardware address

// Define IPM IO parameters

#define IOPAR_IPMBASE     0x0600
#define IOPAR_IPMRMTPID   0x0601 // g 4  IPM remote PID

// Define message IO parameters

#define IOPAR_MSGBASE     0x0700
#define IOPAR_MSGLCLADDR  0x0701 // g S  Message local address
#define IOPAR_MSGRMTADDRS 0x0702 // gsS  Message remote address (send)
#define IOPAR_MSGRMTADDRR 0x0703 // g S  Message remote address (receive)

// Define RUN function IO parameters

#define IOPAR_RUNBASE     0x1000
#define IOPAR_RUNCMDTAIL  0x1001 //  sS  Command tail (argument list)
#define IOPAR_RUNDEVLIST  0x1002 //  sS  Device list
#define IOPAR_RUNENVLIST  0x1003 //  sS  Additional environment data
#define IOPAR_RUNDEBUGBFR 0x1004 // g S  Buffer for debug data
#define IOPAR_RUNADDRESS  0x1005 // gs8  Load address
#define IOPAR_RUNRELOCVAL 0x1006 // gs4  Relocation value
#define IOPAR_RUNFCB1     0x1007 //  sS  First DOS FCB
#define IOPAR_RUNFCB2     0x1008 //  sS  Second DOS FCB
#define IOPAR_RUNACTPRIV  0x1009 //  sS  Active privileges for child
#define IOPAR_RUNAVLPRIV  0x100A //  sS  Available privileges for child

#define IOPAR_RUNWSLIMIT  0x100C //  s4  Working set size limit for child
#define IOPAR_RUNWSALLOW  0x100D //  s4  Working set size allowed for child
#define IOPAR_RUNTMLIMIT  0x100E //  s4  Total user memory limit for child
#define IOPAR_RUNTMALLOW  0x100F //  s4  Total user memory allowed for child
#define IOPAR_RUNPMLIMIT  0x1010 //  s4  Protected mode memory limit for child
#define IOPAR_RUNPMALLOW  0x1011 //  s4  Protected mode memory allowed for child
#define IOPAR_RUNRMLIMIT  0x1012 //  s4  Real mode memory limit for child
#define IOPAR_RUNRMALLOW  0x1013 //  s4  real mode memory allowed for child
#define IOPAR_RUNOMLIMIT  0x1014 //  s4  Overhead memory limit for child
#define IOPAR_RUNOMALLOW  0x1015 //  s4  Overhead memory allowed for child

// Define SPAWN function IO parameters

#define IOPAR_SPNBASE     0x1100
#define IOPAR_SPNMEMLIST  0x1101 //  sS  Memory transfer list  

#define IOPAR_CLASS       0x8000 // gs4  Device class

// Define bits in the IOPAR_CLRBUFR value

#define CB_OUTPUT 	0x40	// Clear output buffer
#define CB_INPUT  	0x20	// Clear current input buffer
#define CB_AHEAD  	0x10	// Clear type-ahead buffer

// Define bits in the IOPAR_FILOPTN value

#define XFO_PHYDEV   0x00800000	// Return physical device name even if rooted
								//   logical
#define XFO_DOSDEV   0x00400000	// Return DOS disk name
#define XFO_VOLUME   0x00200000	// Return disk volume name 
#define XFO_XOSDEV   0x00100000	// Return XOS device name
#define XFO_NODENUM  0x00080000	// Return node number
#define XFO_NODENAME 0x00040000	// Return network node name
#define XFO_NODEPORT 0x00020000	// Return node port number
#define XFO_RPHYDEV  0x00008000	// Return remote physical device name even if
								//   rooted
#define XFO_RDOSDEV  0x00004000	// Return remote DOS disk name
#define XFO_RVOLUME  0x00002000	// Return remote disk volume name
#define XFO_RXOSDEV  0x00001000	// Return remote XOS device name
#define XFO_PATH     0x00000800	// Return file path
#define XFO_FILE     0x00000200	// Return file name (includes extension)
#define XFO_VERSION  0x00000080	// Return file version number

// Define bits for the IOPAR_DEVSTS device parameter value (these bits are stored
//   in dcb_dsp) 

#define DS_RANDOM  0x02000000	// Random access device
#define DS_NONSTD  0x01000000	// Device uses non-standard API
#define DS_PHYS    0x00800000	// Physical device
#define DS_REMOVE  0x00400000	// Removeable media device
#define DS_CASESEN 0x00200000	// File names are case sensitive
#define DS_ALIAS   0x00100000	// File structure supports DOS 8x3 file name
								//   aliases
#define DS_NAMEEXT 0x00080000	// File structured device uses name.ext format
#define DS_LCLDISK 0x00040000	// Device is a local disk
#define DS_SPOOL   0x00020000	// Device is spooled
#define DS_CONTROL 0x00010000	// Device is controlling terminal for session
#define DS_NOABORT 0x00008000	// Device cannot be aborted
#define DS_UNBFRD  0x00004000	// Device should be unbuffered
#define DS_DUPLEX  0x00002000	// Device is full duplex (simultanious input
								//   and output)
#define DS_MAPPED  0x00001000	// Memory mapped device
#define DS_MLTUSER 0x00000800	// Multi-user device (any process can open
								//   device, even if device is in use)
#define DS_REMOTE  0x00000400	// Device is remote
#define DS_FILE    0x00000200	// Device is file structured
#define DS_SODIR   0x00000100	// Device supports search open directory
								//   operation
#define DS_MOUT    0x00000080	// Device can do mapped output
#define DS_MIN     0x00000040	// Device can do mapped input
#define DS_DOUTDFT 0x00000020	// Direct output is default
#define DS_DINDFT  0x00000010	// DIrect input is default
#define DS_DOUT    0x00000008	// Device can do direct output
#define DS_DIN     0x00000004	// Device can do direct input
#define DS_QOUT    0x00000002	// Device can do queued output
#define DS_QIN     0x00000001	// Device can do queued input

// Define bits for the IOPAR_USTS device parameter value

#define US_CHNGD   0x8000	// Disk media has been changed
#define US_MOUNT   0x4000	// Disk is mounted
#define US_VALID   0x2000	// Disk contains valid data
#define US_NOTF    0x1000	// Disk is not file structured
#define US_MEDIA   0x0800	// Media type is specified
#define US_RECAL   0x0400	// Need to recalabrate
#define US_HFTRK   0x0200	// Have 48 tpi disk in 96 tpi drive
#define US_MOTON   0x0100	// Motor is on (floppy disk only)
#define US_TKDEN   0x00C0	// Track density
#define US_DBLS    0x0020	// Disk is double sided (floppy only)
#define US_M8H     0x0020	// Disk has more than 8 heads (hard disk only)
#define US_DEN     0x0018	// Data density
#define   DN_SD      0		//   Single density (also all hard
							//     disks)
#define	  DN_DD      1		//   Double density
#define	  DN_HD      2		//   High density
#define US_RSIZE   0x0007	// Record size
#define	  RS_UNKWN   0		//   Unknown
#define   RS_128     1		//   128 byte records
#define   RS_256     2		//   256 byte records
#define   RS_512     3		//   512 byte records
#define   RS_1024    4		//   1024 byte records

// Define functions for QFNC_DEVCHAR and QFNC_CLASSFUNC (stored in qab_option)


// Define functions for QFNC_CLASSFUNC (stored in qab_option)

#define CF_SIZE    1		// Return sizes of complete characteristics array
#define CF_ALL     3		// Return name and type of all characteristics
#define CF_ONE     5		// Obtain type of single characteristic
#define CF_VALUES  7		// Get or set characteristics values
							// Functions beyond this point only apply to
							//   QFNC_CLASSFUNC
#define CF_ADDUNIT 8		// Add unit
#define CF_PUNITS  9		// Get names of physical units
#define CF_AUNITS  10		// Get names of active units
#define CF_DEVSRCH 11		// Search for device given name

// Define command bits for qab_option for svcIoRun

#define XR_SAMEPROC  0x80000000	// Use same process
#define XR_CHILDTERM 0x40000000	// Function is not complete until child process
								//   terminates
#define XR_SESSION   0x04000000	// Create new session for child process
#define XR_DEBUG     0x02000000	// Do debug load of program into same process
#define XR_CPYENV    0x00800000	// Copy current enviroment to new process
#define XR_ACSENV    0x00400000	// Allow new process to access this process's
								//   enviroment
#define XR_CHGENV    0x00200000	// Allow new process to change this process's
								//   enviroment
#define XR_ALLDEV    0x00080000	// Pass all devices to new process (device list
								//   is not used)
#define XR_CPYPTH    0x00040000	// Copy default paths to new process
#define XR_CHGPTH    0x00020000	// Allow new process to change this process's
								//   default paths
#define R_DOSEXEC   0x00000001	// DOS EXEC function (XOS internal use only)

// Define offsets in the argument data msect

#define arg_filofs 	0	// File specification offset
#define arg_fillen 	4	// File specification length
#define arg_cmdofs 	8	// Command data offset
#define arg_cmdlen 	12	// Command data length
#define arg_filspec	16	// File specification

// Define format of the kernel's exported symbol table

typedef struct exporttable_ EXPORTTABLE;
_Packed struct exporttable_
{	EXPORTTABLE *next;
	char         prefix[8];
	long         num;
	uchar        entry[1];
};
extern EXPORTTABLE knlExportTable[];

// Define spawn function argument block structure (SAB)

typedef _Packed struct sab_
{
    ushort    sab_func;		// Function
    ushort    sab_status;	// Returned status
    long      sab_error;	// Returned error code
    long      sab_pid;		// Returned PID
    uchar     sab_type;		// Returned process type
	uchar     sab_rsvd1[3];	// Reserved, must be 0
    ushort    sab_vector;	// Vector number
    ushort    sab_rsvd2;	// Reserved, must be 0
    ulong     sab_option;	// Options bits
    char far *sab_name;		// Name for new process
    long      sab_EIP;		// Initial EIP value for new process
    long      sab_CS;		// Initial CS value for new process
    long      sab_EFR;		// Initial EFR value for new process
    long      sab_ESP;		// Initial ESP value for new process
    long      sab_SS;		// Initial SS value for new process
    long      sab_EAX;		// Initial EAX value for new process
    long      sab_EDI;		// Initial EDI value for new process
    void far *sab_parm;		// Address of parameter list
    short     sab_parmx;
} SAB;

// Define values for sab_func

#define SFNC_WAIT  	0x8000	// Wait until function is complete
#define SFNC_CHILD 	1    	// Create child process

// Define values for sab_option

#define S_SESSION	0x04000000	// Create new session for child process
#define S_ALLDEV 	0x00080000	// Pass all devices to new process (device list
								//   is ignored if specified)
#define S_SETUP  	0x00000200	// Place child process in set up mode
#define S_NOCDS  	0x00000100	// Child process should not produce child died
								//   signal
#define S_EVENT  	0x000000FF	// Event number for termination (used if either
								//   S_SETUP or S_NOCDS is set)

// Define argument bits for svcIoCancel

#define CAN_WAIT  	0x0100	// Wait until finished
#define CAN_ALLDEV	0x0080	// Cancel all requests for all devices
#define CAN_ALL   	0x0040	// Cancel all requests for handle
#define CAN_AFTER 	0x0020	// Cancel specified request and all following requests
#define CAN_NOINT 	0x0010	// Suppress IO done interrupts
#define CAN_OPEN  	0x0004	// Cancel open request
#define CAN_OUTPUT	0x0002	// Cancel output queue requests
#define CAN_INPUT 	0x0001	// Cancel input queue requests

// Define function bits for svcIoTransName

#define TNB_NOFINALBS	0x20	// Suppress final \ in directory names
#define TNB_SEMICOLON	0x10	// Use semi-colon instead of comma to seperate names
#define TNB_FLAG     	0x08	// Insert flag character before device names
#define TNB_VOLNAME  	0x04	// Return volume name if available
#define TNB_DOSNAME  	0x02	// Return DOS disk name if available
#define TNB_ALLDEF   	0x01	// Return all definitions

// Define protection set values

#define FP_NETWORK	0x01000000	// (N) Network users
#define FP_WORLD  	0x00010000	// (W) Local users not part of the owner's group (world)
#define FP_GROUP  	0x00000100	// (G) Members of the owner's group
#define FP_OWNER  	0x00000001	// (O) Owner of the file

// Define file protection bit values

#define	FP_EXEC  	0x10		// (X) File is executable
#define	FP_READ  	0x08		// (R) File is readable
#define	FP_EXTEND	0x04		// (E) File can be extended
#define	FP_WRITE 	0x02		// (W) File is writable
#define	FP_MODIFY	0x01		// (M) File attributes can be modified

// Define directory protection bit values

#define FP_ACCESS	0x10		// (A) Files in directory can be accessed
#define FP_SEARCH	0x08		// (S) Directory can be searched
#define FP_CREATE	0x02		// (C) Files can be created in directory
#define FP_MODIFY	0x01		// (M) Directory attributes can be modified

// Define function values for the svcMemDescSet call

#define SDF_BASE  	1		// Set segment base linear address
#define SDF_LIMIT 	2		// Set segment limit
#define SDF_ACCESS	3		// Set access bits

// Define standard message types (this value is stored in the first byte of a
//   standard system message)

// Values 1 - 15 are reserved for general response messages

#define MT_INTRMDMSG     1	// Intermediate normal response
#define STR_MT_INTRMDMSG "\x01"
#define MT_INTRMDWRN     2	// Intermediate warning response
#define STR_MT_INTRMDWRN "\x02"
#define MT_INTRMDERR     3	// Intermediate error response
#define STR_MT_INTRMDERR "\x03"
#define MT_INTRMDSTS     4	// Intermediate status (temp) response
#define STR_MT_INTRMDSTS "\x04"
#define MT_FINALMSG      5	// Final normal response
#define STR_MT_FINALMSG  "\x05"
#define MT_FINALWRN      6	// Final warning response
#define STR_MT_FINALWRN  "\x06"
#define MT_FINALERR      7	// Final error response
#define STR_MT_FINALERR  "\x07"

// Values 16 - 31 are reserved for requests to INIT and LOGGER

#define MT_SYMBREQ  	16	// Request to run symbiont (INIT)
#define STR_MT_SYMBREQ "\x10"
#define MT_TERMREQ  	17	// Request to change terminal state (INIT)
#define MT_SYSLOG   	18	// Data for system log (LOGGER)
#define STR_MT_SYSLOG "\x12"
#define MT_USERFILE 	19
#define MT_TERMDATA 	20	// Data from idle terminal (INIT)
#define MT_SESNREQ      21	// Request to create session on a terminal

// Values 32 - 47 are reserved for requests to general servers

#define MT_SRVCMD   	32
#define STR_MT_SRVCMD   "\x20"
#define MT_SRVCMDPRG   	32	    // Responses are sent with initial 4-byte binary 
#define STR_MT_SRVCMDPRG "\x21" //   error/status code



// Values 48 - 63 are reserved for requests to the screen symbiont

#define MT_SSSTATUS 	48	// Screen status report
#define MT_EGACLEAR 	49	// EGA screen clear
#define MT_EGADUMP  	50	// EGA screen dump to printer
#define MT_SSREGPRG     51	// Register program to handle screen switching
#define MT_SSACTION     52	// Action request from program
#define MT_SSREPORT     53	// Report to program

#define MT_GSREQUEST    60	// Graphics set up request/response
#define STR_MT_GSREQUEST "\x3C"
#define MT_SSALMCHAR	63	// Alarm window character

// Values 64 - 79 are reserved for requests to the error logging symbiont

// Values 80 - 87 are reserved for requests to unspool symbionts

#define MT_UNSPLREADY	80	// File is ready to unspool
#define MT_UNSPLCMD  	81	// Command

// Values 88 - 95 are reserved for communications related requests

#define MT_NETLINK  88		// Network link has changed state

// Values 96 - 99 are reserved for accounting functions

#define MT_ACT		96		// System accounting message
#define MT_UDF		97      // UDF access requests (to or from USERSRV)
#define MT_RELOGREQ	98		// Relogin request
#define MT_CRDTCARD	99		// Credit card requests (to or from CRDTSRV)

// Values 100 - 127 are reserved for additional system functions

// Values 128-255 are available for user defined functions

// Define command bits for qab_command for open type functions (note: The high
//   order 13 bits describe open actions, they are not saved once the device is
//   open.  The low order 19 bits are saved and describe how the device is used
//   while open.)

// The following 13 bits are not stored in the handle table - they are only used
//   when the device is initially opened

#define XO_REPEAT   0x80000000	// Repeated operation
#define XO_REQFILE  0x40000000	// Require file structured device
#define XO_NOMOUNT  0x20000000	// Do not mount device if not mounted (ignored
								//   if O_RAW or O$PHYS not also set)
#define XO_ODFS     0x10000000	// Open directory for search
#define XO_FAILEX   0x08000000	// Fail if file exists (has no efect if file
								//   does not exist)
#define XO_CREATE   0x04000000	// Create new file if file does not exist (has
								//   no effect if file exists)
#define XO_TRUNCA   0x02000000	// Truncate existing file allocated length (has no effect
#define XO_TRUNCW   0x01000000	// Truncate existing file written length (has
								//   no effect does not exist)
#define XO_APPEND   0x00800000	// Append to file
#define XO_FHANDLE  0x00400000	// Force handle (uses value of qab_handle to
								//   specify handle, fails if handle in use)
#define XO_UNQNAME  0x00200000	// Create unique name for file

// The following 2 bits are used internally by XOS and are stored in the
//   handle table

#define XO_NOQUE    0x00040000	// IO queueing is disabled
#define XO_OPNCLS   0x00020000	// Open or close is in progress (close if
								//   O_NOQUE is also set)

// The following bits are stored in the handle table and effect the operation
//   of a device for as long as it is open

#define XO_NORDAH   0x00010000	// Do not read ahead (do single sector
								//   transfers)
#define XO_NODFWR   0x00008000	// Do not defer writes
#define XO_CONTIG   0x00004000	// File allocation must be contiguous
#define XO_CRIT     0x00002000	// Do critical error processing
#define XO_FAPPEND  0x00001000	// Force append
#define XO_NOHOLD   0x00000800
#define XO_PHYS     0x00000400	// Physical IO (exact meaning depends on device!)
#define XO_RAW      0x00000200	// Raw IO (exact meaning depends on device!)
#define XO_FNR      0x00000100	// Fail if device is not ready
#define XO_PARTIAL  0x00000080	// Accept partial input
#define XO_NOINH    0x00000040	// Device cannot be passed to child
#define XO_EXWRITE  0x00000020	// Exclusive write access
#define XO_EXREAD   0x00000010	// Exclusive read access

#define XO_OUT      0x00000002	// Output is allowed
#define XO_IN       0x00000001	// Input is allowed

// Define file attribute bits

#define XA_FILE   0x80			// Normal file
#define XA_ARCH   0x20			// Archive bit (set if file has been modified)
#define XA_DIRECT 0x10			// Directory
#define XA_LABEL  0x08			// Volume label
#define XA_SYSTEM 0x04			// System file
#define XA_HIDDEN 0x02			// Hidden file
#define XA_RDONLY 0x01			// Read only file

// Define option bits for the close SVC

#define XC_RESET   	0x80000000	// Reset file/device/connection
#define XC_KILL    	0x40000000	// Kill file/device/connection
#define XC_ONCEONLY	0x00008000	// Final once-only close
#define XV_NOHOLD   0x00000800
#define XC_DELETE  	0x00000008	// Delete file being closed
#define XC_NODEAL  	0x00000001	// Do not deallocate extra space in file

// Define bits for the svcSysFindEnv and svcSysDefEnv system calls

#define FES_SYSTEM 	0x0C000		// Search system level
#define FES_SESSION	0x08000		// Specify search level relative to session
#define FES_PROCESS	0x04000		// Specify search level relative to process

// Define bits for the svcIoFndLog and svcIoDefLog system calls

#define FLN_SYSTEM 	0x0C000		// Search system level
#define FLN_SESSION	0x08000		// Specify search level relative to session
#define FLN_PROCESS	0x04000		// Specify search level relative to process

#define TLN_SUBST  	0x40000000	// Substituted name
#define TLN_ROOTED 	0x20000000	// Rooted name

// Define structures used for managing critical regions

typedef struct
{	long label;
	char bits;
	char xxx[3];
	long head;
	long tail;
} CRIT;

// Define bits for CRIT.bits

#define CRIT_BUSY 	0x01		// Critical region is busy

XOSCALL svcSchCritDone(CRIT *crit);
XOSCALL svcSchCritWait(CRIT *crit);

// Define the inline function that is used to begin a critical region

extern void critBegin(CRIT *);
#ifndef _MSC_VER
#pragma aux critBegin =				\
	"lock bts dword ptr 4[eax], 0"	\
	"jc       short wt"				\
	"cmp      dword ptr 8[eax], 0"	\
	"je       short cr"				\
"wt: push     eax"					\
	"call     svcSchCritWait"		\
"cr:"								\
	parm [eax] modify [eax edx ecx];
#endif

// Define the inline function that is used to end a critical region

extern void critEnd(CRIT *);
#ifndef _MSC_VER
#pragma aux critEnd =				\
	"lock and dword ptr 4[eax], 0xFFFFFFFE"	\
	"cmp      dword ptr 8[eax], 0"	\
	"je       short dn"				\
	"push     eax"					\
	"call     svcSchCritDone"		\
"dn:"								\
	parm [eax] modify [eax edx ecx];
#endif

// Define structure for the TID value

typedef _Packed struct piddata
{	short number;
    short code;
} pid_data;

typedef struct
{	ushort sync;
	ushort length;
	char   str[1];
} ENVSTR;

#define ENVSYNC  0xA33A
#define LOGSYNC  0xA44A
#define PATHSYNC 0xA55A

// Define structure for the header for the HUM (High User Memory) Area -
//   These locations are guarenteed to not change with new versions of XOS.
//   Additions location should be referenced by their imported names.

typedef struct
{	char   *fileaddr;			// Address of program's file specification
	long    filelen;			// Length of program's file specification
	char   *cmdaddr;			// Address of program's argument string
	long    cmdlen;				// Length of program's argument string
	ENVSTR *envaddr;			// Address of start of the environment area
	long    logaddr;			// Address of start of the logical name area
	long    pathaddr;			// Address of start of the path area
	char   *avladdr;			// Address of first available byte
	char   *topaddr;			// First address not allocated
} HUMA;

extern void *humPTResvd;		// Start of the reserved area
extern long  humPTTid;			// TID
extern long  humPTLtn;			// Local thread number
extern long  humPTErrNo;		// errno

extern HUMA  humArea;
extern char *humData;


typedef _Packed struct imgdata
{	char *imgspec;
    long  imgspecsz;
    char *cmdtail;
    long  cmdtailsz;
} img_data;

// Define structure for parameter list entry for 0 byte value

typedef _Packed struct
{	uchar  desp;
    uchar  size;
    ushort index;
} BYTE0PARM;

// Define structure for parameter list entry for 1 byte value

typedef _Packed struct
{	uchar  desp;
    uchar  size;
    ushort index;
    char   value;
} BYTE1PARM;

// Define structure for parameter list entry for 2 byte value

typedef _Packed struct
{	uchar  desp;
    uchar  size;
    ushort index;
    short  value;
} BYTE2PARM;

// Define structure for parameter list entry for 4 byte value

typedef _Packed struct
{	uchar  desp;
    uchar  size;
    ushort index;
    long   value;
} BYTE4PARM;

// Define structure for parameter list entry for 8 byte value

typedef _Packed struct
{	uchar  desp;
    uchar  size;
    ushort index;
    llong  value;
} BYTE8PARM;

// Define structure for parameter list entry for 8 byte time value

typedef _Packed struct
{	uchar  desp;
    uchar  size;
    ushort index;
    time_s value;
} TIME8PARM;

// Define structure for parameter list entry for 2 byte text value

typedef _Packed struct
{	uchar  desp;
    uchar  size;
    ushort index;
    char   value[2];
} TEXT2PARM;

// Define structure for parameter list entry for 4 byte text value

typedef _Packed struct
{	uchar  desp;
    uchar  size;
    ushort index;
    char   value[4];
} TEXT4PARM;

// Define structure for parameter list entry for 8 byte text value

typedef _Packed struct
{	uchar  desp;
    uchar  size;
    ushort index;
    char   value[8];
} TEXT8PARM;

// Define structure for parameter list entry for 16 byte value

typedef _Packed struct
{	uchar  desp;
    uchar  size;
    ushort index;
    char   value[16];
} BYTE16PARM;

// Define structure for parameter list entry for long string value

typedef _Packed struct
{
    uchar  desp;
    uchar  size;
    ushort index;
    char  *buffer;
    ushort bfrlen;
    ushort strlen;
} LNGSTRPARM;

// Define structure for characteristics list entry for 1 byte value

typedef _Packed struct
{	uchar desp;
    uchar size;
    char  name[8];
    char  value;
} BYTE1CHAR;

// Define structure for characteristics list entry for 2 byte value

typedef _Packed struct
{	uchar desp;
    uchar size;
    char  name[8];
    short value;
} BYTE2CHAR;

// Define structure for characteristics list entry for 4 byte value

typedef _Packed struct
{	uchar desp;
    uchar size;
    char  name[8];
    long  value;
} BYTE4CHAR;

typedef _Packed struct
{	uchar desp;
    uchar size;
    char  name[8];
    char  value[2];
} TEXT2CHAR;

typedef _Packed struct
{	uchar desp;
    uchar size;
    char  name[8];
    char  value[4];
} TEXT4CHAR;

typedef _Packed struct
{	uchar desp;
    uchar size;
    char  name[8];
    char  value[8];
} TEXT8CHAR;

typedef _Packed struct
{	uchar desp;
    uchar size;
    char  name[8];
    char  value[16];
} TEXT16CHAR;

// Define structure for characteristics list entry for long string value

typedef _Packed struct
{	uchar  desp;
    uchar  size;
    char   name[8];
    char  *buffer;
    ushort bfrlen;
    ushort strlen;
} LNGSTRCHAR;

# include <_epilog.h>

#endif // _XOS_H_
