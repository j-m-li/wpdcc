#if !defined(_XOSXWS_H_)
#define _XOSXWS_H_

#define BASE(x) (&x->bas)

#pragma aux XWSCALL "*" parm [] reverse routine modify [EAX EBX ECX EDX] \
	value [EAX];

typedef struct xwsrgn_    XWSRGN;
typedef struct xwsrsb_    XWSRSB;
typedef struct xwsdrw_    XWSDRW;
typedef struct xwswin_    XWSWIN;
typedef struct xwslist_   XWSLIST;
typedef struct xwsedit_   XWSEDIT;
typedef struct xwsresp_   XWSRESP;
typedef struct xwscntr_   XWSCNTR;
typedef struct xwsbutn_   XWSBUTN;
typedef struct xwssabl_   XWSSABL;
typedef struct xwssbar_   XWSSBAR;
typedef struct xwsedb_    XWSEDB;
typedef struct xwsefte_   XWSEFTE;
typedef struct not_       XWSNOT;
typedef struct xwscur_    XWSCUR;
typedef union  xwswinpar_ XWSWINPAR;
typedef struct xwstxtpar_ XWSTXTPAR;
typedef struct xwsfnb_    XWSFNB;
typedef struct xwsfsb_    XWSFSB;
typedef struct xwsfnt_    XWSFNT;
typedef struct xwsgbm_    XWSGBM;
typedef struct xwsgcb_    XWSGCB;
typedef struct xwsptr_    XWSPTR;
typedef struct xwsrect_   XWSRECT;
typedef struct xwscoord_  XWSCOORD;
typedef struct xwsinsets_ XWSINSETS;
typedef struct xwsbitmap_ XWSBITMAP;
typedef struct xwsedblk_  XWSEDBLK;

typedef struct xwsbasepar_ XWSBASEPAR;
typedef struct xwssablpar_ XWSSABLPAR;
typedef struct xwscntrpar_ XWSCNTRPAR;
typedef struct xwslistpar_ XWSLISTPAR;
typedef struct xwseditpar_ XWSEDITPAR;
typedef struct xwsbutnpar_ XWSBUTNPAR;
typedef struct xwssbarpar_ XWSSBARPAR;
typedef struct xwsdpdnpar_ XWSDPDNPAR;
typedef struct xwsindcpar_ XWSINDCPAR;


extern XWSCNTR   *xws_ScreenWIN;
extern XWSDRW     xws_ScreenDRW;
extern XWSCNTRPAR xws_WinParContainerDefault;
extern XWSCNTRPAR xws_WinParContainerPlain;
extern XWSLISTPAR xws_WinParListDefault;
extern XWSBUTNPAR xws_WinParButtonDefault;
extern XWSEDITPAR xws_WinParEditDefault;
extern XWSSBARPAR xws_WinParScrollBarDefault;

extern XWSTXTPAR xws_TxtParContainerDefault;
extern XWSTXTPAR xws_TxtParButtonDefault;

extern long xws_TrmHndl;
extern long xws_SchedReq;

extern XWSCUR *xws_CurFinger;
extern XWSCUR *xws_CurPoint;
extern XWSCUR *xws_CurMove;
extern XWSCUR *xws_CurSizeNS;
extern XWSCUR *xws_CurSizeEW;
extern XWSCUR *xws_CurSizeNESW;
extern XWSCUR *xws_CurSizeNWSE;

extern long xws_TotalMem; 
extern long xws_TotalBlk;
extern long xws_AllocCnt;
extern long xws_FreeCnt;


typedef void XWSDRAW(XWSWIN *win);
#pragma aux (XWSCALL) XWSDRAW;

typedef long XWSEVENT(XWSEDBLK *edblk);
#pragma aux (XWSCALL) XWSEVENT;

typedef void XWSTIMER(long hndl, long data);
#pragma aux (XWSCALL) XWSTIMER;


void *xwsMalloc(int size);
#pragma aux (XWSCALL) xwsMalloc;

void *xwsCMalloc(int size);
#pragma aux (XWSCALL) xwsCMalloc;

void xwsFree(void *addr);
#pragma aux (XWSCALL) xwsFree;

long    xwsBitmapCopyScaled(XWSWIN *win, XWSBITMAP *obm, long width,
		long height, long flags, XWSBITMAP **sbm);
#pragma aux (XWSCALL) xwsBitmapCopyScaled;

void    xwsBitmapDestroy(XWSBITMAP *bitmap);
#pragma aux (XWSCALL) xwsBitmapDestroy;

