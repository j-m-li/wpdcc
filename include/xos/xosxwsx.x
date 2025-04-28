#include <ft2build.h>
#include <freetype.h>
#include <ftglyph.h>

typedef struct tb_ TB;
typedef struct tl_ TL;
typedef struct ib_ IB;
typedef struct bgc_ BGC;

extern uchar screenbufr[];		// Offset of the display screen buffer
extern uchar rgnbufr[];			// Offset of the temporary region buffer

#define RGNBSZ 0x40000			// Maximum size of the temporary region buffer


typedef FNT *XWSFNTLOAD(char *name, long height, long width, long attr);
#pragma aux (XWSCALL) XWSFNTLOAD;

typedef void XWSFNTUNLOAD(FNT *fnt);
#pragma aux (XWSCALL) XWSFNTUNLOAD;

typedef GCB *XWSFONTGETGLYPH(FNT *fnt, int chr);
#pragma aux (XWSCALL) XWSFONTGETGLYPH;

typedef void RGNFILL(DRW *drw, RGN *rgn, long xpos, long ypos, long color);
#pragma aux (XWSCALL) RGNFILL;

typedef int RGNCOPY(DRW *srcdrw, DRW *dstdrw, RGN *rgn, long srcxpos,
		long srcypos, long dstxpos, long dstypos);
#pragma aux (XWSCALL) RGNCOPY;

typedef void RECTFILL(WIN *win, long xpos, long ypos, long width, long height);
#pragma aux (XWSCALL) RECTFILL;

typedef CHARDRAW(DRW *drw, RGN *rgn, int xrgn, int yrgn, int xpos, int ypos,
		GBM *gbm, int height, int fgcol, int bgcol);
#pragma aux (XWSCALL) CHARDRAW;

typedef void SETCURSOR(CUR *newcur);
#pragma aux (XWSCALL) SETCURSOR;

typedef void SHWCURSOR(void);
#pragma aux (XWSCALL) SHWCURSOR;

typedef void WAITIDLE(void);
#pragma aux (XWSCALL) WAITIDLE;

typedef void UPDATE(void);
#pragma aux (XWSCALL) UPDATE

typedef void GETCOLOR(long color);
#pragma aux (XWSCALL) GETCOLOR;

void    xwsdodraw(WIN *win);
#pragma aux (XWSCALL) xwsdodraw;

void    xwsbitmapgiveib(WIN *win, IB **ppnt);
#pragma aux (XWSCALL) xwsbitmapgiveib;

long    xwsbitmaploadjpeg(FILE *file, WIN *win, long bits, BITMAP **bm);
#pragma aux (XWSCALL) xwsbitmaploadjpeg;

long    xwsbitmaploadbmp(FILE *file, WIN *win, long bits, BITMAP **bm);
#pragma aux (XWSCALL) xwsbitmaploadbmp;

long    xwsbitmaploadpng(FILE *file, WIN *win, long bits, BITMAP **bm);
#pragma aux (XWSCALL) xwsbitmaploadpng;

long    xwsbitmaploadgif(FILE *file, WIN *win, long bits, BITMAP **bm);
#pragma aux (XWSCALL) xwsbitmaploadgif;

GCB    *xwsfontgetglyph(FNT *fnt, int chr);
#pragma aux (XWSCALL) xwsfontgetglyph;

TB     *xwsfindtextboxbyinx(WIN *win, long inx, TB **ptb);
#pragma aux (XWSCALL) xwsfindtextboxbyinx;

TB     *xwsfindtextboxbystr(WIN *win, char *str, long len, TB **ptb);
#pragma aux (XWSCALL) xwsfindtextboxbystr;

void    xwsguierror(char *fmt, ...);

XWSFNTLOAD xwsftfload;
XWSFNTLOAD xwsbmfload;

GETCOLOR xwsgethicolor;
RGNFILL  xwsmemrgnfill;
RGNCOPY  xwsmemrgncopy;
RECTFILL xwsmemrectfill;
CHARDRAW xwsmemchardraw;
void     xwsretins(void);
void     xwsret4a(void);		// This is not really right, but it works
								//   since we always cast this!

