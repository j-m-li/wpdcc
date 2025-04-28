#if !defined(_XOSXXWS_H_)
#define _XOSXXWS_H_

#include <xosxws.h>
#include <ft2build.h>
#include <freetype.h>
#include <ftglyph.h>

typedef struct xwsfmt_    XWSFMT;
typedef struct xwstb_     XWSTB;
typedef struct xwstl_     XWSTL;
typedef struct xwsib_     XWSIB;
typedef struct xwsbgc_    XWSBGC;
typedef struct xwsxcol_   XWSXCOL;

extern uchar screenbufr[];		// Offset of the display screen buffer
extern uchar rgnbufr[];			// Offset of the temporary region buffer

#define RGNBSZ 0x40000			// Maximum size of the temporary region buffer

void xwsdisplaysessionmenu(void);
#pragma aux (XWSCALL) xwsdisplaysessionmenu;


///#define xwsdounscale(value, dval) (value * dval)

typedef XWSFNT *XWSFNTLOAD(char *name, long height, long width, long attr);
#pragma aux (XWSCALL) XWSFNTLOAD;

typedef void    XWSFNTUNLOAD(XWSFNT *fnt);
#pragma aux (XWSCALL) XWSFNTUNLOAD;

typedef XWSGCB *XWSFONTGETGLYPH(XWSFNT *fnt, int chr);
#pragma aux (XWSCALL) XWSFONTGETGLYPH;

// Prototypes for the DRW function dispatch functions

typedef void XWSWAITIDLE(void);
#pragma aux (XWSCALL) XWSWAITIDLE;

typedef void XWSUPDATE(void);
#pragma aux (XWSCALL) XWSUPDATE

typedef void XWSGETCOLOR(long color);
#pragma aux (XWSCALL) XWSGETCOLOR;

typedef void XWSEXPCOLOR(XWSXCOL *xcolor, uchar *color);
#pragma aux (XWSCALL) XWSEXPCOLOR;

typedef long XWSCOMCOLOR(XWSXCOL *xcolor, uchar *color);
#pragma aux (XWSCALL) XWSCOMCOLOR;

typedef void XWSRGNFILL(XWSDRW *drw, XWSRGN *rgn, long xpos, long ypos,
		long color);
#pragma aux (XWSCALL) XWSRGNFILL;

typedef int XWSRGNCOPY(XWSDRW *srcdrw, XWSDRW *dstdrw, XWSRGN *rgn, long srcxpos,
		long srcypos, long dstxpos, long dstypos);
#pragma aux (XWSCALL) XWSRGNCOPY;

typedef void XWSRECTFILL(XWSWIN *win, long xpos, long ypos, long width,
		long height);
#pragma aux (XWSCALL) XWSRECTFILL;

typedef XWSCHARDRAW(XWSDRW *drw, XWSRGN *rgn, int xrgn, int yrgn, int xpos,
		int ypos, XWSGBM *gbm, int height, int fgcol, int bgcol);
#pragma aux (XWSCALL) XWSCHARDRAW;

typedef void XWSSETCURSOR(XWSCUR *newcur);
#pragma aux (XWSCALL) XWSSETCURSOR;

typedef void XWSSHWCURSOR(void);
#pragma aux (XWSCALL) XWSSHWCURSOR;



long    xwsaddhorscrollbar(XWSCNTR *win, XWSSABLPAR *parm, XWSEVENT hsbevent,
		XWSEDB *edb);
#pragma aux (XWSCALL) xwsaddhorscrollbar;

long    xwsaddverscrollbar(XWSCNTR *win, XWSSABLPAR *parm, XWSEVENT *vsbevent,
		XWSEDB *edb);
#pragma aux (XWSCALL) xwsaddverscrollbar;

void    xwsdodraw(XWSWIN *win);
#pragma aux (XWSCALL) xwsdodraw;

long    xwsbitmapcopyscaledns(XWSBITMAP *obm, long width, long height,
		long flags, XWSBITMAP **sbm);