void    xwsBitmapGetDim(XWSWIN *win, XWSBITMAP *bitmap, XWSCOORD *size);
#pragma aux (XWSCALL) xwsBitmapGetDim;

long    xwsBitmapLoad(char *file, XWSWIN *win, long bits, XWSBITMAP **bm);
#pragma aux (XWSCALL) xwsBitmapLoad;

void    xwsCtlMain(void);
#pragma aux (XWSCALL) xwsCtlMain;

void    xwsCtlUpdate(void);
#pragma aux (XWSCALL) xwsCtlUpdate;

XWSCUR *xwsCurCreate(long xsize, long ysize, long xhot, long yhot, long *colors,
		long num ,uchar *patbm, uchar *maskbm);
#pragma aux (XWSCALL) xwsCurCreate;

void    xwsCurSetPos(long xpos, long ypos);
#pragma aux (XWSCALL) xwsCurSetPos;

void    xwsDrawBitmap(XWSWIN *win, XWSBITMAP *bitmap, long xpos, long ypos,
		long xrept, long yrept, long func);
#pragma aux (XWSCALL) xwsDrawBitmap;

long    xwsDrawChar(XWSWIN *win, long chr, long xpos, long ypos, XWSFNT *font,
		long color);
#pragma aux (XWSCALL) xwsDrawChar;

void    xwsDrawCircle(XWSWIN *win, long xpos, long ypos, long radius,
		long color);
#pragma aux (XWSCALL) xwsDrawCircle;

long    xwsDrawLine(XWSWIN *win, long x1, long y1, long x2, long y2, long width,
		long color);
#pragma aux (XWSCALL) xwsDrawLine;

long    xwsDrawRect(XWSWIN *win, long xpos, long ypos, long xsize, long ysize,
		long linewid, long color);
#pragma aux (XWSCALL) xwsDrawRect;

long    xwsDrawText(XWSWIN *win, char *text, long length, long xpos, long ypos,
		XWSFNT *font, long color);
#pragma aux (XWSCALL) xwsDrawText;

long    xwsDrawTextCentered(XWSWIN *win, char *text, long length, long xpos,
		long ypos, XWSFNT *font, long color);
#pragma aux (XWSCALL) xwsDrawTextCentered;

void    xwsFatal(long code, char *msg);
#pragma aux (XWSCALL) xwsFatal;

long    xwsFillRect(XWSWIN *win, long xpos, long ypos, long xsize, long ysize,
		long color);
#pragma aux (XWSCALL) xwsFillRect;

long    xwsFontGetLength(XWSFNT *fnt, XWSWIN *win, char *text, long cnt);
#pragma aux (XWSCALL) xwsFontGetLength;

XWSFNT *xwsFontLoad(char *name, XWSWIN *win, long height, long width,
		long attr);
#pragma aux (XWSCALL) xwsFontLoad;

void   xwsFontUnload(XWSFNT *fnt);
#pragma aux (XWSCALL) xwsFontUnload;

//@@@ CEY (pragma was for xwsInitS3A!!!)
void   xwsInitATIB(void);
#pragma aux (XWSCALL) xwsInitATIB;

int    xwsInitVesaA(void);
#pragma aux (XWSCALL) xwsInitVesaA;

void   xwsInitBegin(char  *trmname, long maxthrds, uchar *thrdbase,
		long vectbase, long horz, long vert);
#pragma aux (XWSCALL) xwsInitBegin;

void   xwsInitCursor(void);
#pragma aux (XWSCALL) xwsInitCursor;

int    xwsInitLCDA(void);
#pragma aux (XWSCALL) xwsInitLCDA;

void   xwsInitMouse(void);
#pragma aux (XWSCALL) xwsInitMouse;

int    xwsInitS3A(void);
#pragma aux (XWSCALL) xwsInitS3A;

void   xwsInitScreen(void);
#pragma aux (XWSCALL) xwsInitScreen;

void   xwsInitStart(XWSCNTRPAR *parms, XWSEVENT event, long evmask,
		XWSEDB *edb);
#pragma aux (XWSCALL) xwsInitStart;

long   xwsPrintBegin(char *prtname, char *filespec, char *jobname, int cpycnt,
		int mode, XWSPTR **pptr);
#pragma aux (XWSCALL) xwsPrintBegin;

long   xwsPrintEnd(XWSPTR *ptr);
#pragma aux (XWSCALL) xwsPrintEnd;

long   xwsPrintPage(XWSPTR *ptr, XWSCNTR *memwin);
#pragma aux (XWSCALL) xwsPrintPage;

