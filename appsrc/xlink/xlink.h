//********************************
// Parameter definitions for XLINK
//********************************
// Written by John Goltz
//********************************

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

#define CFG_XLINK_VER_MAJ   4
#define CFG_XLINK_VER_MIN   9
#define MV CFG_XLINK_VER_MAJ
#define NV CFG_XLINK_VER_MIN
#define EN 0

#define CFG_XLINK_MULTIPASS 0	// Auto-multipass link mode if non-0
#define CFG_XLINK_DEBUG     0	// Additional debug output if non-0

#define SYMMAXSZ  256			// Maximum length for symbol name
#define POLSSIZE  30			// Size of the polish expression stack
#define MAPPGSZ   50			// Page length (excluding header) for map
#define MSGRPMAX  1500			// Maximum number of MS segments in an MS
								//   group
#define HASHSIZE  512			// Size of the symbol table hash table
#define HASHMASK  0x1FF

#define SEGNUMBASE   ((char *)0x10000000)
#define MSECTNUMBASE ((char *)0x10040000)
#define PSECTNUMBASE ((char *)0x10080000)
#define LNAMESBASE   ((char *)0x100C0000)
#define SYMNUMBASE   ((char *)0x20000000)
#define OUTBUFBASE   ((char *)0x30000000)
char *OUTBUFBASE_;

#define SHDRSIZE 12				// Size of segment header in RUN file
#define MHDRSIZE 36				// Size of msect header in RUN file

#define offsetof(strt, label) (int)&(((strt *)0)->label)

// Define processor types

#define P_LOW   0x00
#define P_Z80   0x01
#define P_8086  0x02
#define P_80186 0x03
#define P_80286 0x04
#define P_80386 0x05
#define P_80486 0x06
#define P_80586 0x07
#define P_HIGH  0x80
#define P_68000 0x81

// Define image types

#define I_ALONE   0x01
#define I_XOSUSER 0x02
#define I_MSDOS   0x03
#define I_XOS8086 0x06
#define I_XOSLKE  0x07

// Define segment flag bits for OBJ file segment definition section

#define SF1_LARGE  0x80			// Large segment
#define SF1_32BIT  0x40			// 32 bit segment
#define SF1_CONF   0x20			// Conformable segment
#define SF1_READ   0x10			// Readable segment
#define SF1_WRITE  0x08			// Writable segment

#define SF2_EXTEND 0x80			// Extenable segment
#define SF2_LINKED 0x10			// Segment is linked to another segment
#define SF2_ADDR   0x08			// Address specified

// Define msect flag bits for OBJ file msect definition section

#define MF1_CONF   0x20			// Conformable msect
#define MF1_READ   0x10			// Readable msect
#define MF1_WRITE  0x08			// Writable msect

#define MF2_ADDR   0x08			// Address specified

// Define psect flag bits for OBJ file psect definition section

#define PF1_OVER   0x40			// Overlayed psect

#define PF3_ADDR   0x08			// Address specified

typedef struct _sy     SY;
typedef struct _sd     SD;
typedef struct _md     MD;
typedef struct _pd     PD;
typedef struct _rd     RD;
typedef struct _modblk MB;
typedef struct _objblk OB;
typedef struct _msgb   MSGB;

// Define structure for module list blocks

struct _modblk
{
	long   mb_magic;			// "*MOD"
	MB    *mb_next;				// Address of next module block
	OB    *mb_obj;				// Address of OB for file cotaining module
	char  *mb_name;				// Name of module
    long   mb_mod;				// Offset in OBJ file for start of module
    SY   **mb_symnumtbl;		// Offset of base of symbol number table
    long   mb_symnummax;		// Maximum symbol number for module
    char **mb_lnamestbl;		// Offset of base of lnames table
    long   mb_lnamesmax;
    SD   **mb_segnumtbl;		// Offset of base of segment number table
    long   mb_segnummax;		// Maximum segment number for module
    MD   **mb_msectnumtbl;		// Offset of base of msect number table
    long   mb_msectnummax;		// Maximum msect number for module
    PD   **mb_psectnumtbl;		// Offset of base of psect number table
    long   mb_psectnummax;		// Maximum psect number for module
};

