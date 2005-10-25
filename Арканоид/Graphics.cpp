#include "StdAfx.h"
#pragma hdrstop

#include "winerrors.h"
#include "dxerrors.h"

#include "a_shared.h"
#include "Graphics.h"
#include "Screen.h"
#include "ddutil.h"
#include "WINException.h"
#include "debug.h"
#include "winarkanoid.h"
#include "win_gr.h"
#include "Controls.h"


/* global variables */
// defined in winarkanoid.h
extern HWND global_hWnd = NULL;
extern int fActive = false;
extern int fMinimized = false;


/* Local variables */
static bool initialized = false;
static bool fullscreen = false;

Renderer* pRenderer;

static struct
{
	int width;
	int height;
	unsigned short* data;
} cache[MAX_IMAGE_CACHE] = {0};


// Local function prototypes

void RegisterMainWindowClass();
HWND CreateMainWindow();
void DetermineWindowSize(SIZE* pSize);

int MapKey(int vkey)
{
	switch (vkey)
	{
	case VK_UP:
		return KEY_UP;
	case VK_DOWN:
		return KEY_DOWN;
	case VK_LEFT:
		return KEY_LEFT;
	case VK_RIGHT:
		return KEY_RIGHT;
	case VK_ESCAPE:
		return KEY_ESC;
	default:
		return KEY_OTHER;
	}
}

static LRESULT CALLBACK wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) throw ()
{
	switch (message) 
	{
	case WM_HOTKEY:
		return 0;

	case WM_SYSKEYDOWN:
		if (wParam == 13)
		{
			Graphics::Shutdown();
			fullscreen = !fullscreen;
			try 
			{
				Graphics::Init();
			}
			catch (...)
			{
				fullscreen = !fullscreen;
				Graphics::Init();
			}

			if (!fullscreen)
			{
				RECT clRect;
				clRect.left = clRect.top = 0;
				clRect.right = SCN_WIDTH;
				clRect.bottom = SCN_HEIGHT;
				::AdjustWindowRect(&clRect, WINDOW_STYLE, FALSE);
				::MoveWindow(global_hWnd, 20, 20, clRect.right-clRect.left, clRect.bottom-clRect.top, TRUE);
				::ShowCursor(TRUE);
			}
			else
				::ShowCursor(FALSE);
			return 0;
		}
	case WM_KEYDOWN:
		Controls::keys[MapKey(wParam)] = true;
		if (wParam == VK_ESCAPE)
			PostQuitMessage(0);
		break;

	case WM_SYSKEYUP:
	case WM_KEYUP:
		Controls::keys[MapKey(wParam)] = false;
		break;

	case WM_ACTIVATE:
		switch (LOWORD(wParam)) {
		case WA_ACTIVE:
		case WA_CLICKACTIVE:
			fActive = true;
			break;
		case WA_INACTIVE:
			fActive = false;
			break;
		}
		fMinimized = (BOOL)HIWORD(wParam);
		return DefWindowProc(hWnd, message, wParam, lParam);

	case WM_SYSCOMMAND:
		if (wParam == SC_SCREENSAVE)
			return 0;
		return DefWindowProc(hWnd, message, wParam, lParam);

	//case WM_DESTROY:
		//PostQuitMessage(0);
		//return 0;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
   }
	return DefWindowProc(hWnd, message, wParam, lParam);
}

void Graphics::Init()
{
	if (initialized)
		return;

	global_hWnd = CreateMainWindow();

	::ShowWindow(global_hWnd, SW_SHOW);

	pRenderer = &DirectDrawRenderer1;
	pRenderer->Init(fullscreen);
	initialized = true;
}

HWND CreateMainWindow() {
	RegisterMainWindowClass();

	SIZE windowSize;
    DetermineWindowSize(&windowSize);

	HWND result = ::CreateWindow(
		WINDOW_CLASS,
		"Arkanoid",
		WINDOW_STYLE,
		CW_USEDEFAULT, CW_USEDEFAULT, windowSize.cx, windowSize.cy,
		NULL, // root window
		NULL, // no menu
		global_hInstance,
		NULL  // no params
	);

	if (result == NULL)
		throw WINException("Screen::Init(): CreateWindow(): ");

	return result;
}

void RegisterMainWindowClass() {
	WNDCLASSEX wcex;

	wcex.cbSize         = sizeof(WNDCLASSEX); 
	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)wndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= global_hInstance;
	wcex.hIcon			= NULL;
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= WINDOW_CLASS;
	wcex.hIconSm		= NULL;

	if( !RegisterClassEx(&wcex) )
		throw WINException("Ошибка при регистрации класса окна: ");
}

void DetermineWindowSize(SIZE* pSize) {
	RECT rect;

	rect.left   = 0;
	rect.top    = 0;
	rect.right  = SCN_WIDTH;
	rect.bottom = SCN_HEIGHT;

	if( !AdjustWindowRect(&rect, WINDOW_STYLE, FALSE) ) {
		pSize->cx = SCN_WIDTH  + 10;
		pSize->cy = SCN_HEIGHT + 10;
		return;
	}

	pSize->cx = rect.right  - rect.left;
	pSize->cy = rect.bottom - rect.top;
}

void Graphics::Shutdown() throw()
{
	initialized = false;

	if(pRenderer)
		pRenderer->Shutdown();

	if (global_hWnd != NULL)
		::DestroyWindow(global_hWnd);

	::UnregisterClass(WINDOW_CLASS, global_hInstance);
}