long   xwsTimerStart(XWSTIMER func, long data, llong time);
#pragma aux (XWSCALL) xwsTimerStart;

long   xwsTimerStop(long handle);
#pragma aux (XWSCALL) xwsTimerStop;

XWSRGN *xwsRgnCreateRect(XWSWIN *win, long xpos, long ypos, long xsize,
		long ysize);
#pragma aux (XWSCALL) xwsRgnCreateRect;

void   xwsRgnDestroy(XWSRGN *rgn);
#pragma aux (XWSCALL) xwsRgnDestroy;

long   xwsWinBitmapFromBM(XWSWIN *win, long index, long xpos, long ypos,
		long width, long height, long attr, XWSBITMAP *bm);
#pragma aux (XWSCALL) xwsWinBitmapFromBM;

long   xwsWinBitmapFromFile(XWSWIN *win, long index, long xpos, long ypos,
		long width, long height, long attr, char *filename);
#pragma aux (XWSCALL) xwsWinBitmapFromFile;

long   xwsWinBitmapRemove(XWSWIN *win, long index);
#pragma aux (XWSCALL) xwsWinBitmapRemove;

void   xwsWinClear(XWSWIN *win);
#pragma aux (XWSCALL) xwsWinClear;

long   xwsWinCreateButton(XWSCNTR *parent, long xpos, long ypos,
		long xsize, long ysize, XWSBUTNPAR *winparms, XWSTXTPAR *txtparms,
		XWSEVENT event, long evmask, XWSEDB *edb, XWSBUTN **pwin, char *text,
		long length);
#pragma aux (XWSCALL) xwsWinCreateButton;

long   xwsWinCreateContainer(XWSCNTR *parent, long xpos, long ypos,
		long xsize, long ysize, XWSCNTRPAR *parms, XWSEVENT event,
		long evmask, XWSEDB *edb, XWSCNTR **pwin);
#pragma aux (XWSCALL) xwsWinCreateContainer;

long   xwsWinCreateEdit(XWSCNTR *parent, long xpos, long ypos, long xsize,
	    long ysize, XWSEDITPAR *winparms, XWSTXTPAR *txtparms, XWSEVENT event,
		long evmask, XWSEDB *edb, XWSEDIT **pwin, char *text, long length);
#pragma aux (XWSCALL) xwsWinCreateEdit;

long   xwsWinCreateList(XWSCNTR *parent, long xpos, long ypos, long xsize,
	    long ysize, XWSLISTPAR *parms, XWSEVENT event, long evmask, XWSEDB *edb,
		XWSLIST **win);
#pragma aux (XWSCALL) xwsWinCreateList;

long   xwsWinCreateResponse(XWSCNTR *parent, long xpos, long ypos, long xsize,
		long ysize, XWSEDITPAR *winparms, XWSTXTPAR *txtparms, XWSEVENT event,
		long evmask, XWSEDB *edb, XWSRESP **pwin, char *text, long length);
#pragma aux (XWSCALL) xwsWinCreateResponse;

long   xwsWinCreateScrollBar(XWSCNTR *parent, long xpos, long ypos, long width,
		long height, XWSSBARPAR *parms, XWSEVENT *event, long evmask,
		XWSEDB *edb, XWSSBAR **pwin);

void   xwsWinDestroy(XWSWIN *win);
#pragma aux (XWSCALL) xwsWinDestroy;

void   xwsWinDispBitmap(XWSWIN *win);
#pragma aux (XWSCALL) xwsWinDispBitmap;

void   xwsWinDispText(XWSWIN *win);
#pragma aux (XWSCALL) xwsWinDispText;

void   xwsWinGetBndBox(XWSWIN *win, XWSRECT *rect);
#pragma aux (XWSCALL) xwsWinGetBndBox;

XWSRGN *xwsWinGetBrdRgn(XWSWIN *win);
#pragma aux (XWSCALL) xwsWinGetBrdRgn;

uchar *xwsWinGetBuffer(XWSWIN *win);
#pragma aux (XWSCALL) xwsWinGetBuffer;

void   xwsWinGetClient(XWSWIN *win, XWSRECT *rect);
#pragma aux (XWSCALL) xwsWinGetClient;

void   xwsWinGetInsets(XWSWIN *win, XWSINSETS *insets);
#pragma aux (XWSCALL) xwsWinGetInsets;

void   xwsWinGetOrigin(XWSWIN *win, long *data);
#pragma aux (XWSCALL) xwsWinGetOrigin;