#pragma aux (XWSCALL) xwsbitmapcopyscaledns;

void    xwsbitmapgiveib(XWSWIN *win, XWSIB **ppnt);
#pragma aux (XWSCALL) xwsbitmapgiveib;

long    xwsbitmaploadjpeg(FILE *file, XWSWIN *win, long bits, XWSBITMAP **bm);
#pragma aux (XWSCALL) xwsbitmaploadjpeg;

long    xwsbitmaploadbmp(FILE *file, XWSWIN *win, long bits, XWSBITMAP **bm);
#pragma aux (XWSCALL) xwsbitmaploadbmp;

long    xwsbitmaploadpng(FILE *file, XWSWIN *win, long bits, XWSBITMAP **bm);
#pragma aux (XWSCALL) xwsbitmaploadpng;

long    xwsbitmaploadgif(FILE *file, XWSWIN *win, long bits, XWSBITMAP **bm);
#pragma aux (XWSCALL) xwsbitmaploadgif;

long    xwsbitmapnew(long width, long height, XWSBITMAP **pbm);
#pragma aux (XWSCALL) xwsbitmapnew;

long    xwsbitmapscalecolorlilow(XWSDRW *drw, ushort *datad, long wd, long hd,
		ushort *datas, long ws, long hs);
#pragma aux (XWSCALL) xwsbitmapscalecolorlilow;

long    xwsbitmapscalesmoothlow(XWSDRW *drw, ushort *datad, long wd, long hd,
		ushort *datas, long ws, long hs, long size);
#pragma aux (XWSCALL) xwsbitmapscalesmoothlow;

long    xwscreatewindow(XWSWIN *parent, int wtype, long size, long xpos,
		long ypos, long width, long height, XWSWINPAR *parms, XWSEVENT *event,
		long evmask, XWSEDB *edb, XWSDRW *drw, XWSWIN **win);
#pragma aux (XWSCALL) xwscreatewindow;

long    xwsdispchar(XWSWIN *win, XWSFNT *fNT, int chr, int xpos, int ypos,
		int color);
#pragma aux (XWSCALL) xwsdispchar;

void    xwsdoevent(XWSEDBLK *edblk);
#pragma aux (XWSCALL) xwsdoevent;

void    xwserror(long code, char *fmt, ...);

XWSGCB *xwsfontgetglyph(XWSFNT *fnt, int chr);
#pragma aux (XWSCALL) xwsfontgetglyph;

long    xwsfontgetlengthns(XWSFNT *fnt, char *text, long cnt);
#pragma aux (XWSCALL) xwsfontgetlengthns;

XWSTB  *xwsfindtextboxbyinx(XWSWIN *win, long inx, XWSTB **ptb);
#pragma aux (XWSCALL) xwsfindtextboxbyinx;

XWSTB  *xwsfindtextboxbystr(XWSWIN *win, char *str, long len, XWSTB **ptb);
#pragma aux (XWSCALL) xwsfindtextboxbystr;

XWSTB *xwslistfindbyypos(XWSLIST *win, long ypos);
#pragma aux (XWSCALL) xwslistfindbyypos;

void    xwslistscroll(XWSLIST *win, long amnt);
#pragma aux (XWSCALL) xwslistscroll;

void    xwslistsetseltb(XWSLIST *win, XWSTB *tb);
#pragma aux (XWSCALL) xwslistsetseltb;

int     xwslistputtbattop(XWSLIST *win, XWSTB *tb, int check);
#pragma aux (XWSCALL) xwslistputtbattop;

int     xwslistputtbinview(XWSLIST *win, XWSTB *tb);
#pragma aux (XWSCALL) xwslistputtbinview;

long    xwslisthsbevent(XWSEDBLK *edblk);
#pragma aux (XWSCALL) xwslisthsbevent;

long    xwslistvsbevent(XWSEDBLK *edblk);
#pragma aux (XWSCALL) xwslistvsbevent;


