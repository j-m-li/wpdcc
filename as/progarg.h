// Define bits for flags (argument to PROGARG)

#define PAF_PATOM     0x8000	// Treat plus sign as an atom
								//  Note: Use of both PAF_PATOM and
								//	  PAF_PSWITCH not allowed,
								//	  PAF_PSWITCH has priority
#define PAF_EATQUOTE  0x4000	// Strip quotes from the strings
#define PAF_MNORMAL   0x2000	// Treat minus sign as normal character
#define PAF_PSWITCH   0x1000	// Allow plus sign as a switch character
#define PAF_INDIRECT  0x0800	// Allow indirect files
#define PAF_ECHOINDIR 0x0400	// Echo indirect input to stdout
#define PAF_MWEAK     0x0200	// Treat minus sign as a "weak" switch character
								//   (it will start an option but can also be
								//   embedded in an option keyword)

// Define values for argdata.flags

#define ADF_NVAL   0x4000		// Numeric value present
#define ADF_SSVAL  0x2000		// Short string value present
#define ADF_LSVAL  0x1000		// Long string value present
#define ADF_XSVAL  0x0800		// String table index value present
#define ADF_NONE   0x0400		// No = after option or keyword
#define ADF_NULL   0x0200		// No value after =
#define ADF_NEGATE 0x0040		// Value was negated (prefix NO)
#define ADF_NVAL16 0x0020		// Value is numeric 16 bit
#define ADF_NVAL8  0x0010		// Value is numeric 8 bit
#define ADF_BOOL   0x0008		// Value is a boolean
#define ADF_FLAG   0x0004		// Value is a bit field

// Values for argspec.flags

#define ASF_VALREQ 0x8000	// A value is required
#define ASF_NVAL   0x4000	// Value may be numeric
#define ASF_SSVAL  0x2000	// Value may be a short string
#define ASF_LSVAL  0x1000	// Value may be a long string
#define ASF_XSVAL  0x0800	// String table index value present
#define ASF_PLUS   0x0100	// Restrict switch character to '+' only
#define ASF_MINUS  0x0080	// Restrict switch character to '-' only
#define ASF_NEGATE 0x0040	// Value may be negated (prefix NO)
#define ASF_NVAL16 0x0020	// Value may be numeric 16 bit
#define ASF_NVAL8  0x0010	// Value may be numeric 8 bit
#define ASF_BOOL   0x0008	// Value is a boolean (no input value, negatable)
#define ASF_FLAG   0x0004	// Value is a bit field (no input value, negatable)
#define ASF_STORE  0x0002	// Store value directly

// Define structure which describes argument values

typedef struct argdata
{   char *name;					// Pointer to name of option or keyword
    union						// Value of option or keyword
    {	long  n;
		char *s;
		char  c[4];
    }     val;
    long  data;					// Data from option or keyword definition
    long  flags;				// Value description bits
    long  length;				// Length of string value
} arg_data;

// Combined sub options/help structure for XSVAL type fields

typedef struct suboptions
{   char *option;
    char *desc;
} SUBOPTS;

// Define structure which specifies options and keywords

typedef struct argspec
{   char *name;					// Name of option or keyword
    long  flags;				// Flag bits
    union
    {	SUBOPTS *s;				// Pointer to string value table
		long    *l[4];			// Array of bitfield masks
    }     svalues;
    union
    {	int (*func)(arg_data *); // Pointer to function
		void  *t;				// Pointer to target location
    }     func;
    union
    {	long l;					// Data to pass to function
		int  i[2]; 	        	// Data to pass to function
		char c[4];				// Data to pass to function
    }     data;
    char *help_str;				// Pointer to optional help string
} arg_spec;

int progarg(char **apnt, unsigned int flags, arg_spec *optbl, arg_spec *kwtbl,
	int (*fnc)(char *), void (*uerrrtn)(char *, char *), int (*contfnc)(void),
    char *indirext);