long   xwsWinGetSelItem(XWSLIST *win);
#pragma aux (XWSCALL) xwsWinGetSelItem;

void  *xwsWinGetUser(XWSWIN *win);
#pragma aux (XWSCALL) xwsWinGetUser;

void   xwsWinInvalidateAll(XWSWIN *win);
#pragma aux (XWSCALL) xwsWinInvalidateAll;

void   xwsWinInvalidateRect(XWSWIN *win, long xpos, long ypos, long width,
		long height);
#pragma aux (XWSCALL) xwsWinInvalidateRect;

void   xwsWinInvalidateRgn(XWSWIN *win, XWSRGN *rgn);
#pragma aux (XWSCALL) xwsWinInvalidateRgn;

long   xwsWinCreateMemory(long width, long height, long type, XWSCNTRPAR *parms,
		XWSEVENT event, long evmask, XWSEDB *edb, XWSCNTR **pwin);
#pragma aux (XWSCALL) xwsWinCreateMemory;

void   xwsWinMove(XWSWIN *win, long xpos, long ypos, long width, long height);
#pragma aux (XWSCALL) xwsWinMove;

void   xwsWinDrawBegin(XWSWIN *win);
#pragma aux (XWSCALL) xwsWinDrawBegin;

void   xwsWinDrawEnd(XWSWIN *win);
#pragma aux (XWSCALL) xwsWinDrawEnd;

long   xwsWinListSetSelItem(XWSLIST *win, long inx);
#pragma aux (XWSCALL) xwsWinListSetSelItem;

long   xwsWinListGetSelItem(XWSLIST *win);
#pragma aux (XWSCALL) xwsWinListGetSelItem;

long   xwsWinListPutItemAtTop(XWSLIST *win, long inx);
#pragma aux (XWSCALL) xwsWinListPutItemAtTop;

long   xwsWinListPutItemInView(XWSLIST *win, long inx);
#pragma aux (XWSCALL) xwsWinListPutItemInView;

void   xwsWinListScroll(XWSLIST *win, long amnt);
#pragma aux (XWSCALL) xwsWinListScroll;

long   xwsWinSaveAsBMP(XWSCNTR *win, char *spec);
#pragma aux (XWSCALL) xwsWinSaveAsBMP;

long xwsWinScrollBarSet(XWSSBAR *win, long rage, long tmbpos, long tmbsize);
#pragma aux (XWSCALL) xwsWinScrollBarSet;

void   xwsWinSetBgBitmap(XWSWIN *win, XWSBITMAP *bm, int redraw);
#pragma aux (XWSCALL) xwsWinSetBgBitmap;

void   xwsWinSetBgColor(XWSWIN *win, long bgcol, int redraw);
#pragma aux (XWSCALL) xwsWinSetBgColor;

void   xwsWinSetBrdColor(XWSWIN *win, long fgcol);
#pragma aux (XWSCALL) xwsWinSetBrdColor;

long   xwsWinSetFocus(XWSWIN * win, int mode);
#pragma aux (XWSCALL) xwsWinSetFocus;

void   xwsWinSetOriginAbs(XWSWIN *win, long xorg, long yorg);
#pragma aux (XWSCALL) xwsWinSetOriginAbs;

void   xwsWinSetOriginRel(XWSWIN *win, long xorg, long yorg);
#pragma aux (XWSCALL) xwsWinSetOriginRel;

void   xwsWinSetScale(XWSWIN *win, long xsfd, long ysfd);
#pragma aux (XWSCALL) xwsWinSetScale;

void   xwsWinSetSelItem(XWSLIST *win, long item);
#pragma aux (XWSCALL) xwsWinSetSelItem;

void   xwsWinSetUser(XWSWIN *win, void *user);
#pragma aux (XWSCALL) xwsWinSetUser;

void   xwsWinScrollRgn(XWSWIN *win, XWSRGN *rgn, long xdist, long ydist);
#pragma aux (XWSCALL) xwsWinScrollRgn;

void   xwsWinTextAdd(XWSWIN *win, long inx, char *text, long count);
#pragma aux (XWSCALL) xwsWinTextAdd;

long   xwsWinTextGet(XWSWIN *win, long inx, long mode, char *bufr, long len);		// Length of the buffer. 0 indicates no buffer.
#pragma aux (XWSCALL) xwsWinTextGet;

void   xwsWinTextGetBBox(XWSWIN *win, long inx, XWSRECT *bbox);
#pragma aux (XWSCALL) xwsWinTextGetBBox;

