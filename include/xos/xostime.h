//------------------------------------------------------------------------------
//
//  XOSTIME.H - Time parameters for XOS
//
//  Edit History:
//  -------------
//  09/05/88(brn) - Created first version
//   8Feb95 (sao) - Creation of dt_parse() routine
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

#ifndef _XOSTIME_H_
#define _XOSTIME_H_

#include <_prolog.h>

// Define time constants

#define SPPERSEC      50		// Number of scheduler periods per second
#define SPPERDAY      4320000	// Number of scheduler periods per day
#define TICKPERSP     200000	// Number of system ticks per scheduler period
#define TICKPERDAY_LO 0x2A69C000
#define TICKPERDAY_HI 0xC9
#define TICKMAX_LO    0xF4373FFF
#define TICKMAX_HI    0x2BCA2875

#define ST_MILLISEC 10000		// Number of system ticks per millisecond
#define ST_SECOND   10000000	// Number of system ticks per second
#define ST_MINUTE   600000000	// Number of system ticks per minute

// Define offsets in the time_d structure - this is used to hold DOS (or broken
//   down) time values

typedef struct tmx
{   unsigned short tmx_msec;    // Milliseconds (0 - 999)
    unsigned short tmx_sec;     // Second (0 -59)
    unsigned short tmx_min;     // Minute (0 - 59)
    unsigned short tmx_hour;    // Hour (0 - 23)
    unsigned short tmx_mday;    // Day of month (1 - 31)
    unsigned short tmx_mon;     // Month in year (1 - 12)
    unsigned short tmx_year;    // Year (1600 - )
    unsigned short tmx_wday;    // Day of week (0 - 6, 0 = Sunday)
    unsigned short tmx_yday;    // Day of year (0 - 365)
} time_d;

typedef struct tmxz
{   unsigned short tmx_msec;    // Milliseconds (0 - 999)
    unsigned short tmx_sec;     // Second (0 -59)
    unsigned short tmx_min;     // Minute (0 - 59)
    unsigned short tmx_hour;    // Hour (0 - 23)
    unsigned short tmx_mday;    // Day of month (1 - 31)
    unsigned short tmx_mon;     // Month in year (1 - 12)
    unsigned short tmx_year;    // Year (1600 - )
    unsigned short tmx_wday;    // Day of week (0 - 6, 0 = Sunday)
    unsigned short tmx_yday;    // Day of year (0 - 365)
             short tmx_tzone;	// Time zone (offset from GMT in minutes)
             short tmx_dlst;	// Daylight savings time offset (minutes)
} time_dz;

// Define function values for the svcSysDateTime system call

#define T_GTSYSDTTM    1		// Get current date/time in system format
#define T_GTDOSDTTM    2		// Get current date/time in DOS format
#define T_GTSYSDTTMTZ  3		// Get current date/time with time zone
								//   information in system format
#define T_GTDOSDTTMTZ  4		// Get current date/time with time zone
								//   information in DOS format
#define T_GTPETIM      6		// Get process elapsed time (system format)
#define T_GTSETIM      7		// Get session elapsed time (system format)
#define T_GTPCTIM      8		// Get process CPU time (system format)
#define T_CVDOS2SYS    9		// Convert DOS to system date/time
#define T_CVSYS2DOS   10		// Convert system to DOS date/time
#define T_GTHRDTTM    11		// Get current DHR date/time (system format)
#define T_GTHRPETIM   12		// Get high-res process time (system format)
#define T_GTHRSETIM   13		// Get high-res session time (system format)
#define T_GTHRDTTMTZ  14		// Get current DHR date/time with time zone
								//   information (system format)
#define T_GTRTCDTTM   15		// Get RTC date/time (admin priv)
#define T_STSYSDTTM   16		// Set system date/time (admin priv)
#define T_STRTCDTTM   17		// Set RTC date/time (admin priv)
#define T_GTTZINFO    18		// Get time zone information
#define T_STTZINFO    19		// Set time zone information
#define T_CVSYS2DAYS  20		// Convert system date/time to days since
								//   1-Jan-0001

// Define time for XOS

// Legacy/Standard "C"/XOS definition

#ifndef _TIME_T_DEFINED
  #define _TIME_T_DEFINED
  #define _TIME_T_DEFINED_
  typedef signed long time_t;
#endif

typedef struct
{	llong dt;
}   time_s;

typedef struct
{	llong dt;
	short tzone;
	short dlst;
} time_sz;

typedef struct
{	time_s sys;
	time_d dos;
} time_x;

typedef struct hrtime
{	ulong second;
	ulong fraction;
} time_hr;

//  The following #defines are used by dt_parse

// Acceptable date formats for dt_parse are:
//
//    US      mm/dd/[[yy]yy]
//    INTL    dd.mm.[[yy]yy]
//    TEXT    dd[-]mmm[-][[yy]yy]  or
//            mmm[-]dd[-][[yy]yy]
//    PACK    yymmdd (all six values must be present)
//    TIME    hh:mm[:ss][A|P]
//
//  Note:    If the year is not included, current year is used
//           If century is not included, current century is used

#define INVDTFORMAT     1
#define INVMONTH        2
#define INVDAY          3
#define INVDTRANGE      4
#define INVTMMIN        5
#define INVTMHOUR       6
#define INVTMSECOND     7
#define INVTMFORMAT     8

// XOS time functions

void add2sdt(time_s *dtv, long value);
void days2ticks(time_s *dt, long days);
int  dt_parse(char *inp_str, time_s *dt);
int  ddt2sdt(time_d *tm, time_s *dtv);
int  ddt2str(char *string, char *format, time_dz *tm);
void dosdttm(time_d *datetime);
long hourmin2sys(long time);
void sdt2ddt(time_s *dtv, time_d *tm);
int  sdt2str(char *string, char *format, time_sz *sdt);
void sdtadd(time_s *sum, time_s *a1, time_s *a2);
int  sdtcmp(time_s *a, time_s *b);
void sdtsub(time_s *dif, time_s *s1, time_s *s2);
long sdt2days(time_s *dt);
long ticks2days(time_s *dt);
void xos_sysdttm(time_s *datetime);

# include <_epilog.h>

#endif  // _XOSTIME_H_
