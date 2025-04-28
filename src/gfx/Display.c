/* "$Id: $"
 *
 *                          5 december MMIX PUBLIC DOMAIN
 *             The author disclaims copyright to this source code.
 *
 *          s = (a + b + c) / 2.0;
 *          A = sqrt(s * (s - a) * (s - b) * (s - c));    // Heron of Alexandria
 */


#include <gfx/Display.h>
#include <gfx/IRaster.h>
#include <gfx/IEventHandler.h>

#include <windows.h>
#include <math.h>
#include <GL/gl.h> 

#define    PixelFormat32bppARGB       (10 | (32 << 8) | PixelFormatAlpha | PixelFormatGDI | PixelFormatCanonical)
#define    PixelFormatIndexed      0x00010000 // Indexes into a palette
#define    PixelFormatGDI          0x00020000 // Is a GDI-supported format
#define    PixelFormatAlpha        0x00040000 // Has an alpha component
#define    PixelFormatPAlpha       0x00080000 // Pre-multiplied alpha
#define    PixelFormatExtended     0x00100000 // Extended color 16 bits/channel
#define    PixelFormatCanonical    0x00200000 

int32 __stdcall
GdipCreateBitmapFromScan0(int32 width,
                          int32 height,
                          int32 stride,
                          int32 format,
                          BYTE* scan0,
                          void** bitmap);

#define FLAGS_SWAPPED 1

void *gfx__Display__h_instance = 0;

static gfx__IRaster__class gfx__IRaster__;


static void __init__(Ex **e__)
{
	(void)e__;
	gfx__Display__.super = &Object__;
	INIT_INTERFACE(gfx__IRaster, gfx__Display);

	return;
}

static void __ctor__(Ex **e__, gfx__Display *self)
{
	F("__ctor__");
	
	self->x = -1;
	self->y = -1;
	self->w = 300;
	self->h = 150;
	self->line_size = self->w * 4;
	if ((self->line_size & 0x07) != 0) {
		//self->line_size += 8 - (self->line_size & 0x07);
	}
	self->rgba = Gc__malloc(e__, self->line_size * self->h);
	CHECK(error1);
	self->gh_dc = 0;
	self->gh_rc = 0;
	self->gh_wnd = 0;
	self->e__ = e__;
	self->root.__self__ = 0;
	self->event = new_(gfx__Event);
	CHECK(error2);
	self->canvas = new_(gfx__Canvas);
	CHECK(error3);
	self->mask = Gc__malloc(e__, self->w);
	CHECK(error4);
	return;
error4:
	delete_(self->canvas);
error3:
	delete_(self->event);
error2:
	Gc__free(e__, self->rgba);	
error1:
	return;
}


static void destroy_the_window(Ex **e__, gfx__Display *self)
{
	(void)e__;
	if (self->gh_rc) { 
		wglDeleteContext(self->gh_rc);
	} 
	if (self->gh_dc) {
		ReleaseDC(self->gh_wnd, self->gh_dc);
	} 
	self->gh_rc = 0; 
	self->gh_dc = 0; 
 
	if (self->gh_wnd) {
		DestroyWindow(self->gh_wnd);
	} 
	self->gh_wnd = 0;
}

static void __dtor__(Ex **e__, gfx__Display *self)
{
	F("__dtor__");

	Gc__free(e__, self->mask);
	CHECK(error5);
error5:
	destroy_the_window(e__, self);
	CHECK(error4);
error4:
	delete_(self->event);
	CHECK(error3);
error3:
	//delete_mask(e__, self, self->clipping_mask);
	CHECK(error2);
error2:
	Gc__free(e__, self->rgba);
	CHECK(error1);
error1:
	return;
}

static uint32 hash_code(Ex **e__, gfx__Display *self)
{
	(void)e__;
#ifdef _MSC_VER
	return *((uint32*)&self) >> 4;
#else 
	return (uint32)self >> 4;
#endif
} 

static void to_string(Ex **e__, gfx__Display *self, String *out)
{
	F("to_string");
	String s;

	STR(s, self->_->__name__);
	out->_->add(e__, out, &s);
	CHECK(error);
error:
	return;
}