long  dispchar(WIN *win, FNT *fnt, long chr, long xpos, long ypos, long fgcol,
    long bgcol);
#pragma aux (XWSCALL) dispchar;


void *xwscmalloc(int size);
void  xwsfree(void *);
void *xwsmalloc(int size);

// Define vectors (offset from our base vector)

#define VECT_CTLP  0
#define VECT_MOUSE 1
#define VECT_IPM   2

// Define structure for the RGN (region object)

struct rgn
{   ulong  id;					// ID = 'RGN*'
    ushort user;
    uchar  type;				// Region type
    uchar  res1[1];				// Reserved
    long   size;				// Total size of RGN (bytes)
    long   usecnt;				// Use count (number of users - 1)
    long   ycount;				// Number of Y index table entries
    long   left;				// Left side X position
    long   right;				// Right side X position
    long   top;					// Top Y position
    long   bottom;				// Bottom Y position
    RSB   *ytable[1];			// Start of the Y index table
};

// Define structure for each RSB (Region Segment Block)

struct rsb
{   long left;					// Left side
    long right;					// Right side
};

// Define values for rgn.type

#define RGNTYPE_NULL     0		// Null region
#define RGNTYPE_SIMPLE   1		// Simple region
#define RGNTYPE_COMPLEX  2		// Complex region

// Define structure for the DRW (drawing context object)

struct drw
{   ulong      label;			// ID
	ushort     user;
    uchar      type;			// Type
    uchar      disable;			// Non-zero if disabled
    uchar     *buffer;			// Offset of start of display buffer
    long       bufsize;			// Size of buffer (bytes)
    long       width;			// Width (pixels)
    long       height;			// Height (pixels)
    long       pixelspan;		// Span between lines (pixels)
    long       bytespan;		// Span between lines (bytes)
    long       pixelsize;		// Pixel size (bytes)
    WAITIDLE  *waitidle;		// Offset of wait until idle routine
	UPDATE    *update;			// Offset of display update routine
	GETCOLOR  *getcolor;		// Offset of get color routine
	RGNFILL   *rgnfill;			// Offset of region fill routine
	RGNCOPY   *rgncopy;			// Offset of region copy routine
	RECTFILL  *rectfill;		// Offset of rectangle fill routine
	CHARDRAW  *chardraw;		// Offset of character draw routine
	SETCURSOR *setcursor;		// Offset of set cursor routine
	SHWCURSOR *shwcursor;		// Offset of show cursor routine
};


// Define values for drw_type

#define DRWTYPE_DISPLAY 1		// Physical display
#define DRWTYPE_MEMORY  2		// Memory


typedef struct
{   TL    *bashlbgnline;
    ushort bashlbgnchar;
    ushort bashlbgnindex;
} HLA;

// Define structure for common part of the WIN (window object)