XWSRGN *xwsrgncreaterectns(long xpos, long ypos, long xsize, long ysize);
#pragma aux (XWSCALL) xwsrgncreaterectns;

void   xwstimerinit(void);
#pragma aux (XWSCALL) xwstimerinit;

void   xwswinmoveus(XWSWIN *win, long xpos, long ypos, long width, long height);
#pragma aux (XWSCALL) xwswinmoveus;


XWSFNTLOAD xwsftfload;
XWSFNTLOAD xwsbmfload;

XWSEVENT xwseventscrollbar;

XWSGETCOLOR xwsgethicolor;
XWSRGNFILL  xwsmemrgnfill;
XWSRGNCOPY  xwsmemrgncopy;
XWSRECTFILL xwsmemrectfill;
XWSCHARDRAW xwsmemchardraw;
void     xwsretins(void);
void     xwsret4a(void);		// This is not really right, but it works
								//   since we always cast this!

long  dispchar(XWSWIN *win, XWSFNT *fnt, long chr, long xpos, long ypos,
		long fgcol, long bgcol);
#pragma aux (XWSCALL) dispchar;


void *xwscmalloc(int size);
void  xwsfree(void *);
void *xwsmalloc(int size);

// Define vectors (offset from our base vector)

#define XWS_VECT_CTLP  0
#define XWS_VECT_MOUSE 1
#define XWS_VECT_TIMER 2
#define XWS_VECT_IPM   3

// Define expanded color structure

struct xwsxcol_
{	long red;
	long green;
	long blue;
};

// Define structure for the RGN (region object)

struct xwsrgn_
{   ulong   id;					// ID = 'RGN*'
    ushort  user;
    uchar   type;				// Region type
    uchar   res1[1];			// Reserved
    long    size;				// Total size of RGN (bytes)
    long    usecnt;				// Use count (number of users - 1)
    long    ycount;				// Number of Y index table entries
    long    left;				// Left side X position
    long    right;				// Right side X position
    long    top;				// Top Y position
    long    bottom;				// Bottom Y position
    XWSRSB *ytable[1];			// Start of the Y index table
};

// Define structure for each RSB (Region Segment Block)

struct xwsrsb_
{   long left;					// Left side
    long right;					// Right side
};

// Define values for rgn.type

#define RGNTYPE_NULL     0		// Null region
#define RGNTYPE_SIMPLE   1		// Simple region
#define RGNTYPE_COMPLEX  2		// Complex region

// Define the DRW function dispatch table

typedef struct
{	XWSWAITIDLE  *waitidle;
	XWSUPDATE    *update;
	XWSGETCOLOR  *getcolor;
	XWSEXPCOLOR  *expcolor;
	XWSCOMCOLOR  *comcolor;
	XWSRGNFILL   *rgnfill;
	XWSRGNCOPY   *rgncopy;
	XWSRECTFILL  *rectfill;
	XWSCHARDRAW  *chardraw;
	XWSSETCURSOR *setcursor;
	XWSSHWCURSOR *shwcursor;
} XWSDRWDSP;

// Define structure for the DRW (drawing context object)

struct xwsdrw_
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
	XWSDRWDSP *funcdsp;			// Function dispatch table
};


// Define values for drw_type

#define DRWTYPE_DISPLAY 1		// Physical display
#define DRWTYPE_MEMORY  2		// Memory


typedef struct
{   XWSTL *bashlbgnline;
    ushort bashlbgnchar;
    ushort bashlbgnindex;
} XWSHLA;

// Define structure for common part of the WIN (window object)