static void size(Ex **e__, gfx__Display *self, int32 x, int32 y,
	int32 w, int32 h)
{
	F("size");

	if (x >= 0 && y >= 0) {
		self->x = x;
		self->y = y;
	}
	if (w > 0 && h > 0) {
		self->w = w;
		self->h = h;

		self->line_size = self->w * 4;
		Gc__free(e__, self->rgba);
		self->rgba = 0;
		Gc__free(e__, self->mask);
		self->mask = 0;
		CHECK(error1);
		self->rgba = Gc__malloc(e__, self->line_size * self->h);
		memset(self->rgba, 0, self->line_size * self->h);
		CHECK(error1);
		self->mask = Gc__malloc(e__, self->w);
		CHECK(error2);		
	}
	return;
error2:
	Gc__free(e__, self->rgba);
error1:
	return;
}

BOOL bSetupPixelFormat(HDC hdc) 
{ 
    PIXELFORMATDESCRIPTOR pfd, *ppfd; 
    int pixelformat; 
 
    ppfd = &pfd; 
 
    ppfd->nSize = sizeof(PIXELFORMATDESCRIPTOR); 
    ppfd->nVersion = 1; 
    ppfd->dwFlags = /*PFD_DRAW_TO_WINDOW | */PFD_SUPPORT_OPENGL /* |  
                        PFD_DOUBLEBUFFER*/; 
    ppfd->dwLayerMask = PFD_MAIN_PLANE; 
    ppfd->iPixelType = PFD_TYPE_RGBA; 
    ppfd->cColorBits = 24; 
    ppfd->cDepthBits = 0; 
    ppfd->cAccumBits = 0; 
    ppfd->cStencilBits = 0; 
 
   //pixelformat = ChoosePixelFormat(hdc, ppfd); 
 
    if ( (pixelformat = ChoosePixelFormat(hdc, ppfd)) == 0 ) 
    { 
        MessageBox(NULL, L"ChoosePixelFormat failed", L"Error", MB_OK); 
        return FALSE; 
    } 
 
    if (SetPixelFormat(hdc, pixelformat, ppfd) == FALSE) 
    { 
        MessageBox(NULL, L"SetPixelFormat failed", L"Error", MB_OK); 
        return FALSE; 
    } 
 
    return TRUE; 
} 

static void draw(Ex **e__, gfx__Display *self) 
{
	F("draw");

		ECHO("draw\r\n"); 

	memset(self->rgba, 0, self->line_size * self->h);

	if (self->root.__self__ != 0) {
		gfx__IRaster raster;
		self->_->gfx__i_raster(e__, self, &raster);
		self->event->canvas = self->canvas;
		self->canvas->_->set_context(e__, self->canvas, &raster,
			self->x, self->y, self->w, self->h);
		self->event->id = GFX__EVENT_DRAW;
		self->event->r.x = 0;
		self->event->r.y = 0;
		self->event->r.w = self->w;
		self->event->r.h = self->h;
		self->event->handled = 1;
		self->root._->event(e__, &self->root, self->event);
	}
	CHECK(error);

	wglMakeCurrent(self->gh_dc, self->gh_rc); 
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
	glDrawPixels(self->w, self->h, GL_RGBA, GL_UNSIGNED_BYTE, self->rgba);

	SwapBuffers(self->gh_dc); 
	return;
error:
	return;
}

static void char_event(Ex **e__, gfx__Display *self, 
	UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	//ToUnicode(
}