struct winbas
{   ulong  label;				// Label = WIN#
    ushort user;
    ushort xxx1;
    DRW   *drw;					// Pointer to DRW for window
    long   offset;				// Offset of window in its display buffer
    WIN   *parent;				// Offset of WIN for parent window
    WIN   *fchild;				// Offset of WIN for first child window
    WIN   *sibling;				// Offset of WIN for next sibling window
    uchar  type;				// Window type
    uchar  moving;
    uchar  drawflg;
    uchar  caret;
    long   xpos;				// X position in parent window
    long   ypos;				// Y position in parent window
    long   width;				// Width
    long   height;				// Height
    long   scnxpos;				// X position on screen
    long   scnypos;				// Y position on screen
    long   zorder;				// Z-order (larger value is toward viewer)
    long   xorg;
    long   yorg;
    long   clntwidth;			// Width of client area
    long   clntheight;			// Height of client area
    long   leftwidth;			// Width of left border area
    long   rightwidth;
    long   topheight;			// Height of top border area
    long   btmheight;
    long   bits1;				// Window bits 1
    long   bits2;				// Window bits 2
    long   bits3;				// Window bits 3
    long   bits4;				// Window bits 4
    long   evmask;				// Event mask bits
    long   status;				// Window status bits
    long   cursor;				// Offset of CUR for cursor for window
    long   mvxofs;				// X offset for cursor when dragging window
    long   mvyofs;				// Y offset for cursor when dragging window
    WIN   *drawnext;			// Offset of next WIN in the draw list
    RGN   *basergn;				// Offset of RGN for base region for window (this
								//   region covers the entire window)
    RGN   *clientrgn;			// Offset of RGN for client region for window
    RGN   *userrgn;				// Offset of RGN for user specified clipping
								//   region (this is initially a copy of
								//   basclientrgn and is always a subset of
								//   basclientrgn)
    RGN   *maskrgn;				// Offset of RGN for currently visible part of
								//   the window (this does not exclude the
								//   parts not in the clipping region)
    RGN   *cliprgn;				// Offset of current clipping region RGN (this
								//   is the intersection of basmaskrgn and
								//   basuserrgn - it is used to clip
								//   everything drawn to the window)
    RGN   *drawrgn;				// Offset of RGN for current redraw region
	RGN   *drwmaskrgn;
    RGN   *savcliprgn;			// Offset of saved clipping region RGN when in
								//   redraw mode
    WIN   *prevmodal;			// Previous modal window (bit 31 set to
								//   indicate previous focus, not modal window)
    TB    *tbhead;
    TB    *tbtail;
	HLA    hlbgn;
	HLA    hlend;
	ushort carettar;
	ushort xxx2;
	IB    *ibhead;
    long   bgcoln;				// Normal background color
    long   bgcola;				// Alternate background color
    long   bgbmn;				// Normal background bitmap
    long   bgbma;				// Alternate background bitmap
    long   brdrwo;				// Outer border width
    long   brdrwc;				// Center border width
    long   brdrwi;				// Inter border width
    long   shdcoln;				// Normal border shadow color
    long   shdcola;				// Alternate border shadow color
    long   hilcoln;				// Normal border highlight color
    long   hilcola;				// Alternate border highlight color
    long   brdrcol;				// Center border color
    long   brdrbm;				// Center border bitmap
    uchar  pressofs;
    uchar  yyyy[3];
    NOT   *fnot;				// First notify object
    EDB   *edb;
    long   usrdraw;				// Offset of user redraw function
    long   usrevent;			// Offset of system event function
	long   evarg1;
	void  *evarg2;
    long   usrpntr;				// User defined pointer
};

struct win_
{	struct winbas bas;
};

// Define values for type in WIN

#define WINTYPE_CONTAINER 1
#define WINTYPE_BUTTON    2
#define WINTYPE_CHECKBOX  3
#define WINTYPE_LIST      4
#define WINTYPE_SELECT    5
#define WINTYPE_VALUE     6
#define WINTYPE_RESPONSE  7
#define WINTYPE_EDIT      8

struct list_
{	struct winbas bas;
	long   selitem;
	long   alphamin;
	long   alphamax;
};

// Define structure for the NOT (notify object)

struct not
{   ulong id;					// ID
    void *not;					// The notifier object
    NOT  *nextnot;				// Next notify object for same notifier
    NOT  *nextwin;				// Next notify object for same window
    WIN  *win;					// This window object
    NOT **head;					// Offset of head pointer for this notify list
};

// Define structure for the CUR (cursor object)

struct cur
{   ulong  id;					// ID
    ushort user;
    ushort xxx;
    long   xsize;				// X size
    long   ysize;				// Y size
    long   xhot;				// X hot-spot position
    long   yhot;				// Y hot-spot position
    long   color1;				// First color
    long   color2;				// Second color
    uchar *patbm;				// Pointer to pattern bitmap
    uchar *maskbm;				// Pointer to mask bitmap
};


