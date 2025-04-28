_Packed typedef struct
{	char  label[4];
	char  hlen;
	char  fmtver;
	short fill;
	llong cdt;
	ulong totalsize;
	ulong totalitems;
	ulong crcvalue;
} XIDHEAD;

_Packed typedef struct
{	ushort sync;
	uchar  seqnum;
	uchar  flag;
	ushort namelen;
	long   filelen;
	llong  cdt;
} ITEMHEAD;

// Define values for flag

#define IF_COPY  0x0080		// Install on boot disk
#define IF_ISET  0x0040		// Add to install set
#define IF_TYPE  0x000F		// Item type

#define IT_BSTRP  0			// Bootstrap file (must be first item)
#define IT_OSISET 1			// Start of OS install set
#define IT_APISET 2			// Start of Application install set
#define IT_DTISET 3			// Start of data install set

#define IT_RTDIR  8			// Root directory
#define IT_SBDIR  9			// Sub-directory
#define IT_FILE  10			// File
#define IT_XCLD  11			// Excluded file (never present in XID or XIS file)
#define IT_END   15			// End of install set
