//------------------------------------------------------------------------------
//
//  XOSDISK.H - User-mode disk definitions.
//
//  Edit History:
//  -------------
//  09/07/92(brn) - Add comment header
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

// Define functions for the svcIoSpecFunc SVC for disks

#ifndef _XOSDISK_H_
#define _XOSDISK_H_

#include <_prolog.h>

// Define partition types - These are the same as the values stored in the
//   partition table.

#define PRTN_FAT12  0x01	// FFS with 12 bit FAT entries
#define PRTN_FAT16  0x04	// FFS with 16 bit FAT entries less than 32MB
#define PRTN_EXT    0x05	// Extended partition
#define PRTN_FAT16H 0x06	// FFS with 16 bit FAT entries 32MB or larger
#define PRTN_NTFS   0x07	// NTFS file system
#define PRTN_FAT32  0x0B	// FFS with 32 bit FAT entries
#define PRTN_FAT32X 0x0C	// FFS with 32 bit FAT entries with "LBA support"
#define PRTN_FAT16X 0x0E	// FFS with 16 bit FAT entries with "LBA support"
#define PRTN_EXTX   0x0F	// FFS extended partition with "LBA support"
#define PRTN_XOS    0x23	// XOS native file system

// Define file system types - These are internal XOS values.

#define FS_NONE  0x00		// No reconized file system
#define FS_FAT12 0x01		// FFS with 12 bit FAT entries
#define FS_FAT16 0x02		// FFS with 16 bit FAT entries
#define FS_FAT32 0x03		// FFS with 32 bit FAT entries
#define FS_XOS   0x04		// XOS native file system

// Define functions for the svcIoSpecFunc SVC for disks

#define DSF_SETDS    1		// Set disk state
#define DSF_DISMOUNT 2		// Dismount disk
#define DSF_MOUNT    3		// Mount disk
#define DSF_FORMAT   4		// Format disk

// Define offsets for the disk/cache status data block (returned as the
//  value of the DCSINFO DISK class characteristic)

typedef struct dcsinfodata
{	long sbuft;				// Total number of system cache buffers
    long sbufa;				// Number of available system cache buffers
    long sbuff;				// Number of free system cache buffers
    long sbufo;				// Number of system cache buffers with 0 users
							//   (open FIBs)
    long sbufd;				// Number of system cache buffers with 0 users
							//   (defered write FAT blocks)
    long sbuf1;				// Number of system cache buffers with 1 user
    long sbuf2;				// Number of system cache buffers with 2 users
    long sbuf3;				// Number of system cache buffers with 3 users
    long sbuf4;				// Number of system cache buffers with 4 users
    long sbuf5;				// Number of system cache buffers with 5 or more
							//   users
    long dbuft;				// Total number of data cache buffers
    long dbufa;				// Number of available data cache buffers
    long dbuff;				// Number of free data cache buffers
    long dbufo;				// Number of data cache buffers with 0 users
							//   (open FIBs - should always be 0!)
    long dbufd;				// Number of data cache buffers with 0 users
							//   (defered write data blocks)
    long dbuf1;				// Number of data cache buffers with 1 user
    long dbuf2;				// Number of data cache buffers with 2 users
    long dbuf3;				// Number of data cache buffers with 3 users
    long dbuf4;				// Number of data cache buffers with 4 users
    long dbuf5;				// Number of data cache buffers with 5 or more users
} dcsinfo_data;

#define	dcsi_SIZE	(sizeof(dcsinfo_data))

#include <_epilog.h>

#endif // _XOSDISK_H_