// Define values for objtype

#define OT_XOS 1				// OBJ file is XOS format
#define OT_MS  2				// OBJ file is MS format

// Define structure of the OBJ file data block

struct _objblk
{	long obj_magic;
	OB  *obj_next;				// Pointer to next OBJ block
	MB  *obj_mdl;				// Pointer to first entry in module list
	char obj_fsp[1];			// File specification
};

// Define OBJ file section types.  Values of 20 or less are actual XOS format
//   section types.  Values greater than 20 are translated internal values
//   for the MS OBJ file section types.  The actual OBJ values are defined
//   with the prefix MSSC_

#define SC_EOM     0			// End of module
#define SC_INDEX   1			// Library index section
#define SC_START   2			// Start of module section
#define SC_SEG     3			// Segment definition section
#define SC_MSECT   4			// Msect definition section
#define SC_PSECT   5			// Psect definition section
#define SC_DATA    6			// Data section
#define SC_INTERN  7			// Internal symbol definition section
#define SC_LOCAL   8			// Local symbol definition section
#define SC_EXPORT  9			// Exported symbol definition section
#define SC_STRADR  10			// Starting address section
#define SC_DEBUG   11			// Debugger address section
#define SC_STACK   12			// Initial stack address section
#define SC_FILEREQ 13			// File request section
#define SC_EXTERN  14			// External symbol definition section
#define SC_SHARED  15			// Shared library definition section
#define SC_IMPORT  16			// Imported symbol definition section
#define SC_ENTRY   20			// Entry list section (obsolete)
#define SC_XOSMAX  20			// Maximum XOS section type

#define SC_BAD     21			// Illegal MS section type
#define SC_MTHEADR 22			// (0x80) Module header
#define SC_MCOMENT 23			// (0x88) Comment
#define SC_MMODEND 24			// (0x8A) Module end
#define SC_MEXTDEF 25			// (0x8C) External definition
#define SC_MTYPDEF 26			// (0x8E) Type definition
#define SC_MPUBDEF 27			// (0x90) Public definition
#define SC_MLOCSYM 28			// (0x92) Local symbols
#define SC_MLINNUM 29			// (0x94) Source line number
#define SC_MLNAMES 30			// (0x96) Name list
#define SC_MSEGDEF 31			// (0x98) Segment definition
#define SC_MGRPDEF 32			// (0x9A) Group definition
#define SC_MFIXUPP 33			// (0x9C) Fix up previous data image
#define SC_MLEDATA 34			// (0xA0) Logical data image
#define SC_MLIDATA 35			// (0xA2) Logical repeated (iterated) data
								//	    image
#define	SC_MCOMDEF 36			// (0xB0) Communal names definition
#define SC_MLOCEXD 37			// (0xB4) External definition visible within
								//	    module only
#define SC_MFLCEXD 38			// (0xB5) Func ext definition visible within
								//	    module only
#define SC_MLOCPUB 39			// (0xB6) Public definition visible within
								//	    module only
#define	SC_MLOCCOM 40			// (0xB8) Communal name visible within module
								//	    only
#define SC_MCOMDAT 41			// (0xC2) Initialized communal data
#define SC_MAX     41

// Define values for MS format OBJ file sections

#define MSSC_MIN    0x80
#define MSSC_THEADR 0x80		// Module header
#define MSSC_COMENT 0x88		// Comment
#define MSSC_MODEND 0x8A		// 16-bit module end
#define MSSC_386END 0x8B		// 32-bit module end
#define MSSC_EXTDEF 0x8C		// External definition
#define MSSC_TYPDEF 0x8E		// Type definition
#define MSSC_PUBDEF 0x90		// 16-bit public definition
#define MSSC_PUB386 0x91		// 32-bit public definition
#define MSSC_LOCSYM 0x92		// 16-bit local symbols
#define MSSC_LOC386 0x93		// 32-bit local symbols
#define MSSC_LINNUM 0x94		// 16-bit source line number
#define MSSC_LIN386 0x95		// 32-bit source line number
#define MSSC_LNAMES 0x96		// Name list
#define MSSC_SEGDEF 0x98		// 16-bit segment definition
#define MSSC_SEG386 0x99		// 32-bit segment definition
#define MSSC_GRPDEF 0x9A		// Group definition
#define MSSC_FIXUPP 0x9C		// Fix up previous 16-bit data image
#define MSSC_FIX386 0x9D		// Fix up previous 32-bit data image
#define MSSC_LEDATA 0xA0		// 16-bit logical data image
#define MSSC_LED386 0xA1		// 32-bit logical data image
#define MSSC_LIDATA 0xA2		// 16-bit logical repeated (iterated) data
								//   image