struct xwswin_
{   ulong     label;		// Label = WIN#
    ushort    user;
    ushort    xxx1;
    XWSDRW   *drw;			// Pointer to DRW for window
    long      offset;		// Offset of window in its display buffer
    XWSWIN   *parent;		// Offset of WIN for parent window
    XWSWIN   *fchild;		// Offset of WIN for first child window
    XWSWIN   *sibling;		// Offset of WIN for next sibling window
    uchar     type;			// Window type
    uchar     moving;
    uchar     drawflg;
    uchar     caret;
    long      xpos;			// X position in parent window
    long      ypos;			// Y position in parent window
    long      width;		// Width
    long      height;		// Height
    long      scnxpos;		// X position on screen
    long      scnypos;		// Y position on screen
    long      zorder;		// Z-order (larger value is toward viewer)
    long      xorg;
    long      yorg;
	long      xsfr;			// X scale factor rounding value
	long      xsfd;			// X scale factor divisor
	long      ysfr;			// Y scale factor rounding value
	long      ysfd;			// Y scale factor divisor
    long      clntwidth;	// Width of client area
    long      clntheight;	// Height of client area
    long      leftwidth;	// Width of left border area
    long      rightwidth;
    long      topheight;	// Height of top border area
    long      btmheight;
    long      bits1;		// Window bits 1
    long      bits2;		// Window bits 2
    long      bits3;		// Window bits 3
    long      bits4;		// Window bits 4
    long      evmask;		// Event mask bits
    long      status;		// Window status bits
    long      cursor;		// Offset of CUR for cursor for window
    long      mvxofs;		// X offset for cursor when dragging window
    long      mvyofs;		// Y offset for cursor when dragging window
    XWSWIN   *drawnext;		// Offset of next WIN in the draw list
    XWSRGN   *basergn;		// Offset of RGN for base region for window (this
							//   region covers the entire window)
    XWSRGN   *clientrgn;	// Offset of RGN for client region for window
    XWSRGN   *userrgn;		// Offset of RGN for user specified clipping
							//   region (this is initially a copy of
							//   basclientrgn and is always a subset of
							//   basclientrgn)
    XWSRGN   *maskrgn;		// Offset of RGN for currently visible part of
							//   the window (this does not exclude the
							//   parts not in the clipping region)
    XWSRGN   *cliprgn;		// Offset of current clipping region RGN (this
							//   is the intersection of basmaskrgn and
							//   basuserrgn - it is used to clip
							//   everything drawn to the window)
    XWSRGN   *drawrgn;		// Offset of RGN for current redraw region
	XWSRGN   *drwmaskrgn;
    XWSRGN   *savcliprgn;	// Offset of saved clipping region RGN when in
							//   redraw mode
    XWSWIN *prevmodal;		// Previous modal window (bit 31 set to
							//   indicate previous focus, not modal window)
	XWSWIN   *trvsnext;		// Address of next transversal window
    XWSTB    *tbhead;
    XWSTB    *tbtail;
	XWSHLA    hlbgn;
	XWSHLA    hlend;
	ushort    carettar;
	ushort    trvsinx;		// Transversal index
	XWSIB    *ibhead;
    long      bgcoln;		// Normal background color
    long      bgcola;		// Alternate background color
    long      bgbmn;		// Normal background bitmap
    long      bgbma;		// Alternate background bitmap
    long      brdrwo;		// Outer border width
    long      brdrwc;		// Center border width
    long      brdrwi;		// Inter border width
    long      shdcoln;		// Normal border shadow color
    long      shdcola;		// Alternate border shadow color
    long      hilcoln;		// Normal border highlight color
    long      hilcola;		// Alternate border highlight color
    long      brdrcol;		// Center border color
    long      brdrbm;		// Center border bitmap
    uchar     shift;
    uchar     yyyy[3];
    XWSNOT   *fnot;			// First notify object
    XWSEDB   *edb;
    XWSDRAW  *usrdraw;		// Offset of user redraw function
    XWSEVENT *usrevent;		// Offset of system event function
	long      evarg1;
	void     *evarg2;
    long      usrpntr;		// User defined pointer
};


struct xwssabl_
{	XWSSBAR *horsb;
	XWSSBAR *versb;
};

// Define values for type in XWSWIN