/* main window procedure */ 
LONG WINAPI MainWndProc ( 
    HWND    hWnd, 
    UINT    uMsg, 
    WPARAM  wParam, 
    LPARAM  lParam) 
{
	F("MainWndProc");
	Ex **e__ = 0; 
	gfx__Display *self = (void*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
	LONG    lRet = 1; 
	PAINTSTRUCT    ps;
	CREATESTRUCT *cs = (void*)lParam; 
	RECT rect; 
	char b[100];
	int8 update  = 0;

	Ex *e = 0;

	if (e__ == 0) e__ = &e;
	if (self != 0) e__ = self->e__;
	sprintf(b, "%x ", uMsg);
	ECHO(b);

	if (self != 0) {
		self->event->id = 0;
	}

	switch (uMsg) { 
	case WM_CREATE:
		self = cs->lpCreateParams; 
		e__ = self->e__;
		self->gh_dc = GetDC(hWnd); 
		if (!bSetupPixelFormat(self->gh_dc)) { 
			PostQuitMessage (0);
		}
		self->gh_rc = wglCreateContext(self->gh_dc);
		self->_->draw(e__, self);
		break;
	case WM_ACTIVATE:
		if (wParam != WA_INACTIVE) {
			update = 1;
		}
		break;
 /*case WM_ACTIVATEAPP:
    if (wParam)
    {
      ulong state = 0;
      if (GetAsyncKeyState(VK_CAPITAL)) state |= FL_CAPS_LOCK;
      if (GetAsyncKeyState(VK_NUMLOCK)) state |= FL_NUM_LOCK;
      if (GetAsyncKeyState(VK_SCROLL)) state |= FL_SCROLL_LOCK;
      if (GetAsyncKeyState(VK_CONTROL)&~1) state |= FL_CTRL;
      if (GetAsyncKeyState(VK_SHIFT)&~1) state |= FL_SHIFT;
      if (GetAsyncKeyState(VK_MENU)) state |= FL_ALT;
      if ((GetAsyncKeyState(VK_LWIN)|GetAsyncKeyState(VK_RWIN))&~1) state |= FL_META;
      Fl::e_state = state;
      return 0;
    }

*/
	case WM_SETFOCUS:
		break;
	case WM_KILLFOCUS:
		break;
	//case WM_SETCURSOR:
	//	break;
 	case WM_MOVE:  
		update = 1;
		break;
	case WM_MOUSEMOVE:
		break;
	case WM_PAINT:
		ECHO("paint\r\n"); 
		BeginPaint(hWnd, &ps); 	
		self->_->draw(e__, self);
		EndPaint(hWnd, &ps); 
		break; 
	case WM_SIZE: 
		GetClientRect(hWnd, &rect);
		self->_->size(e__, self, -1, -1, rect.right, rect.bottom); 
		CHECK(error);
		self->_->draw(e__, self);
		break; 
	case WM_CLOSE: 
		destroy_the_window(e__, self);
		break; 
	case WM_DESTROY: 
		if (self->gh_rc) {
			wglDeleteContext(self->gh_rc);
		} 
		if (self->gh_dc) { 
			ReleaseDC(hWnd, self->gh_dc); 
		}
		self->gh_rc = 0; 
		self->gh_dc = 0; 
		PostQuitMessage (0); 
		break;

	case WM_DEADCHAR:
	case WM_SYSDEADCHAR:
	case WM_CHAR:
	case WM_SYSCHAR:
		char_event(e__, self, uMsg, wParam, lParam);
		update = 1;
		break;
	default: 
		lRet = DefWindowProc (hWnd, uMsg, wParam, lParam); 
		break; 
	} 
 
	if (self && self->root.__self__ != 0) {
		self->root._->event(e__, &self->root, self->event);
	}

	if (update) {
		RedrawWindow(self->gh_wnd, 0, 0, 
			RDW_INTERNALPAINT |RDW_UPDATENOW);
	}	
	return lRet;
error:
	return lRet;
} 

static void show(Ex **e__, gfx__Display *self, uint8 state)
{
	F("show");
	MSG        msg; 
	WNDCLASS   wndclass; 
	WCHAR szAppName[]=L"WHOOW.ORG"; 
	BYTE XORmaskCursor[] = {0};
	BYTE ANDmaskCursor[] = {0xff};

	(void)__func__;
	(void)msg;

	self->e__ = e__;
	if (state == 0) {
		destroy_the_window(e__, self);
		return;
	} 
	/* Register the frame class */
	wndclass.style         = 0; 
	wndclass.lpfnWndProc   = (WNDPROC)MainWndProc; 
	wndclass.cbClsExtra    = 0; 
	wndclass.cbWndExtra    = 8; 
	wndclass.hInstance     = gfx__Display__h_instance; 
	wndclass.hIcon         = LoadIcon (gfx__Display__h_instance, szAppName); 
	wndclass.hCursor       = 
		CreateCursor(gfx__Display__h_instance,   // app. instance 
			0,                // horizontal position of hot spot 
			0,                 // vertical position of hot spot 
			1,                // cursor width 
			1,                // cursor height 
			ANDmaskCursor,     // AND mask 
			XORmaskCursor );   ; 
    wndclass.hbrBackground = (HBRUSH)(COLOR_WINDOW+1); 
    wndclass.lpszMenuName  = szAppName; 
    wndclass.lpszClassName = szAppName; 
 
    if (!RegisterClass (&wndclass) ) return; 
 
    /* Create the frame */ 
    self->gh_wnd = CreateWindow (szAppName, 
		L"Whoow...", 
		WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, 
		self->x >= 0 ? self->x : CW_USEDEFAULT, 
		self->y >= 0 ? self->y : CW_USEDEFAULT, 
		self->w, 
		self->h, 
		NULL, 
		NULL, 
		gfx__Display__h_instance, 
		self); 
 
	/* make sure window was created */ 
	if (!self->gh_wnd) {
		return;
	} 
	SetWindowLongW(self->gh_wnd, GWLP_USERDATA, (LONG)self);
	SetWindowPos(self->gh_wnd, 0, 		
		0, 0, 0, 0, SWP_NOOWNERZORDER | SWP_NOMOVE | SWP_NOSIZE);
	/* show and update main window */ 
	ShowWindow (self->gh_wnd, SW_SHOW); 
	UpdateWindow(self->gh_wnd);
}

static void process_events(Ex **e__, gfx__Display *self)
{
	MSG msg;

	self->e__ = e__; 
        while (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE) == TRUE) 
        { 
            if (GetMessage(&msg, NULL, 0, 0) ) 
            { 
                TranslateMessage(&msg); 
                DispatchMessage(&msg); 
            } else { 
                return; 
            } 
        } 
}