void   xwsWinTextIns(XWSWIN *win, long inx, char *text, long count, long line,
		long chr);
#pragma aux (XWSCALL) xwsWinTextIns;

long   xwsWinTextNew(XWSWIN *win, long inx, long xpos, long ypos, long width,
		long height, XWSTXTPAR *parms, char *text, long count);
#pragma aux (XWSCALL) xwsWinTextNew;

void   xwsWinTextRemove(XWSWIN *win, long inx);
#pragma aux (XWSCALL) xwsWinTextRemove;

// Define structure for the EDB (Environment Data Block)

struct xwsedb_
{	long   numfonts;	// Number of fonts in the font table
	char **fonttbl;		// Offset of the font table
	long   numcols;		// Number of colors in the color table
	long  *coltbl;		// Offset of the color table
};

// Define structure for the EFTE (Environment Font Table Entry)

struct efte
{	char   *name;		// Name of the font
	XWSFNT *fnt;		// XWSFNT object for the font
};

#if !defined(_XOSXXWS_H_)

struct xwswin_
{	long xxx;
};

struct xwscntr_
{	XWSWIN bas;
};

struct xwsbutn_
{	XWSWIN bas;
};

struct xwsedit_
{	XWSWIN bas;
};

struct xwslist_
{	XWSWIN bas;
};

struct xwssbar_
{	XWSWIN bas;
};

#endif

// Define the XWSxxxPAR structures which are used to specify parameters when
//   creating windows

struct xwsbasepar_
{	long       blklen;
	long       zorder;		// Z-order value
	long       bgcoln;		// Normal background color
	long       bdcol;		// Border color
	long       hlcoln;		// Normal highlight color
	long       shcoln;		// Nornal shadow color
	long       bdwido;		// Outer border width
	long       bdwidc;		// Center border width
	long       bdwidi;		// Inner border width
	long       bits1;		// Window bits 1
	long       bits2;		// Window bits 2
	long       bits3;		// Window bits 3
	long       bits4;		// Window bits 4
	long       xradius;		// X radius
	long       yradius;		// Y radius
	long       xsfd;		// X scale factor divisor
	long       ysfd;		// Y scale factor divisor
	ushort     orient;		// Orientation
	ushort     format;		// Format
	XWSCUR    *cursor;		// Offset of CUR for cursor for window
	XWSBITMAP *bgbmn;		// Normal background bitmap
	XWSBITMAP *brdbm;		// Border bitmap
	XWSDRAW   *draw;		// Draw function
	long       evarg1;		// First event argument
	void      *evarg2;		// Second event argument
};


typedef struct xwssablpar_
{	long       sbsize;		// Size
	long       sbbdwid;		// Border width
	long       sbhlcoln;	// Normal highlight color for scroll bar
	long       sbshcoln;	// Normal shadow color for scroll bar
	long       sbbarcol;	// Bar color
	long       sbbtnsize;	// Button size
	long       sbbtnbdwid;	// Button border width
	long       sbbtnbgcoln;	// Button normal background color
	long       sbbtnbgcola;	// Button alternate background color
	long       sbbtntxcoln;	// Button normal text color
	long       sbbtntxcola;	// Button alternate text
	long       sbbtnhlcoln;	// Normal button and thumb highlight color
	long       sbbtnhlcola;	// Alternate button and thumb highlight color
	long       sbbtnshcoln;	// Normal button and thumb shadow color
	long       sbbtnshcola;	// Alternate button and thumb shadow color
	long       sbbtnshift;	// Button pressed offset
	XWSBITMAP *sbbtn1bmn;	// Normal bit map for button 1
	XWSBITMAP *sbbtn1bma;	// Alermnate bit map for button 1
	XWSBITMAP *sbbtn2bmn;	// Normal bit map for button 2
	XWSBITMAP *sbbtn2bma;	// Alternate bit map for button 2
	long       sbtmbbdwid;	// Thumb border width
	long       sbtmbbgcoln;	// Thumb normal background color
	long       sbtmbbgcola;	// Thumb alternate background color
};


typedef struct xwssimplepar_
{	XWSBASEPAR;
} XWSSIMPLEPAR;


struct xwscntrpar_
{	XWSBASEPAR;
	XWSSABLPAR;
	long titlesize;			// Title bar size
	long titlebits;			// Title bar bits
	long titletext;			// Title bar text
};


struct xwslistpar_
{	XWSBASEPAR;
	XWSSABLPAR;
	long bgcols;			// Background color for selected items
};