#define MSSC_LID386 0xA3		// 32-bit logical repeated (iterated) data
								//   image
#define MSSC_COMDEF 0xB0		// Communal names definition
#define MSSC_LOCEXD 0xB4		// External definition visible within module
								//   only
#define MSSC_FLCEXD 0xB5		// Func ext definition vis within module only
#define MSSC_LOCPUB 0xB6		// 16-bit public definition visible within
								//   module only
#define MSSC_LPB386 0xB7		// 32-bit public definition visible within
								//   module only
#define MSSC_LOCCOM 0xB8		// Communal name visible within module only
#define MSSC_COMDAT 0xC2		// 16-bit initialized communal data
#define MSSC_CMD386 0xC3		// 32-bit initialized communal data

//@@@ CEY added C4,C5
#define MSSC_LINSYM16 0xC4
#define MSSC_LINSYM32 0xC5

#define MSSC_MAX    0xC5

// Define structure of symbol table entry

struct _sy
{	SY   *sy_next;				// Link to next symbol for same psect
	SY   *sy_sort;				// Pointer used by heapsort
	long  sy_magic;
	SY   *sy_hash;				// Hash list link
	SY   *sy_export;			// Pointer to next exported symbol
	SY   *sy_weak;				// Pointer to weak or lazy symbol defaut symbol
	PD   *sy_psd;				// Pointer to psect data block for symbol
	MB   *sy_mod;				// Pointer to MB if symbol is local to a
								//   single module, NULL if global
	uint  sy_flag;				// Flag bits
	union						// Value or pointer to segment, msect, or
	{	long  v;				//   psect data block
		SD   *s;
		MD   *m;
		PD   *p;
		MSGB *g;
	}     sy_val;
	short sy_sel;				// Selector value
	char  sy_name[4];			// Symbol name
};

// Define bits for sy_flag

#define SYF_LOCAL  0x8000		// Local symbol
#define SYF_LAZY   0x4000		// Symbol is lazy
#define SYF_USED   0x2000		// Symbol has been used for relocation
#define SYF_EXPORT 0x1000		// Exported symbol
#define SYF_IMPORT 0x0800		// Imported symbol
#define SYF_INSYMT 0x0400		// Symbol has been placed in XDT symbol table
#define SYF_MULT   0x0200		// Multipily defined symbol
#define SYF_UNDEF  0x0100		// Undefined symbol

// Following 4 bits exactly match corresponding bits in the OBJ file

#define SYF_ABS    0x0080		// Absolute value
#define SYF_ENTRY  0x0040		// Entry symbol
#define SYF_ADDR   0x0020		// Address
#define SYF_SUP    0x0010		// Suppressed

// Define values for the debugger symbol table flag byte (those marked
//   with * match corresponding OBJ file bits

#define DBF_SEG  0x80			//   Segment selector value
#define DBF_IMP  0x40			//   Imported symbol
#define DBF_ADR  0x20			// * Address
#define DBF_SUP  0x10			// * Suppressed
#define DBF_INT  0x08			//   Internal symbol
#define DBF_MSC  0x04			//   Msect selector/offset offset value
#define DBF_MOD  0x02			//   Module name
#define DBF_REL  0x01			//   Symbol is relocatable

// Define values for first byte of symbuf

#define SYM_SYM   1				// Symbol
#define SYM_SEG   2				// Segment name
#define SYM_MSCT  3				// Msect name
#define SYM_PSCT  4				// Psect name
#define SYM_MSGRP 5				// MS group name

// Define structure for polish stack items

typedef struct
{   int  ps_type;				// Item type
    long ps_value;				// Item offset value
    long ps_select;				// Item selector value
    union						// Pointer to msect or segment data block
    {   SD *s;
        MD *m;
        SY *i;					//   Imported symbol
    }    ps_pnt;
} PSTK;