static void run(Ex **e__, gfx__Display *self)
{
	do {
		WaitMessage();
		self->_->process_events(e__, self);
	} while (self->gh_wnd != 0);
}

static void set_root(Ex **e__, gfx__Display *self, gfx__IEventHandler *root)
{
	(void)e__;
	self->root = *root;
}

static void gfx__i_raster(Ex **e__, gfx__Display *self, gfx__IRaster *out)
{
	(void)e__;
	out->_ = &gfx__IRaster__;
	out->__self__ = (Object*)self;	
}

static void get_info(Ex **e__, gfx__IRaster *itf, int32 x, int32 y, int32 w, 
	int32 h, gfx__IRaster_info *info)
{
	gfx__Display *self = (gfx__Display*)itf->__self__;

	(void)e__;

	info->vector = 0;
	info->pixel_format = GFX__IRASTER_RGBA;
	info->x = 0;
	info->y = 0;
	info->w = self->w;
	info->h = self->h;
	info->data = self->rgba;
	info->mask = self->mask;
	info->scanline_width = self->line_size;
}

static void flush(Ex **e__, gfx__IRaster *itf, gfx__IRaster_info *info)
{
	gfx__Display *self = (gfx__Display*)itf->__self__;
	(void)e__;
	(void)info;
	(void)self;
}

INIT_EXPORT gfx__Display__class gfx__Display__  = {
	// Object
	(utf8*)"gfx__Display",
	sizeof(gfx__Display),
	0,
	__init__,
	__ctor__,
	__dtor__,
	hash_code,
	to_string,

	// Display
	size,
	show,
	process_events,
	run,
	draw,
	set_root,
	gfx__i_raster,
	get_info,
	flush
	
};

static gfx__IRaster__class gfx__IRaster__  = {
	// Object
	0, 0, 0, 0, 0, 0, 0, 0,

	// IPaint
	get_info,
	flush
};