#define XWS_WINTYPE_SIMPLE    1
#define XWS_WINTYPE_CONTAINER 2
#define XWS_WINTYPE_LIST      3
#define XWS_WINTYPE_RESPONSE  4
#define XWS_WINTYPE_EDIT      5
#define XWS_WINTYPE_BUTTON    6
#define XWS_WINTYPE_CHECKBOX  7
#define XWS_WINTYPE_COMPOSITE 8		// FIrst composite type
#define XWS_WINTYPE_SCROLLBAR 8
#define XWS_WINTYPE_DROPDOWN  9
#define XWS_WINTYPE_INCDEC    10

struct xwscntr_
{	union
	{	XWSWIN bas;
		XWSWIN;
	};
	XWSSABL;
};

struct xwsbutn_
{	union
	{	XWSWIN bas;
		XWSWIN;
	};
	XWSSABL;
};


struct xwslist_
{	union
	{	XWSWIN bas;
		XWSWIN;
	};
	XWSSABL;
	XWSTB *selitem;
	XWSTB *topitem;
	long   alphamin;
	long   alphamax;
};


struct xwssbar_
{	union
	{	XWSWIN bas;
		XWSWIN;
	};
	XWSBUTN *btn1;			// Left or top button
	XWSBUTN *btn2;			// Right or bottom button
	XWSBUTN *thumb;			// Thumb button
	long     tmbrange;		// Logical units for 100% thumb
	long     tmbsize;		// Scroll bar size in logical units
	long     tmbpos;		// Scroll bar position in logical units
	long     tmbpxbegin;	// Pixel position of beginning (left or top)
							//   of the thumb area
	long     tmbpxtotal;	// Pixel size of the thumb area
	long     tmbpxpos;		// Pixel position of beginning (left or top)
							//   of thumb relative to start of thumb area
	long     tmbpxsize;		// Pixel size of thumb
	long     tmbpxgrab;		// Pixel distance between beginning of thumb
							//   and the current grab point
	ushort   state;			// State bits
	ushort   xxx;
	long     tmrhndl;		// Timer handle
	long     delayi;
	long     delayr;
};

// Define bits for state in XWSSCROLLBAR

#define XWSSBS_VERT  0x8000		// Vertical scroll bar
#define XWSSBS_DELAY 0x0010		// Doing initial repeat delay
#define XWSSBS_BAR1  0x0008		// Bar region 1 is pressed
#define XWSSBS_BAR2  0x0004		// Bar region 2 is pressed
#define XWSSBS_BTN1  0x0002		// Button 1 is pressed
#define XWSSBS_BTN2  0x0001		// Button 2 is pressed


// Define structure for the NOT (notify object)

struct xwsnot_
{   ulong    id;			// ID
    void    *not;			// The notifier object
    XWSNOT  *nextnot;		// Next notify object for same notifier
    XWSNOT  *nextwin;		// Next notify object for same window
    XWSWIN  *win;			// This window object
    XWSNOT **head;			// Offset of head pointer for this notify list
};

// Define structure for the CUR (cursor object)

struct xwscur_
{   ulong  id;				// ID
    ushort user;
    ushort xxx;
    long   xsize;			// X size
    long   ysize;			// Y size
    long   xhot;			// X hot-spot position
    long   yhot;			// Y hot-spot position
    long   color1;			// First color
    long   color2;			// Second color
    uchar *patbm;			// Pointer to pattern bitmap
    uchar *maskbm;			// Pointer to mask bitmap
};


// Define structure for the TB (Text Block)

struct xwstb_
{	ulong  label;			// Label (TB##)
	XWSTB *next;			// Offset of next TB in window
	XWSTB *prev;
	XWSTL *tlhead;			// Offset of first TL
	XWSTL *tltail;			// Offset of last TL
	long   inx;				// Index value
	long   xpos;			// X position
	long   ypos;			// Y position
	long   yinc;			// Y increment (list windows only)
	long   width;			// Width
	long   height;			// Height
	long   lmargin;			// Left margin
	long   rmargin;			// Right margin
	long   tmargin;			// Top margin
	long   bmargin;			// Bottom margin
	long   nrmcol;			// Normal text color
	long   altcol;			// Alternate text color
	long   hlcol;
	long   caretcol;
	long   bgcol;			// Background fill color
	long   bghlcol;			// Background highlight color
	ushort fnum;			// Font number
	ushort fattr;			// Font attributes
	ushort fheight;			// Font height
	ushort fwidth;			// Font width
	ushort attr;
	ushort xxxx;
};

