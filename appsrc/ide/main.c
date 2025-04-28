#include <gfx/Display.h>
#include <gfx/Window.h>

void wpdcc_main(Ex **e__)
{
	F("wpdcc_main");
	gfx__Display *disp;
	gfx__Window *root;
	gfx__IEventHandler handler;
	gfx__Box *c;
	String s;
	float32 f = 0.00000000023283064365386962890625f;

	root = new_(gfx__Window);

	disp = new_(gfx__Display);
	CHECK(error1);

	root->_->gfx__i_event_handler(e__, root, &handler);
	disp->_->set_root(e__, disp, &handler);

	ECHO("The PUBLIC DOMAIN IDE.\n");


	

	root->_->size(e__, root, 10, 10, 200, 150);
	STR(s, "hello this is me en @ tesxt% & jdfsdf fsjdh hfsdaf end, the quick brown fow jumps over the lazy dog");
	root->_->insert_text(e__, root, &s);
	c = root->_->insert_box(e__, root, GFX__STYLE_DISPLAY_INPUT_TEXT);

	disp->_->size(e__, disp, -1, -1, 300, 200);
	CHECK(error2);
	disp->_->show(e__, disp, 1);
	CHECK(error2);
	disp->_->run(e__, disp);
	CHECK(error2);
	delete_(disp);
	CHECK(error2);
	return;
error2:
	delete_(disp);
error1:
	return;
}




#if 0
/* 
 * Example of a Win32 OpenGL program.  
 * The OpenGL code is the same as that used in  
 * the X Window System sample 
 */ 
#include <windows.h> 
#include <GL/gl.h> 
 
/* Windows globals, defines, and prototypes */ 
WCHAR szAppName[]=L"Win OpenGL"; 
HWND  ghWnd; 
HDC   ghDC; 
HGLRC ghRC; 
 
#define SWAPBUFFERS SwapBuffers(ghDC) 
#define BLACK_INDEX     0 
#define RED_INDEX       13 
#define GREEN_INDEX     14 
#define BLUE_INDEX      16 
#define WIDTH           300 
#define HEIGHT          200 
 
LONG WINAPI MainWndProc (HWND, UINT, WPARAM, LPARAM); 
BOOL bSetupPixelFormat(HDC); 
 
 
GLvoid resize(GLsizei, GLsizei); 
GLvoid initializeGL(GLsizei, GLsizei); 
GLvoid drawScene(GLvoid); 
void polarView( GLdouble, GLdouble, GLdouble, GLdouble); 
BYTE XORmaskCursor[] = {0};
BYTE ANDmaskCursor[] = {0xff}; 
int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) 
{ 
    MSG        msg; 
    WNDCLASS   wndclass; 
 
    /* Register the frame class */ 
    wndclass.style         = 0; 
    wndclass.lpfnWndProc   = (WNDPROC)MainWndProc; 
    wndclass.cbClsExtra    = 0; 
    wndclass.cbWndExtra    = 0; 
    wndclass.hInstance     = hInstance; 
    wndclass.hIcon         = LoadIcon (hInstance, szAppName); 
    wndclass.hCursor       = CreateCursor( hInstance,   // app. instance 
             0,                // horizontal position of hot spot 
             0,                 // vertical position of hot spot 
             1,                // cursor width 
             1,                // cursor height 
             ANDmaskCursor,     // AND mask 
             XORmaskCursor );   ; 
    wndclass.hbrBackground = (HBRUSH)(COLOR_WINDOW+1); 
    wndclass.lpszMenuName  = szAppName; 
    wndclass.lpszClassName = szAppName; 
 
    if (!RegisterClass (&wndclass) ) 
        return FALSE; 
 
    /* Create the frame */ 
    ghWnd = CreateWindow (szAppName, 
             L"Generic OpenGL Sample", 
         WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, 
             CW_USEDEFAULT, 
             CW_USEDEFAULT, 
	           WIDTH, 
             HEIGHT, 
             NULL, 
             NULL, 
             hInstance, 
             NULL); 
 
    /* make sure window was created */ 
    if (!ghWnd) 
        return FALSE; 
 
    /* show and update main window */ 
    ShowWindow (ghWnd, nCmdShow); 
 
    UpdateWindow (ghWnd); 
 
    /* animation loop */ 
    while (1) { 
        /* 
         *  Process all pending messages 
         */ 
 
        while (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE) == TRUE) 
        { 
            if (GetMessage(&msg, NULL, 0, 0) ) 
            { 
                TranslateMessage(&msg); 
                DispatchMessage(&msg); 
            } else { 
                return TRUE; 
            } 
        } 
       // drawScene(); 
    } 
} 
 
