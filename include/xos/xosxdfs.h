
// ++++
//  This software is in the public domain.  It may be freely copied and used
//  for whatever purpose you see fit, including commerical uses.  Anyone
//  modifying this software may claim ownership of the modifications, but not
//  the complete derived code.  It would be appreciated if the authors were
//  told what this software is being used for, but this is not a requirement.

//    THIS SOFTWARE IS PROVIDED BY THE AUTHORS "AS IS" AND ANY EXPRESS OR
//    IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
//    OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
//    IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY DIRECT, INDIRECT,
//    INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
//    BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
//    OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
//    ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
//    TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
//    USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// ----

// -------------------------------------------------------------------------------
// 
//   XOSXDFS.PAR - Kernel-mode include file for DOS file system
// 
// -------------------------------------------------------------------------------

// Define structure for the DOS FAT12/FAT16 disk boot block

_Packed typedef struct
{	uchar  jump[3];		// Jump instruction to start of bootstrap code
	char   oemname[8];	// OEM name and version
	ushort secsize;		// Sector size in bytes
	uchar  secpcls;		// Sectors per cluster
	ushort ressec;		// Total number of reserved sectors
	uchar  numfats;		// Number of FATs
	ushort rdirent;		// Number of root directory entries
	ushort sectors;		// Total number of sectors on disk (16 bit value)
	uchar  media;		// Media description byte
	ushort secpfat;		// Number of sectors per FAT table
	ushort secptrk;		// Number of sectors per track
	ushort heads;		// Number of heads
	long   hidsec;		// Number of hidden sectors
	long   ttlsec;		// Total number of sectors on disk (32 bit value)
						// Following is the extended boot parameter block -
						//   This has the same contents for all formats but is
						//   in a different position for 12/16 and 32 bit FATs.
	uchar  drive;		// BIOS drive number (0 or 0x80)
	uchar  xxx;			// Reserved
	uchar  extsig;		// EBPB signature
	long   sernum;		// Disk serial number (random value)
	char   vollbl[11];	// Volume label (not used)
	char   sysid[8];	// File system ID (FAT12, FAT16, or FAT32)
						// End of the EBPB
	uchar  code[448];	// Boot code
	ushort label;		// Block label
} BOOTBLK;

_Packed typedef struct
{	uchar  jump[3];		// Jump instruction to start of bootstrap code
	char   oemname[8];	// OEM name and version
	ushort secsize;		// Sector size in bytes
	uchar  secpcls;		// Sectors per cluster
	ushort ressec;		// Total number of reserved sectors
	uchar  numfats;		// Number of FATs
	ushort rdirent;		// Number of root directory entries
	ushort sectors;		// Total number of sectors on disk (16 bit value)
	uchar  media;		// Media description byte
	ushort secpfat;		// Number of sectors per FAT table
	ushort secptrk;		// Number of sectors per track
	ushort heads;		// Number of heads
	long   hidsec;		// Number of hidden sectors
	long   ttlsec;		// Total number of sectors on disk (32 bit value)
						// Items before this point are common to all formats
	long   ttlsecpfat;	// Sectors per FAT (32 bit value)
	ushort extflags;	// Flags
	ushort version;		// File system version
	long   rootcls;		// Number of first cluster in root directory
	ushort fsinfo;		// Sector number of the file system information sector
	ushort backboot;	// Sector number of the backup boot sector (0FFFFh if
						//   no backup boot sector)
	uchar  reserved[12];// Reserved
						// Following is the extended boot parameter block -
						//   This has the same contents for all formats but is
						//   in a different position for 12/16 and 32 bit FATs.
	uchar  drive	;	// BIOS drive number (0 or 0x80)
	uchar  xxx2;		// Reserved
	uchar  extsig;		// EBPB signature
	long   sernum;		// Disk serial number (random value)
	char   vollbl[11];	// Volume label (not used)
	char   sysid[8];	// File system ID (FAT12, FAT16, or FAT32)
						// End of the EBPB
	uchar  xosdrive;	// XOS drive info
	uchar  lvl2blk;		// Block number of first reserved block used
	uchar  lvl2num;		// Number of reserved blocks used
    uchar  code[384];	// Level 1 bootstrap code
	uchar  bsversion;	// Bootstrap version number
	uchar  bseditnum;	// Bootstrap edit number
    char   errmsg[31];	// Error message text
	ushort label;		// Block label
} BOOTBLK32;

// Define bits for drive

#define BBDRV_HARD  0x80	// Disk is a hard disk
#define BBDRV_PART  0x40	// Disk is partitioned
#define BBDRV_FAT32 0x20	// Disk contains FAT32 file system
#define BBDRV_FAT16 0x10	// Disk contains FAT16 file system
#define BBDRV_UNIT  0x0F	// Unit number

// Define structure for a DOS base directory entry

_Packed typedef struct
{	char   name[8];		// Name
	char   ext[3];		// Extension
	uchar  attrib;		// Attribute byte
	uchar  lcase;		// Case for base and extension (not supported)
	uchar  ctms;		// Creation time (10 milliseconds)
	ushort ctime;		// Creation time
	ushort cdate;		// Creation date
	ushort adate;		// Access date
	ushort pntrhi;		// High 16 bits of number of first cluster
	ushort mtime;		// Modification time
	ushort mdate;		// Modification date
	ushort pntr;		// Number of first cluster (low 16 bits)
	long   size;		// File size
} DIRENT;

// Define structure for a DOS long file name directory slot entry

_Packed typedef struct
{	uchar  id;			// Sequence number for slot
	ushort name0[5];	// First 5 characters
	uchar  attrib;		// Attribute byte (always 0Fh)
	uchar  resrvd;		// Reserved (always 0)
	uchar  chksum;		// Checksum of short name
	ushort name5[6];	// Next 6 characters
	ushort pntr;		// Number of first cluster (not used, must be 0)
	ushort name11[2];	// Next 2 characters
} DIRLNG;