// Define the background color structure

struct xwsbgc_
{	long color;				// Color for background area
	long length;			// Length of background area
	long ascent;			// Ascent for background area
	long descent;			// Descent for background area
};


struct xwsfmt_
{	long   ascent;			// Maximum ascent from baseline (positive)
	long   descent;			// Maximum descent from baseline (positive)
	long   hortab;
	long   lmargin;
	long   rmargin;			// Right margin X position
	long   lspace;			// Line spacing
	long   pspace;			// Paragraph spacing
	long   offset;
	ushort bits;
	ushort attr;
	ushort fnum;			// Font number
	ushort fattr;			// Font attributes
	ushort fsize;			// Font size
	ushort fwidth;			// Font width
	long   txtcol;			// Initial forground (text) color
	XWSBGC bgcol;			// Initial background color
};

// Define the XWSTL (Text Line) structure

struct xwstl_
{	ulong  label;			// Label (TL##)
	XWSTB *tb;				// Address of TB for block
	XWSTL *next;			// Address of next TL in block
	XWSTL *prev;
	long   xpos;			// X position of line (relative to text box)
	long   ypos;			// Y position of baseline of line (relative to
							//   text box)
	long   length;			// Length of line in pixels
	long   hlpos;
	long   hllen;
	XWSFMT fmt;
	ushort xxx;
	ushort lnum;
	ushort numchar;			// Number of characters in line
	ushort count;			// Number of bytes in line
	char   text[1];			// First data byte
};

// Define bits for tl_bits

#define TLB_MONO 0x10		// Line uses single mono-spaced font
#define TLB_WRAP 0x04		// Line has been word-wrapped
#define TLB_EOL  0x02		// Line terminated with EOL (NL) character
#define TLB_EOP  0x01		// Line terminated with EOP character

// Define bits for tl_fattr

#define TLA_XBOLD  0x1000	// Extra bold
#define TLA_BOLD   0x0800	// Bold
#define TLA_DBOLD  0x0400	// Demi-bold
#define TLA_NORMAL 0x0200
#define TLA_LIGHT  0x0100	// Light
#define TLA_STRIKE 0x0008	// Strike-out
#define TLA_DUNDER 0x0004	// Double underscore
#define TLA_UNDER  0x0002	// Underscore
#define TLA_ITALIC 0x0001	// Italic

// Define values for tl_lfmt

#define TFMT_LEFT  0		// Hard left
#define TFMT_RIGHT 1		// Hard right
#define TFMT_CEN   2		// Window centered
#define TFMT_JUST  3		// Justified

// Define offsets in the IB (image blck)

struct xwsib_
{	long       label;
	XWSIB     *next;
	XWSIB     *prev;
	long       inx;
	long       xpos;
	long       ypos;
	XWSBITMAP *bm;
};

// Define structure for the FNB (Font Name Block)
// There is one FNB for each font that is available for use. These FNBs are
//   searched when a font is requrested.

struct xwsfnb_
{	ulong   label;			// Label (FNB#)
	XWSFNB *next;			// Offset of next FNB
	XWSFSB *ffsb;			// First FSB for this FNB
	uchar   ftcmap;			// FreeType character map index
	char    fontname[1];	// Font name
};

// Define structure for the FSB (Font Size Block)
// FSBs are used (along with FNBs) to specify the available font sizes. For
//   bitmapped fonts there is one FSB for each available size. For scaled
//   fonts there is one FSB which specifies 0 for both height and width.