// Define values for ps_type

#define PT_ABSOLUTE  1			// Absolute value
#define PT_RELATIVE  2			// Relative value
#define PT_SELMS     3			// Selector (msect)
#define PT_ABSOFSMS  4			// Absolute offset (msect)
#define PT_RELOFSMS  5			// Relative offset (msect)
#define PT_ADDRMS    6			// Address (msect)
#define PT_SELSYM    7			// Selector (imported symbol)
#define PT_ABSOFSSYM 8			// Absolute offset (imported symbol)
#define PT_RELOFSSYM 9			// Relative offset (imported symbol)
#define PT_ADDRSYM   10			// Address (imported symbol)

// Define structure for relocation data block

struct _rd
{	RD   *rd_next;				// Pointer to next relocaton data block for
								//   msect
	RD   *rd_sort;				// Used by heapsort
	long  rd_magic;				// For debugging
    ulong rd_loc;				// Location in msect where relocation is to
								//   be applied
    union
    {   long n;
        SY  *s;
    } rd_data;
    uchar rd_type;				// Relocation type
};

// Define values for rd_type
								// Following values specify relocation type
#define RT_SELECTOR 0x00		//   Segment selector (3-bit field)
#define RT_8ABSOFS  0x40		//   8-bit absolute offset
#define RT_8RELOFS  0x50		//   8-bit relative offset
#define RT_16ABSOFS 0x60		//   16-bit absolute offset
#define RT_16RELOFS 0x70		//   16-bit relative offset
#define RT_32ABSOFS 0x80		//   32-bit absolute offset
#define RT_32RELOFS 0x90		//   32-bit relative offset
#define RT_16ADDR   0xA0		//   16-bit address
#define RT_32ADDR   0xB0		//   32-bit address

								// Following values specify relocation kind
								//   (2-bit field)
#define RT_SEGMENT  0x00		//   Segment
#define RT_MSECT    0x04		//   Msect
#define RT_SYMBOL   0x08		//   Imported symbol
#define RT_NONE     0x0C		//   None (type must be RT_xxRELOFS)
								// Low 3 bits specify length of delta offset
								//   field

// Define structure for the segment data block

struct _sd
{	long  sd_magic;
	SD   *sd_next;				// Pointer to next segment data block
    SY   *sd_sym;				// Pointer to symbol table entry for segment
    MD   *sd_head;				// Head pointer for msect list for segment
    MD   *sd_tail;				// Tail pointer for msect list for segment
    SD   *sd_linked;			// Offset of SDB for linked segment
    ulong sd_lnknum;			// Local segment number for linked segment
    int   sd_msn;				// Number of msects in segment
    int   sd_num;				// Global segment number
    uchar sd_flag;				// Flag byte
    uchar sd_type;				// Segment type
    uchar sd_priv;				// Privilege level
    uchar sd_xxx;
    int   sd_select;			// Selector for segment
    ulong sd_mod;				// Address modulus for segment
    ulong sd_addr;				// Physical address for segment
    ulong sd_offset;			// Offset in RUN file for segment
};

// Define bits for sd_flag

								// First 5 bits must match corresponding
								//   SF1 bits in the OBJ file
#define SA_LARGE  0x80			// Large segment
#define SA_32BIT  0x40			// 32 bit segment
#define SA_CONF   0x20			// Conformable segment
#define SA_READ   0x10			// Readable segment
#define SA_WRITE  0x08			// Writable segment

#define SA_EXTEND 0x04			// Extendable segment
#define SA_SHARE  0x02			// Sharable segment
#define SA_ADDR   0x01			// Physical address specified

// Define structure for the msect data block