struct xwsbutnpar_
{	XWSBASEPAR;
	long       bgcola;		// Alternate background color
	long       hlcola;		// Alternate highlight color
	long       shcola;		// Alternate shadow color
	long       shift;		// Pressed button offset
	XWSBITMAP *bgbma;		// Alternate background bitmap
};


struct xwseditpar_
{	XWSBASEPAR;
	XWSSABLPAR;
	long titlesize;			// Title bar size
	long titlebits;			// Title bar bits
	long titletext;			// Title bar text
};


struct xwsindcpar_
{	XWSBASEPAR;
	ushort inc1size;		// Increment 1 button size
	ushort inc2size;		// Increment 2 button size
	ushort digits;			// Number of digits
	ushort xxx;
	long   maxvalue;		// Maximum value
	long   minvalue;		// Minimum value
	long   inc1value;		// Increment 1 value
	long   inc2value;		// INcrement 2 value
};

struct xwssbarpar_
{	XWSBASEPAR;
	long       delayi;		// Initial delay before repeating
	long       delayr;		// Repeat delay
	long       tmbrange;	// Thumb range
	long       tmbsize;		// Thumb size
	long       tmbpos;		// Thumb position
	long       tmbbgcoln;	// Thumb normal background color
	long       tmbbgcola;	// Thumb alternate background color
	long       btnsize;		// Button size
	long       btnbgcola;	// Alternate button background color
	long       btntxcoln;	// Normal button text color
	long       btntxcola;	// Alternate button text color
	long       btnhlcoln;	// Normal button highlight color
	long       btnhlcola;	// Alternate button highlight color
	long       btnshcoln;	// Normal button shadow color
	long       btnshcola;	// Alternate button shadow color
	XWSBITMAP *btn1bmn;		// Normal button 1 background bitmap
	XWSBITMAP *btn1bma;		// Alternate button 1 background bitmap
	XWSBITMAP *btn2bmn;		// Normal button 2 background bitmap
	XWSBITMAP *btn2bma;		// Alternate button 2 background bitmap
	long       btnshift;	// Pressed button offset
};


union xwswinpar_
{	XWSCNTRPAR cntr;
	XWSLISTPAR list;
	XWSBUTNPAR butn;
	XWSEDITPAR edit;
	XWSSBARPAR sbar;
};


// Define the XWSTXTPAR structure which is used to specify parameters when
//   creating text boxes

struct xwstxtpar_
{	long   blklen;

	long   right;			// X position of right edge of text box
							//   (returned)
	long   bottom;			// Y postioln of bottom edge of text box
							//   (returned)
	ushort height;			// Height
	ushort width;			// Width
	ushort fontnum;			// Font number
	ushort attr;			// Attributes
	long   bgcolor;			// Background color
	long   nrmcolor;		// Normal text color
	long   altcolor;		// Alternate text color
	long   bghlcolor;		// Background highlight color
	long   hlcolor;			// Highlight color
	long   caretcolor;		// Caret color
	long   leftmargin;		// Left margin
	long   rightmargin;		// Right margin
	long   topmargin;		// Top margin
	long   btmmargin;		// Bottom margin
	long   lspace;			// leading
	long   pspace;			// Paragraph spacing
};

// Define special color values

#define XWS_COLOR_NONE   0xC0000000
#define XWS_COLOR_WINDOW 0xD0000000	// Use window background color as text
									//   background
#define XWS_COLOR_TRANS  0xE0000000	// Text background is transparent

// Define event types

#define XWS_EVENT_MOUSEMV  1		// Mouse movement
#define XWS_EVENT_MOUSEBT  2		// Mouse button
#define XWS_EVENT_KEYMAKE  3		// Key pressed event
#define XWS_EVENT_KEYRELS  4		// Key released event
#define XWS_EVENT_MOVE     8		// Window has been moved
#define XWS_EVENT_RESIZE   9		// Window size has been changed
#define XWS_EVENT_DESTROY  10		// Window is about to be destroyed
#define XWS_EVENT_WINDOW   11		// Window type specific event
#define XWS_EVENT_BMLOAD   12		// Bitmap load update

// Define event mask bits

#define XWS_EVMASK_MOUSEMV  0x00000002 // Mouse movement
#define XWS_EVMASK_MOUSEBT  0x00000004 // Mouse button
#define XWS_EVMASK_KEYMAKE  0x00000008 // Key pressed event
#define XWS_EVMASK_KEYRELS  0x00000010 // Key released event
#define XWS_EVMASK_MOVE     0x00000100 // Window has been moved
#define XWS_EVMASK_RESIZE   0x00000200 // Window size has been changed
#define XWS_EVMASK_DESTROY  0x00000400 // Window is about to be destroyed
#define XWS_EVMASK_WINDOW   0x00000800 // Child window event
#define XWS_EVMASK_BMLOAD   0x00001000 // Bitmap load update