// Define structure for the TB (Text Block)

struct tb_
{	ulong  label;				// Label (TB##)
	TB    *next;				// Offset of next TB in window
	TB    *prev;
	TL    *tlhead;				// Offset of first TL
	TL    *tltail;			// Offset of last TL
	long   inx;				// Index value
	long   xpos;				// X position
	long   ypos;				// Y position
	long   yinc;				// Y increment (list windows only)
	long   width;				// Width
	long   height;				// Height
	long   lmargin;				// Left margin
	long   rmargin;				// Right margin
	long   tmargin;				// Top margin
	long   bmargin;				// Bottom margin
	long   nrmcol;				// Normal text color
	long   altcol;				// Alternate text color
	long   hlcol;
	long   caretcol;
	long   bgcol;				// Background fill color
	ushort fnum;				// Font number
	ushort fattr;				// Font attributes
	ushort fheight;				// Font height
	ushort fwidth;				// Font width
	ushort attr;
	ushort xxxx;
};

// Define the background color structure

struct bgc_
{	long color;					// Color for background area
	long length;				// Length of background area
	long ascent;				// Ascent for background area
	long descent;				// Descent for background area
};

// Define the TL (Text Line) structure

struct tl_
{	ulong  label;				// Label (TL##)
	TB    *tb;					// Address of TB for block
	TL    *next;				// Address of next TL in block
	TL    *prev;
	long   xpos;				// X position of line (relative to text box)
	long   ypos;				// Y position of baseline of line (relative to
								//   text box)
	long   length;				// Length of line in pixels
	long   ascent;				// Maximum ascent from baseline (positive)
	long   descent;				// Maximum descent from baseline (positive)
	ushort xxx;

	ushort hortab;
	long   xright;				// Right margin X position
	long   hlpos;
	long   hllen;
	long   lspace;				// Line spacing
	long   pspace;				// Paragraph spacing
	uchar  bits;
	uchar  lfmt;				// Line format
	ushort lnum;				// Line number in block (starts at 0)
	ushort fnum;				// Font number
	ushort fattr;				// Font attributes
	ushort fsize;				// Font size
	ushort fwidth;				// Font width
	long   txtcol;				// Initial forground (text) color
	BGC    bgcol;				// Initial background color
	ushort numchar;				// Number of characters in line
	ushort count;				// Number of bytes in line
	char   text[1];				// First data byte
};

// Define bits for tl_bits

#define TLB_MONO 0x10			// Line uses single mono-spaced font
#define TLB_WRAP 0x04			// Line has been word-wrapped
#define TLB_EOL  0x02			// Line terminated with EOL (NL) character
#define TLB_EOP  0x01			// Line terminated with EOP character

// Define bits for tl_fattr

#define TLA_XBOLD  0x1000		// Extra bold
#define TLA_BOLD   0x0800		// Bold
#define TLA_DBOLD  0x0400		// Demi-bold
#define TLA_NORMAL 0x0200
#define TLA_LIGHT  0x0100		// Light
#define TLA_STRIKE 0x0008		// Strike-out
#define TLA_DUNDER 0x0004		// Double underscore
#define TLA_UNDER  0x0002		// Underscore
#define TLA_ITALIC 0x0001		// Italic

// Define values for tl_lfmt

#define TFMT_LEFT  0			// Hard left
#define TFMT_RIGHT 1			// Hard right
#define TFMT_CEN   2			// Window centered
#define TFMT_JUST  3			// Justified

// Define offsets in the IB (image blck)

struct ib_
{	long    label;
	IB     *next;
	IB     *prev;
	long    inx;
	long    xpos;
	long    ypos;
	BITMAP *bm;
};

// Define structure for the FNB (Font Name Block)
// There is one FNB for each font that is available for use. These FNBs are
//   searched when a font is requrested.

struct fnb
{	ulong label;				// Label (FNB#)
	FNB  *next;					// Offset of next FNB
	FSB  *ffsb;					// First FSB for this FNB
	uchar ftcmap;				// FreeType character map index
	char  fontname[1];			// Font name
};