struct _md
{	long  md_magic;
	MD   *md_next;				// Pointer to next msect data block
    SY   *md_sym;				// Pointer to symbol table entry for msect
    SD   *md_sgd;				// Pointer to segment data block
    uchar md_flag;				// Flag byte
    uchar md_type;				// Msect type
    uchar md_priv;				// Msect privledge level
    uchar md_xxx;
    int   md_psn;				// Number of psects in msect
    int   md_num;				// Global msect number
    PD   *md_head;				// Head pointer for psect list for msect
    PD   *md_tail;				// Tail pointer for psect list for msect
    ulong md_max;				// Maximum address space wanted
    ulong md_addr;				// Virtual address for msect
    long  md_mod;				// Address modulus
    ulong md_tsize;				// Total size of msect
    ulong md_lsize;				// Loaded size of msect
    ulong md_offset;			// Offset in RUN file for msect
    ulong md_rdhdros;			// Offset in RUN file where data about the
								//   relocation data is stored
    RD   *md_rdhead;			// Pointer to first relocation data block
    RD  **md_rdlast;			// Pointer to next pointer in last relocation
								//   data block
    ulong md_rdcnt;				// Number of relocation items for msect
    ulong md_rdoffset; 			// Offset in RUN file for relocation data
};

// Define values for md_type

#define   MT_DATA  0x01
#define   MT_CODE  0x02
#define   MT_STACK 0x03
#define   MT_COMB  0x04

// Define bits for md_flag

								// First 4 bits must match the corresponding
								//   MF1 bits in the OBJ file
#define MA_CONF  0x20			// Conformable msect
#define MA_READ  0x10			// Readable msect
#define MA_WRITE 0x08			// Writable msect
#define MA_SHARE 0x02			// Sharable msect

#define MA_ADDR  0x01			// Address in segment specified

// Define structure for the psect data block

struct _pd
{	long  pd_magic;
	PD   *pd_next;				// Pointer to next psect data block for msect
								//   (or same psect if this is a file PD)
    SY   *pd_sym;				// Pointer to symbol table entry for psect
///	union
///	{
	    SY *pd_symhead;			// Symbol list head pointer
		PD *pd_filehead;		// File list head pointer
///	};
///	union
///	{
	    SY *pd_symtail;			// Symbol list tail pointer
		PD *pd_filetail;		// File list tail pointer
///	};
	SY   *pd_lochead;			// Local symbol list head pointer
	SY  **pd_loclast;			// Pointer to next pointer in last local symbol
    MD   *pd_msd;				// Pointer to msect data block
	MB   *pd_modblk;			// Pointer to MD for module containing psect

	PD   *pd_filepd;			// Address of file PD
	char *pd_clsname;			// MS class name
    ulong pd_num;				// Global psect number
    uchar pd_flag;				// Psect flags
    uchar pd_xxx[3];
    ulong pd_tsize;				// Total size of psect
    ulong pd_lsize;				// Loaded size of psect
    ulong pd_qsize;				// Size of single part of MS segment
    ulong pd_addr;				// Starting (base) address for psect
    long  pd_modulus;			// Modulus for psect
    ulong pd_offset;			// Offset in RUN file for psect
    ulong pd_base;				// Relocation value for psect
    int   pd_ssmax;				// Length of longest symbol defined for psect
};

// Flag bits for pd_flag

								// First bit must match the corresponding
								//   PF1 bit in the OBJ file
#define PA_OVER    0x40			// Overlayed psect
#define PA_MS32BIT 0x10			// MS segment is a 32-bit segment
#define PA_MSCODE  0x08			// MS segment is a code segment
#define PA_MSWRITE 0x04			// MS segment is writable
#define PA_JUNK    0x02			// Discard contents of psect
#define PA_ADDR    0x01			// Address specified

// Define structure for MS group definition block

struct _msgb
{	long  msgb_magic;
	MSGB *msgb_next;			// Pointer to next MSGB
    SY   *msgb_sym;				// Pointer to symbol table entry
    int   msgb_size;			// Number of segments in this group
    PD   *msgb_segs[MSGRPMAX];	// Definition array
};


extern int     version;
extern int     editnum;

extern int     majvrsn;
extern int     minvrsn;
extern int     editnum;
extern char    vendor[64];
extern int     vendorlen;
extern int     fheadsz;

