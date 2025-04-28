#ifndef _XOSX_H_
#define _XOSX_H_

#include <_prolog.h>

#ifndef _XOSTIME_H_
#include <xostime.h>
#endif



// Define global segment selectors (selectors marked with * are user level)

#define GS_XDATA     0x0028		//  XOS data segment
#define GS_XCODE     0x0030		//  XOS code segment

#define GS_BIOSDATA  0x0040		//* Maps DOS environment BIOS data (Some DPMI
								//    programs count on this!)
#define GS_DISPLAY   0x0048		//  Display screen buffer
#define GS_REAL16    0x0050		//* Segment which maps the real mode segment as
								//    a 16-bit data segment
#define GS_REAL32    0x0058		//* Segment which maps the real mode segment as
								//    a 32-bit data segment

#define GS_STKERRTSS 0x0060		//  Stack error handler task TSS selector
#define GS_DBLERRTSS 0x0068		//  Double exception error handler task TSS
								//    selector
#define GS_NULLTSS   0x0070		//  Null process TSS selector
#define GS_AVAIL     0x0080		//  First selector available for allocation



// Define structure for the device function dispatch table

typedef struct
{	long (*mount)(void);		// sd_mount    =  0 - Mount
	long (*cleardcb)(void);		// sd_cleardcb =  4 - Clear DCB
	long (*opena)(void);		// sd_opena    =  8 - Open additional
	long (*opena)(char *nm);	// sd_open1    = 12 - Open device/file
	long (*findfile)(char *nm);	// sd_findfile = 16 - Device parameters
	long (*delete)(char *nm);	// sd_delete   = 20 - Delete file
	long (*rename)(char *old, char *new);
								// sd_rename   = 24 - Rename file
	long (*inblk)(void);		// sd_inblock  = 28 - Input block
	long (*outblk)(void);		// sd_outblock = 32 - Output block
	long (*getsts)(void);		// sd_getiosts = 36 - Get input/output status
	long (*special)(void);		// sd_special  = 40 - Special device function
	long (*close)(void);		// sd_close    = 44 - Close file
	long (*label)(void);		// sd_label    = 48 - Get device label
	long (*commit)(void);		// sd_commit   = 52 - Commit data to disk
	long (*info)(void);			// sd_devinfo  = 56 - Get device info
	long (*vfychng)(void);		// sd_vfychn   = 60 - Verify changed disk
	long opnparms;				// sd_opnsdpt  = 64 - Open paramter table
	long comparms;				// sd_opnddpt  = 68 - Device dependent paramaters
} DEVDSPTBL;


typedef struct
{	long first;
	long fill1;
	long fill2;
	long (*reset)();
} CHARHEAD;

typedef struct charitem__ CHARITEM;
struct charitem__
{	long   label;
	long   next;
	char   desc;
	char   flags;
	ushort length;
	union
	{	char      name[8];
		CHARITEM *extrn;
	};
	long (*setfunc)();
	long (*getfunc)();
	long  *data;
	char  *info;
};

typedef struct
{	CHARHEAD head;
	CHARITEM item[1];
} CHARTBL;


#define CHARBGN(rst) {0, 0, 0, rst}

