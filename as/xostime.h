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

int  ddt2str(char *string, char *format, time_dz *tm);
