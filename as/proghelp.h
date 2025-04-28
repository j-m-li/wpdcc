// String control block structure

typedef struct strnode
{	struct strnode *prev;
    struct strnode *next;
    char *string;
} STR_NODE;

typedef struct strctrlb
{	int total;
    int pos;            	// The current position in the structure
    int page;           	// The size (in lines) of a page
    int lines;          	// The number of lines (nodes) in use
    STR_NODE *head;     	// Pointer to head node
    STR_NODE *tail;     	// Pointer to tail node
    STR_NODE *curr;     	// Pointer to current node
    char *buffer;       	// Large character buffer, used by give_page
} STR_CB;

typedef struct col_data
{	long fgc;
    long bgc;
    long fgf;
    long bgf;
    long atr;
} COLDATA;


typedef struct proginfo
{	int       console;			// TRUE if console is output device
    int       majedt;			// major edit number;
    int       minedt;			// minor edit number;
    long      errno1;			// last error number
	long	  screen_width;		// Should be set from svcIoTrmMode result
	long	  screen_height;	// Should be set from svcIoTrmMode result
	long	  handle; 			// The terminal device handle
	long	  page;				// The display page from handle
	COLDATA   old_color;		// The color scheme on entry
	COLDATA   hdr_color;		// The color scheme for help header
	COLDATA   bdy_color;		// The color scheme for help body
	STR_CB    scb; 				// Heap string control block
    char     *copymsg;			// Pointer to the Copyright notice string
    char     *prgname;			// Pointer to the Program name string
    char     *build;			// Pointer to the Program build date string
    char     *desc;				// Pointer to the Program description
    char     *example;			// Pointer to the Program example string
    arg_spec *opttbl;			// Pointer to the option table
	arg_spec *kwdtbl;			// Pointer to the keyword table
} Prog_Info;

void reg_pib(Prog_Info *user_pib);	// Must be called before optusage
void opthelp(void);
int  getTrmParms(void);
int  getHelpClr(void);