// Define event callback return values

#define XWS_EVRTN_DONE 0
#define XWS_EVRTN_PASS 1
#define XWS_EVRTN_SYS  2

// Define the common part of the EDBLK

struct xwsedblk_
{	long    label;				// Label ('EDB#')
	ushort  length;				// Total length of block
	uchar   type;				// Event type
	uchar   subtype;			// Event subtype
	XWSWIN *orgwin;				// Originating WIN
	XWSWIN *dlvwin;				// Delivering WIN
	long    arg1;				// First user argument
	void   *arg2;				// Second user argument
};	

// Define sub-type values for EVENT_MOUSEBT events

#define XWS_EVENT_MBTN_WHL 0	// Wheel increment only (a wheel increment can
								//   also be specified with a button change)
#define XWS_EVENT_MBTN_LDN 1	// Left mouse button pressed
#define XWS_EVENT_MBTN_LUP 2	// Left mouse button released
#define XWS_EVENT_MBTN_CDN 3	// Center mouse button pressed
#define XWS_EVENT_MBTN_CUP 4	// Center mouse button released
#define XWS_EVENT_MBTN_RDN 5	// Right mouse button pressed
#define XWS_EVENT_MBTN_RUP 6	// Right mouse button released

// Define bits for the buttons item for EVENT_MOUSEBT events

#define XWS_EVENT_MBTN_C 0x04
#define XWS_EVENT_MBTN_R 0x02
#define XWS_EVENT_MBTN_L 0x01

// Define EDBLK structure for EVENT_MOUSEBT and EVENT_MOUSEMV events

typedef struct
{	XWSEDBLK;
	long   buttons;				// Button states
	long   xpos;				// X position
	long   ypos;				// Y position
	long   winc;				// Wheel increment
	time_s dt;					// Date/time
} XWSEDBLK_MOUSE;

// Define EDBLK structure for EVENT_KEYMAKE and EVENT_KEYRELS events

typedef struct
{	XWSEDBLK;
	long charcd;				// Character code (high 24 bits 0 if ASCII)
	long scancd;				// Scan code
	long kbsts;					// Keyboard status bits
} XWSEDBLK_KEYBRD;

// Define bits for kbsts in EDBLK_KEYBRD

#define XWS_EVENT_KBS_CAPKEY 0x00004000	// Caps-lock key pressed for char
#define XWS_EVENT_KBS_NUMKEY 0x00002000	// Num-lock key pressed for char
#define XWS_EVENT_KBS_SCLKEY 0x00001000	// Scroll-lock key pressed for char
#define XWS_EVENT_KBS_CAPLCK 0x00000400	// Caps-lock state true for char
#define XWS_EVENT_KBS_NUMLCK 0x00000200	// Num-lock state true for char
#define XWS_EVENT_KBS_SCLLCK 0x00000100	// Scroll-lock state true for char
#define XWS_EVENT_KBS_LFTALT 0x00000040	// Left ALT key pressed for char
#define XWS_EVENT_KBS_LFTCTL 0x00000020	// Left control key pressed for char
#define XWS_EVENT_KBS_LFTSHF 0x00000010	// Left shift key pressed for char
#define XWS_EVENT_KBS_RHTALT 0x00000004	// Right ALT key pressed for character
#define XWS_EVENT_KBS_RHTCTL 0x00000002	// Right control key pressed for char
#define XWS_EVENT_KBS_RHTSHF 0x00000001	// Right shift key pressed for char

// Define sub-type values for XWS_EVENT_WINDOW events for button windows

#define XWS_EVENT_WIN_BTN_PRESS 1 // Button has been "pressed"

// Define sub-type values for XWS_EVENT_WINDOW events for list windows

#define XWS_EVENT_WIN_LIST_SEL 3 // List item selected
#define XWS_EVENT_WIN_LIST_ENT 4 // List item entered
#define XWS_EVENT_WIN_LIST_REL 5 // List item released
#define XWS_EVENT_WIN_LIST_DBL 6 // List item double clicked
#define XWS_EVENT_WIN_LIST_CHG 7 // List selection changed

// Define EDBLK structure for XWS_EVENT_WINDOW events for list windows