struct xwsfsb_
{   ulong   label;			// Label (FSB#)
    XWSFSB *next;			// Next FSB for same font
    XWSFNB *fnb;			// FNB
    XWSFNT *ffnt;			// First FNT
	int     szinx;			// Index used to select size in font files
							//   which contain multiple sizes
    ulong   height;			// Height of font (pixels)
    ulong   width;			// Average width of font (pixels)
	int     faceinx;		// FreeType2 face index
	char    filename[1];	// File name
};


// Define structure for the FNT (font object)
// An FNT is allocated for each font face that is loaded.

struct xwsfnt_
{	XWSFNTP;				// Publicly available part (see xosxws.h)
	XWSFNT *next;			// Pointer to next FNT from same font size
	XWSFNB *fnb;			// FNB for this font
	XWSFSB *fsb;			// FSB for this font
	FT_Face ftface;			// Corresponding FreeType2 Face object
	long    loadcnt;		// Load count
	XWSGCB *glyphtbl[128];	// XWSGCB hash table
};


// Define structure for the glyph cach block (GCB).
// These blocks are used to cache glyph definitions. If the glyph is smaller
//   than about 40x40 the bitmap is cached in this block. For larger glyphs
//   from scalable fonts the vector definition is cached. Larger glyphs from
//   bitmapped files are not cached.
// In any case, the actual data that is cached is the FreeType2 glyphslot
//   struture.

struct xwsgcb_
{	ulong    label;			// Label (GCB#)
	XWSGCB  *hash;			// Next GCB in the hash chain
	XWSGCB  *next;			// Next GCB in the management list
	XWSGCB  *prev;			// Previous GCB in the management list
	ulong    glyphinx;		// Glyph index in the font file
	XWSFNT  *fnt;			// FNT which own this GCB
	FT_Glyph ftglyph;		// Pointer to the FreeType2 glyphslot structure
};

extern XWSGCB xwsbadgcb;

extern XWSDRWDSP xwsmemdsp;

struct xwsbitmap_
{	ulong  label;			// Lable (BM##)
	ushort user;
	ushort xxx;
	long   status;			// Status bits
	long   usecnt;			// Use count
///	NOT   *fnot;			// First notify object for BITMAP

	XWSDRW drw;				// Used when drawing on this bitmap
	uchar  bitmap[1];		// Data array
};

// Define values for status in BITMAP

#define BMS_DONE 0x0001		// Bitmap is completely loaded

struct xwsptr_
{	long    label;			// Label (PTR#)
	long    hndl;			// Printer device handle
	long    mode;			// Print mode
	long    width;			// Current page width
	long    monseedsz;		// Size of the monochrome seed row buffer
	long    colseedsz;		// Size of the color seed row buffer
	long    outcnt;			// Available output buffer bytes
	long    bfrpos;			// Position in file of current buffer
	long    lenpos;			// Position in file of length value of current
							//   record
	long    reclen;			// Record length
	uchar  *lenpnt;			// Pointer to length value of current record
							//   in current buffer
	uchar  *monseedrow;		// Monochorme seed row buffer (1 bit per pixel)
	ushort *colseedrow;		// Color seed row buffer (2 bytes per pixel)
	uchar  *currow;			// Current row buffer (1 bit per pixel)
	uchar  *genrow;			// Row output buffer (mode 9 compression)
	uchar  *outbfr;			// Printer output buffer
	uchar  *outpnt;
	uchar   cmode;			// True if color mode valid
	uchar   mmode;			// True if monochrome mode valid
};

extern XWSFNB  *xwsfirstfnb;

extern TRMMODES xwstrmdata;

extern ushort   xwsscnnum;
extern ushort   xwstrmnum;
extern long     xwsvectbase;
extern XWSCUR  *xwscursorcur;
extern XWSDRW   xwsscreendrw;
extern XWSFNT  *xwsdftfnt;

extern long     xwsdrawtdb;

extern uchar    xwsnoscale;

void xwsupdatecursor(void);

#endif	// !defined(_XOSXXWS_H_)