/* main window procedure */ 
LONG WINAPI MainWndProc ( 
    HWND    hWnd, 
    UINT    uMsg, 
    WPARAM  wParam, 
    LPARAM  lParam) 
{ 
    LONG    lRet = 1; 
    PAINTSTRUCT    ps; 
    RECT rect; 
 
    switch (uMsg) { 
 
    case WM_CREATE: 
        ghDC = GetDC(hWnd); 
        if (!bSetupPixelFormat(ghDC)) 
            PostQuitMessage (0); 
 
        ghRC = wglCreateContext(ghDC); 
        wglMakeCurrent(ghDC, ghRC); 
        GetClientRect(hWnd, &rect); 
        //initializeGL(rect.right, rect.bottom); 
        break; 
 
    case WM_PAINT: 
        BeginPaint(hWnd, &ps); 
        EndPaint(hWnd, &ps); 
        break; 
 
    case WM_SIZE: 
        GetClientRect(hWnd, &rect); 
//        resize(rect.right, rect.bottom); 
        break; 
 
    case WM_CLOSE: 
        if (ghRC) 
            wglDeleteContext(ghRC); 
        if (ghDC) 
            ReleaseDC(hWnd, ghDC); 
        ghRC = 0; 
        ghDC = 0; 
 
        DestroyWindow (hWnd); 
        break; 
 
    case WM_DESTROY: 
        if (ghRC) 
            wglDeleteContext(ghRC); 
        if (ghDC) 
            ReleaseDC(hWnd, ghDC); 
 
        PostQuitMessage (0); 
        break; 
     
    case WM_KEYDOWN: 
        switch (wParam) { 
        case VK_LEFT: 
            drawScene();
            break; 
        case VK_RIGHT: 
            
            break; 
        case VK_UP: 
             
            break; 
        case VK_DOWN: 
             
            break; 
        } 
 
    default: 
        lRet = DefWindowProc (hWnd, uMsg, wParam, lParam); 
        break; 
    } 
 
    return lRet; 
} 
 
BOOL bSetupPixelFormat(HDC hdc) 
{ 
    PIXELFORMATDESCRIPTOR pfd, *ppfd; 
    int pixelformat; 
 
    ppfd = &pfd; 
 
    ppfd->nSize = sizeof(PIXELFORMATDESCRIPTOR); 
    ppfd->nVersion = 1; 
    ppfd->dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL |  
                        PFD_DOUBLEBUFFER; 
    ppfd->dwLayerMask = PFD_MAIN_PLANE; 
    ppfd->iPixelType = PFD_TYPE_COLORINDEX; 
    ppfd->cColorBits = 8; 
    ppfd->cDepthBits = 16; 
    ppfd->cAccumBits = 0; 
    ppfd->cStencilBits = 0; 
 
 //   pixelformat = ChoosePixelFormat(hdc, ppfd); 
 
    if ( (pixelformat = ChoosePixelFormat(hdc, ppfd)) == 0 ) 
    { 
        MessageBox(NULL, "ChoosePixelFormat failed", "Error", MB_OK); 
        return FALSE; 
    } 
 
    if (SetPixelFormat(hdc, pixelformat, ppfd) == FALSE) 
    { 
        MessageBox(NULL, "SetPixelFormat failed", "Error", MB_OK); 
        return FALSE; 
    } 
 
    return TRUE; 
} 
 
/* OpenGL code */ 
 
 

GLvoid drawScene(GLvoid) 
{ 
	int i, j;
	unsigned char p[64 * 64 * 3];
	static int k = 0;

	for (i = 0; i < 64; i++) {
		for (j = 0; j < 64; j++) {
			p[i * 64 * 3 + j * 3] = 0x8F;
			p[i * 64 * 3 + j * 3 + 1] = 0x10;
			p[i * 64 * 3 + j * 3 + 2] = 0x10;
		}
	}

	k += 3;
	if (k > 200) k = 0;

	i = 3; // y
		for (j = 0; j < 32; j++) {
			//j = x;
			p[i * 64 * 3 + j * 3 + k] = 0xFF;
			p[i * 64 * 3 + j * 3 + 1 + k] = 0xF0;
			p[i * 64 * 3 + j * 3 + 2 + k] = 0xF0;
		}
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT ); 
    glDrawPixels(64, 64, GL_RGB, GL_UNSIGNED_BYTE, p);
    SWAPBUFFERS; 
} 
 
#endif