typedef struct
{	XWSEDBLK;
	long    index;			// Item index
} XWSEDBLK_LIST;

// Define sub-type values for XWS_EVENT_WINDOW events for scrollbar windows

#define XWS_EVENT_WIN_SB_BTN1  10 // Button 1 has been "pressed"
#define XWS_EVENT_WIN_SB_BAR1  11 // Bar area 1 has been "pressed"
#define XWS_EVENT_WIN_SB_THUMB 12 // Thumb has been dragged
#define XWS_EVENT_WIN_SB_BAR2  13 // Bar area 2 has been "pressed"
#define XWS_EVENT_WIN_SB_BTN2  14 // Button 2 has been "pressed"

// Define EDBLK structure for XWS_EVENT_WINDOW events for scrollbar windows
//   (only used with subthpe XWS_EVENT_WIN_SB_THUMB)

typedef struct
{	XWSEDBLK;
	long tmbpos;
} XWSEDBLK_SCROLLBAR;

// Emd pf evemt definitions

// Define the RECT structure which is used to specify the position and size
//   of a rectangle

struct xwsrect_
{	long  xpos;
	long  ypos;
	long  width;
	long  height;
};

struct xwscoord_
{	long xval;
	long yval;
};

// Define the INSETS structure which is used to specify the widths of window
//   borders

struct xwsinsets
{	long  left;
	long  right;
	long  top;
	long  bottom;
};

// Define publicly available part of the XWSFNT object. These values are
//   READ-ONLY. They MUST NOT be changed by the user. They are provided
//   mainly to provide efficient access to the font metrics.

typedef struct
{	ulong  label;				// Label (FNT#)
	long   user;
	long   width;				// Width (pixels)
	long   height;				// Height (pixels)
	long   ascent;				// Ascent (pixels)
	long   descent;				// Descent (pixels)
	long   extra;				// Extra ascent (pixels)
	long   lnspace;				// Line spaceing (pixels)
	long   maxwidth;			// Maximum width (pixels)
	long   attr;				// Attributes
	long   underpos;			// Underscore position
	long   undersize;			// Underscore size
	long   strikepos;			// Strike-out position
	long   strikesize;			// Strike-out size
	int    flags;				// Flag bits:
								//   Bit 0: 1 if italic font
								//   Bit 1: 1 if monospaced font
								//   Other bits are reserved and must be 0
} XWSFNTP;

// Define bits in attr in FNTP

#define FNTA_XBOLD  0x1000	// Extra bold
#define FNTA_BOLD   0x0800	// Bold
#define FNTA_DBOLD  0x0400	// Demi-bold
#define FNTA_NORMAL 0x0200	// Normal
#define FNTA_LIGHT  0x0100	// Light
#define FNTA_ITALIC 0x0001	// Italic

// Define bits in flags in FNTP

#define FNTF_MONO   0x0004
#define FNTF_OBLICK 0x0002
#define FNTF_ITALIC 0x0001

// Define values for mode in PTR

#define PTRMODE_MONO  0
#define PTRMODE_COLOR 1
#define PTRMODE_BOTH  2

// Define spool file record types

#define PTRRECTYPE_HEADER  0x0F0FF0F0
#define PTRRECTYPE_DATA    0x0C0FF0F0
#define PTRRECTYPE_TRAILER 0x0x0FF0F0

// Define bits for the "bits" argument for text boxes

#define TA_NOSELECT 0x0200	// List window item cannot be selected (list
							//   windows only)
#define TA_SINGLE   0x0100	// Single line (edit widnows only)
#define TA_HEIGHT   0x0080	// Height was specified (internal use only)
#define TA_RIGHT    0x0020	// Text is right justified
#define TA_BOTTOM   0x0010	// Text is at the bottom of the box
#define TA_HORCEN   0x0008	// Text is centered horizontally
#define TA_VERCEN   0x0004	// Text is centered vertically
#define TA_FIXMAR   0x0002	// Right margin is fixed
#define TA_WORDWRAP 0x0001	// Word wrap enabled

// Define bits used when drawing lines

#define LINE_TOLEFT  0x40000000	// Draw line to left of position specified
#define LINE_TORIGHT 0x20000000	// Draw line to right of position specified
#define LINE_ABOVE   0x10000000	// Draw line above position specified
#define LINE_BELOW   0x08000000	// Draw line below position specified

// Define bits for the flags argument for xwsBitmapCopyScaled

#define BMCS_KEEPAR 0x0001	// Retain aspect ratio when scaling

#endif	// !defined(_XOSXWS_H_)