extern uchar   ptype;
extern char    itype;
extern long    offset;
extern long    rdoffset;
extern long    symtblsize;
extern long    stbtotal;
extern long    outbuftop;
extern long    outbufsize;
extern int     seggbn;
extern int     mscgbn;
extern int     pscgbn;
extern int     symsize;
extern int     mltcnt;
extern int     undefcnt;
extern long    outdd;
extern long    stradr;
extern PD     *strpsd;
extern long    stkadr;
extern PD     *stkpsd;
extern long    dbgadr;
extern PD     *dbgpsd;
extern PSTK   *polspnt;
extern PSTK    polstk[];
extern PD      abspsd;
extern PD     *curpsd;
extern MD     *curmsd;
extern SD     *cursgd;
extern PD     *stbpsd;
extern MD     *stbmsd;
extern MSGB   *msgbhead;
extern MSGB   *msgbtail;
extern PD    **psnumpnt;
extern int     psnumcnt;
extern MD    **msnumpnt;
extern int     msnumcnt;
extern SD    **segnumpnt;
extern int     segnumcnt;
extern uchar  *msbuffer;
extern int     mscount;
extern ulong   msoffset;
extern void  (*msdispatch)(void);
extern ulong   stbsize;
extern ulong   stbnum;
extern ulong   stbxsp;
extern int     seccnt;
extern int     sectype;
extern int     sectypx;
extern long    objdd;
extern long    symdd;
extern int     objcnt;
extern uchar  *objpnt;
extern int     objamnt;
extern long    objblk;
extern OB     *firstobj;
extern OB     *curobj;
extern OB    **prevobj;
extern MB     *curmod;
extern char   *firstname;
extern char   *firstext;
extern int     modulenum;
extern char   *symbufr;
extern char   *symbpnt;
extern int     symbcnt;
extern SD     *sgdhead;
extern SD     *sgdtail;
extern SY    **symnumbase;
extern SY     *hashtbl[];
extern SY     *importhead;
extern SY    **importlast;
extern long   importnum;
extern long   importnumoffset;
extern long   importos;
extern SY    *exporthead;
extern SY    *exporttail;
extern long   exportnum;
extern long   exporttotal;
extern struct _mab segnummem;
extern struct _mab msectnummem;
extern struct _mab psectnummem;
extern struct _mab symnummem;
extern struct _mab lnamesmem;
extern uchar *auxloc;
extern uchar *objloc;
extern uchar *objbuf;
extern char  *outfsp;
extern char  *mapfsp;
extern char  *symfsp;
extern FILE  *mapfp;
extern long   debgaddr;
extern int    opsymsz;
extern char   opsymwrd;
extern char   opsymord;
extern char   prgname[];
extern char   inpext[];
extern char   outext[];
extern char   symext[];
extern char   mapext[];
extern char   cmdext[];
extern char   objtype;
extern char   modtype;
extern char   needrun;
extern char   needexe;
extern char   needmap;
extern char   needsym;
extern char   lowfirst;
extern char   libflag;
extern char   mapflag;
extern char   sumflag;
extern char   debugflag;
extern char   done;
extern char   errorflg;
extern char   symblk;
extern char   modname[];
extern char   symbuf[];
extern char   haveweak;
extern char   havelazy;
extern uchar  multstr;
extern uchar  multdbg;
extern uchar  multstk;
extern uchar  checkflg;

// Function prototypes

void  absaddr(char *msg);
void  addrconfl(MD *md1, long top1, MD *md2, long top2);
void  athadd(void);
void  athand(void);
void  athcom(void);
void  athdiv(void);
void  athior(void);
void  athmul(void);
void  athneg(void);
void  athsft(void);
void  athsub(void);
void  athxor(void);
void  badcmd(void);
void  badfmt(void);
void  badrexp(void);
void  badsec(void);
void  checkcur(void);
void  chkoptn(void);
void  closeobj(void);
void  clrbuf(long *buffer, int size);
int   cmpsym(SY *a, SY *b, PD *psd);
void  confladdr(char *blk, SY *name, int curval, int newval);
void  conflatr(char *atrib, char *blk, SY *name);
void  conflblk(char *blk1, SY *name1, char *blk2, SY *name2);
void  conflitm(char *blk, char *thing, SY *name);
void  conflitsegtype(int newtype, int oldtype, SY *name);
void  confllink(SY *name);
void  conflmsseg(SY *name);
void  conflsel(SY *name);
SY   *definternal(int global, int header, PD *psd, ulong value, uint selector);
void  defcommunal(int flags, int size, int pflag, int modulus, char *symname,
		char *msname);