#define CHARENT(name, desc, len, info, get, set, data) \
{'RAHC', -1, REP_##desc, 0, len, #name, set, get, data, info}

#define CHAREND(name, desc, len, info, get, set, data) \
{'RAHC', 0, REP_##desc, 0, len, #name, set, get, data, info}

typedef struct
{	char len;
	char txt[];
} CHARINFO;


// Define union used for arguments to charactristics functions

typedef union
{	llong  lnum;
	long   num;
	struct
	{	char *str;
		long  len;
	};
} CHARARG;


typedef struct
{	char   type;
	char   fill;
	ushort size;
	long (*getfunc)();
	long (*setfunc)();
} PARMITEM;

typedef struct
{	long     size;
	PARMITEM parm[];
} PARMTBL;


#define PARMENT(type, size, getf, setf) \
{REP_##type, 0, size, getf, setf}



typedef struct pda__  PDA;
typedef struct tda__  TDA;

typedef struct dcb__  DCB;
typedef struct dlb__  DLB;
typedef struct ccb__  CCB;
typedef struct hdlb__ HDLB;
typedef struct slb__  SLB;




// Define structure for the common part of a DCB

struct dcb__
{	long       label;		// DCB label (DCB*)
	void     (*fdisp)(void);// Address of fork routine
	DCB       *fnext;		// Address of next DCB in fork request list or DMA
							//   wait list
	char       dmachnl;		// DMA channel number
	char       punit;		// Primary unit number
	ushort     sunit;		// Secondary unit number
	char       name[16];	// Name of device (16 characters)
	char       typename[8];	// Device type (8 characters)
	DCB       *next;		// Address of next DCB in class
	DCB       *prev;		// Address of previous DCB in class
	DEVDSPTBL *sdisp;		// Address of SVC function dispatch table
	long       opencnt;		// Number of times device has been opened
	PDA       *sespda;		// Actual address of PDA for session process which
							//   owns the device
	HDLB      *hdlb;		// Address of HDLB if device is being held
	SLB       *clsslb;		// Address of SLB for close signal
	CCB       *ccb;			// Address of CCB for device class
	long       id;			// Device ID
	long       dsp;			// Device description bits
	void      *devchar;		// Address of device characteristics table
	TDA       *inpthrd;		// Address of TDA for input IO thread (WARNING:
							//   must be enough into the DCB so clearing it
							//   after a dynamic DCB has been given up will
							//   not do any harm! ???)
	char       inpqlmt;		// Input queue limit
	char       bufrlmt;		// Buffer limit
	ushort     fill1;
	TDA       *outthrd;		// Address of TDA for output IO thread
	char       outqlmt;		// Output queue limit
	char       waitque;		// Wait queue to use
	ushort     fill2;
	char       sts1;		// First device status byte (device independent)
	char       sts2;		// Second device status byte (device independent)
	char       sts3;		// Third device status byte (device dependent)
	char       fill3;
};

// Define structure for the device list block (DLB) which is used to keep
//   track of low level device drivers

struct dlb__
{	long   label;			// Label (DLB*)
	DLB   *next;				// Address of next DLB for same class
	char   name[8];			// Dirver type name
	long (*addunit)(void);	// Address of addunit function for device
};

// Define structure for the CCB which defines a device class

struct ccb__
{	long   label;			// Label (CCB*)
	CCB   *next;			// Address of next CCB
	char   name[8];			// Name of this class (8 characters, zero filled)
	long   npfxmsk;			// Name prefix part mask
	long   npfxval;			// Name prefix part value
	long (*dchk)(char name[16], char *path);
							// Address of device check routine
	void  *cfdsp;			// Address of class function dispatch table
	void  *clschar;			// Address of class characteristics table
	DCB   *dcbhead;			// Address of first DCB for class
	DCB   *dcbtail;			// Address of last DCB for class
	void  *blkhead;			// Address of first data block for class
	void  *blktail;			// Address of last data block for class
	DLB   *dlbhead;			// Address of first DLB for class
};










long sysIoNewClass(CCB *ccb);


long sysChrFixUp(CHARTBL *tbl);

long sysChrCharValues(void);
long sysChrGetSysVal(void);
long sysChrSetSysVal(void);

long sysPrmGtDmyN(void);
long sysPrmStDmyN(void);
long sysPrmGtDmyS(void);
long sysPrmStDmyS(void);

long sysPrmGtNumOpen(void);
long sysPrmGtDevSts(void);
long sysPrmGtGlbId(void);
long sysPrmGtTimeOut(void);
long sysPrmStTimeOut(void);

long sysPrmGtDelay(void);
long sysPrmStDelay(void);
long sysPrmGtUnitNum(void);

long sysPrmGtInpQLmt(void);
long sysPrmStInpQLmt(void);
long sysPrmGtOutQLmt(void);
long sysPrmStOutQLmt(void);
long sysPrmGtBufrLmt(void);
long sysPrmStBufrLmt(void);

long sysPrmGtFilOptn(void);
long sysPrmStFilOptn(void);
long sysPrmGtFilSpec(void);
long sysPrmStFilSpec(void);

long sysPrmGtClass(void);
long sysPrmStClass(void);


extern long sysPrmNullFileParms;	// PARMITEM






# include <_epilog.h>

#endif // _XOS_H_