/*void Graphics::DrawTransparent(int scx, int scy)
{
	RECT rect;
	DDSURFACEDESC desc = {0};
	rect.left = scx;
	rect.right = scx+20;
	rect.top = scy;
	rect.bottom = scy+20;
	int rad = 10;
	desc.dwSize = sizeof(desc);
	assert_hres(_back->Lock(&rect, &desc, DDLOCK_WAIT, NULL));
	for (int y = rect.top; y < rect.bottom; y++)
	{
		int ofs = desc.dwWidth*y;
		int bally = y-rad-rect.top;
		for (int x = rect.left; x < rect.right; x++)
		{
			int ballx = x-rad-rect.left;
			int sqrad = ballx*ballx+bally*bally;
			if (sqrad <= 10*10)
			{
				int srcx = x + (ballx<<1);
				int srcy = y + (bally<<1);
				int srcofs = srcy*desc.dwWidth;
				DWORD color1 = ((DWORD*)desc.lpSurface)[srcofs+srcx];
				DWORD color2 = ball_tex[ballx+rad][bally+rad];
				((DWORD*)desc.lpSurface)[ofs+x] = (color1+color2)/2;
			}
		}
	}
	assert_hres(_back->Unlock(desc.lpSurface));
}*/

void Graphics::Clear()
{
	if (!initialized)
		return;

	pRenderer->Clear();
	//for (int y = 0; y < SCN_HEIGHT; y++)
	//	for (int x = 0; x < SCN_WIDTH; x++)
	//		*(pbitmap+y*pitch/2+x)=0;
}


void Graphics::Flush()
{
	if (!initialized)
		return;

	pRenderer->Flush();
}

int Graphics::LoadImg(const char* img_name)
{
	if (!initialized)
		throw std::exception("Called Graphics::LoadImg() on not initialized Graphics.");

	int free_index = -1;

	for (int i = 0; i < MAX_IMAGE_CACHE; i++)
		if (cache[i].data == NULL)
		{
			free_index = i;
			break;
		}

	if (free_index == -1)
		throw std::exception("GDI_LoadImage(): No more free cache.");
	
	cache[i].data = (unsigned short*)pRenderer->LoadImg(img_name);

	/*HBITMAP hbmp;
	if ((hbmp = (HBITMAP)::LoadImage(global_hInstance, img_name, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION)) == NULL)
		if ((hbmp = (HBITMAP)::LoadImage(NULL, img_name, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION|LR_LOADFROMFILE)) == NULL)
			throw std::runtime_error(std::string("GDI_LoadImage(): Cannot load image ") += img_name);
	
	BITMAP bitmap;
	assert_winerror(::GetObject(hbmp, sizeof(bitmap), &bitmap));

	unsigned short* bits = new unsigned short[bitmap.bmWidth*bitmap.bmHeight];
	for (int j = 0; j < bitmap.bmWidth*bitmap.bmHeight; j++)
	{
		unsigned char r, g, b;
		b = *((unsigned char*)bitmap.bmBits+j*3)/8;
		g = *((unsigned char*)bitmap.bmBits+j*3+1)/4;
		r = *((unsigned char*)bitmap.bmBits+j*3+2)/8;
		bits[j] = b | (g << 5) | (r << 11);
	}

	cache[free_index].width = bitmap.bmWidth;
	cache[free_index].height = bitmap.bmHeight;
	cache[free_index].data = bits;*/

	return free_index;
}

void Graphics::DrawImg(int img_id, int scx, int scy, rect_s bmp_rect)
{
	if (!initialized)
		return;

	pRenderer->DrawImg(reinterpret_cast<Image*>(cache[img_id].data), scx, scy, bmp_rect);
	/*int srcy1 = cache[img_id].height - bmp_rect.top - bmp_rect.height;
	int srcy2 = cache[img_id].height - bmp_rect.top;
	int desty;
	if (!fullscreen)
		desty = SCN_HEIGHT-scy-1;
	else
		desty = scy;

	int bmp_rect_right = bmp_rect.left + bmp_rect.width;

	unsigned short* srcoffs = cache[img_id].data+(srcy2-1)*cache[img_id].width;
	unsigned short* destoffs = pbitmap+desty*pitch/2;
	for (int dy = desty, sy = srcy2-1; sy >= srcy1; !fullscreen ? dy-- : dy++, sy--)
	{
		for (int dx = scx, sx = bmp_rect.left; sx < bmp_rect_right; dx++, sx++)
		{
			unsigned short src = *(srcoffs + sx);
			unsigned short *pdest = destoffs + dx;
			if (src != COLOR_KEY)
				*pdest = src;
		}
		srcoffs -= cache[img_id].width;
		if (fullscreen)
			destoffs += pitch/2;
		else
			destoffs -= pitch/2;
	}*/
}

/*void Graphics::DrawTransparentImg(int img_id, int scx, int scy, rect_s bmp_rect)
{
	if (!initialized)
		return;

	int srcy1 = cache[img_id].height - bmp_rect.top - bmp_rect.height;
	int srcy2 = cache[img_id].height - bmp_rect.top;
	int desty;
	if (!fullscreen)
		desty = SCN_HEIGHT-scy-1;
	else
		desty = scy;

	int bmp_rect_right = bmp_rect.left + bmp_rect.width;

	for (int dy = desty, sy = srcy2-1; sy >= srcy1; !fullscreen ? dy-- : dy++, sy--)
	{
		unsigned short* srcoffs = cache[img_id].data+sy*cache[img_id].width;
		unsigned short* destoffs = pbitmap+dy*pitch/2;
		for (int dx = scx, sx = bmp_rect.left; sx < bmp_rect_right; dx++, sx++)
		{
			unsigned short* src = srcoffs + sx;
			unsigned short* dest = destoffs + dx;
			if (*src != COLOR_KEY)
				*dest = *dest/2+*src/2;
		}
	}
}*/

void Graphics::DrawTxt(int x, int y, const char* text)
{
	if (!initialized)
		return;

	pRenderer->DrawText(x, y, text);
}