// Define structure for the FSB (Font Size Block)
// FSBs are used (along with FNBs) to specify the available font sizes. For
//   bitmapped fonts there is one FSB for each available size. For scaled
//   fonts there is one FSB which specifies 0 for both height and width.

struct fsb
{   ulong label;				// Label (FSB#)
    FSB  *next;					// Next FSB for same font
    FNB  *fnb;					// FNB
    FNT  *ffnt;					// First FNT
	int   szinx;				// Index used to select size in font files
								//   which contain multiple sizes
    ulong height;				// Height of font (pixels)
    ulong width;				// Average width of font (pixels)
	int   faceinx;				// FreeType2 face index
	char  filename[1];			// File name
};


// Define structure for the FNT (font object)
// An FNT is allocated for each font face that is loaded.

struct fnt
{	FNTP;						// Publicly available part (see xosxws.h)
	FNT    *next;				// Pointer to next FNT from same font size
	FNB    *fnb;				// FNB for this font
	FSB    *fsb;				// FSB for this font
	FT_Face ftface;				// Corresponding FreeType2 Face object
	long    loadcnt;			// Load count
	GCB    *glyphtbl[128];		// GCB hash table
};


// Define structure for the glyph cach block (GCB).
// These blocks are used to cache glyph definitions. If the glyph is smaller
//   than about 40x40 the bitmap is cached in this block. For larger glyphs
//   from scalable fonts the vector definition is cached. Larger glyphs from
//   bitmapped files are not cached.
// In any case, the actual data that is cached is the FreeType2 glyphslot
//   struture.

struct gcb_
{	ulong    label;				// Label (GCB#)
	GCB     *hash;				// Next GCB in the hash chain
	GCB     *next;				// Next GCB in the management list
	GCB     *prev;				// Previous GCB in the management list
	ulong    glyphinx;			// Glyph index in the font file
	FNT     *fnt;				// FNT which own this GCB
	FT_Glyph ftglyph;			// Pointer to the FreeType2 glyphslot structure
};

extern GCB xwsbadgcb;


struct bitmap_
{	ulong  label;				// Lable (BM##)
	ushort user;
	ushort xxx;
///	NOT   *fnot;				// First notify object for BITMAP
	long   status;				// Status bits
	DRW    drw;					// Used when drawing on this bitmap
	uchar  bitmap[1];			// Data array
};

// Define values for status in BITMAP

#define BMS_DONE 0x0001			// Bitmap is completely loaded

struct ptr_
{	long    label;				// Label (PTR#)
	long    hndl;				// Printer device handle
	long    mode;				// Print mode
	long    width;				// Current page width
	long    monseedsz;			// Size of the monochrome seed row buffer
	long    colseedsz;			// Size of the color seed row buffer
	long    outcnt;				// Available output buffer bytes
	long    bfrpos;				// Position in file of current buffer
	long    lenpos;				// Position in file of length value of current
								//   record
	long    reclen;				// Record length
	uchar  *lenpnt;				// Pointer to length value of current record
								//   in current buffer
	uchar  *monseedrow;			// Monochorme seed row buffer (1 bit per pixel)
	ushort *colseedrow;			// Color seed row buffer (2 bytes per pixel)
	uchar  *currow;				// Current row buffer (1 bit per pixel)
	uchar  *genrow;				// Row output buffer (mode 9 compression)
	uchar  *outbfr;				// Printer output buffer
	uchar  *outpnt;
	uchar   cmode;				// True if color mode valid
	uchar   mmode;				// True if monochrome mode valid
};

extern FNB   *xwsfirstfnb;

extern TRMMODES xwstrmdata;

extern ushort xwsscnnum;
extern ushort xwstrmnum;
extern long   xwsvectbase;
extern CUR   *xwscursorcur;
extern DRW    xwsscreendrw;
extern FNT   *xwsdftfnt;

void xwsupdatecursor(void);