void  diffspace(SY *sym);
void  duppsect(SY *sym);
void  dofilesymtbl(void);
void  domemsymtbl(void);
void  dosummary(void);
void  endofsec(void);
SY   *entersym(void);
void  fail(char *msg);
void  fileone(OB *obj);
void  filetwo(OB *obj);
ulong getbyte(void);
char *getfsp(int offset, char *fsp, char *dftext);
ulong getitem(char size);
void  getmssym(void);
ulong getmsxnum(void);
ulong getnumber(void);
ulong getlong(void);
MD   *getmsd(int local);
MD   *getmsmsd(void);
ulong getmsnum(void);
PD   *getpsd(int local);
int   getsec(void);
SD   *getsgd(int local);
void  getsym(void);
ulong getword(void);
int   havearg(char *arg);
void  helpprint(char *helpstring, int state, int newline);
void  impaldef(SY *sym);
int   lid1block(void);
uchar*lid2block(uchar *pnt);
void  listgbl(int cnt, char *msg, int stst);
SY   *looklocsym(void);
SY   *looksym(void);
void  map1msect(MD *msd);
void  map1psect(PD *psd, char relflag);
void  map1seg(SD *sgd);
void  mapbegin(void);
void  mapconstr(char *str);
void  mapfin(void);
void  mapnline(void);
void  mapnpage(void);
void  mapout(void);
void  maptline(int lines);
void  moduleone(void);
void  moduletwo(void);
void  msdoledata(void);
void  msdolidata(void);
void  msundef(char *msectname, char *label, char *name);
void  multaddr(char *msg);
void  multdef(SY *sym, long value, int psn);
void  nosyms(char *msg);
void  nosymseg(void);
SY   *onesym(void);
void  opnobj(OB *objpnt);
int   optalone(void);
int   optcheck(arg_data *arg);
int   optdebug(arg_data *arg);
char *optfile(arg_data *arg, char *dftext);
void  opthelp(void);
int   optlke(void);
int   optmap(arg_data *arg);
int   optout(arg_data *arg);
int   optexe(arg_data *arg);
int   opts386(arg_data *arg);
int   opts68k(arg_data *arg);
int   opts86(arg_data *arg);
int   optsum(arg_data *arg);
int   optsym(arg_data *arg);
int   optvrsn(arg_data *arg);
int   optvendor(arg_data *arg);
void  outclose(void);
void  outname(char *pnt);
void  outopen(char *spec);
void  outsymbyte(uint chr);
void  overflow(void);
void  passone(void);
void  passtwo(void);
int   peekchar(void);
void  polexan(int header);
void  polexrn(int header);
void  polopr(int header);
void  polosam(void);
void  polpar(int header);
void  polpav(int header);
void  polpush(long value, long select, int type, void *pnt);
void  polraa(int header);
void  polrac(int header);
void  polrra(int header);
void  polssam(void);
void  polssap(void);
void  polssas(void);
void  polsscp(void);
void  polssexn(void);
void  polstraddrl(void);
void  polstraddrw(void);
void  polstrl(void);
void  polstrsb(void);
void  polstrsw(void);
void  polstrub(void);
void  polstruw(void);
void  putbyte(long data);
void  putinsym(MD *msd, SD *sgd, long value, int select, int flags);
void  putlong(long value);
void  putreld(PSTK *item, int size);
void  putvarval(long value);
void  putword(long value);
int   readblk(void);
int   resolvemod(int oldmod, int newmod);
void  resvsym(void);
void  segpush(SD *sgd);
void  setblk(long offset);
void  setobj(void);
void  showfile(int spaces);
void  showplace(SY *sym);
void  skipsp(void);
void  skpbyte(int amnt);
void  symname(void);
void  symnum(void);
void  truncate(int size);
void  underflow(void);
char *usefif(char *ext);
int   vallength(long value);
void  varwmsg(SY *sym, char *msg1, char *msg2);
void  warnaddr(MD *msd);
void  warnpsaddr(PD *psd);
void  xeline(void);
void  xnline(void);
