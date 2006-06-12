#include "StdAfx.h"
#pragma hdrstop

#include "winerrors.h"

#include "a_shared.h"
#include "winarkanoid.h"
#include "win_gr.h"
#include "graphics.h"

void DIBRenderer::Init(bool fullscreen)
{
	hdc = ::GetDC(g_hWnd);
	BITMAPINFO bmpinfo;
	bmpinfo.bmiHeader.biSize = sizeof(bmpinfo.bmiHeader);
	bmpinfo.bmiHeader.biWidth = SCN_WIDTH;
	bmpinfo.bmiHeader.biHeight = SCN_HEIGHT;
	bmpinfo.bmiHeader.biPlanes = 1;
	bmpinfo.bmiHeader.biBitCount = SCN_BPP;
	bmpinfo.bmiHeader.biCompression = BI_RGB;
	bmpinfo.bmiHeader.biSizeImage = 0;
	bmpinfo.bmiHeader.biXPelsPerMeter = 0;
	bmpinfo.bmiHeader.biYPelsPerMeter = 0;
	bmpinfo.bmiHeader.biClrUsed = 0;
	bmpinfo.bmiHeader.biClrImportant = 0;
	VOID * bits;
	HBITMAP hbmp;
	hbmp = ::CreateDIBSection(NULL, &bmpinfo, DIB_RGB_COLORS, &bits, NULL, 0);
	//pbitmap = (unsigned short*)bits;
	//pitch = SCN_WIDTH*2;
	hbackdc = ::CreateCompatibleDC(hdc);
	::SelectObject(hbackdc, hbmp);
}

void DIBRenderer::Shutdown()
{
	::DeleteDC(hbackdc);
	::ReleaseDC(g_hWnd, hdc);
}

/*void DIB_Clear()
{
	BitBlt(hbackdc, 0, 0, SCN_WIDTH, SCN_HEIGHT, NULL, 0, 0, BLACKNESS);
}

int DIB_LoadImage(LPCTSTR img_name)
{
}

void DIB_DrawImage(int img_id, int x, int y, const RECT &bmp_rect)
{
	BitBlt(hbackdc, x, y, bmp_rect.right-bmp_rect.left,
		bmp_rect.bottom-bmp_rect.top, cache[img_id],
		bmp_rect.left, bmp_rect.top, SRCCOPY);
}*/

void DIBRenderer::DrawText(int x, int y, const char* text)
{
	::SetTextColor( hbackdc, RGB(255, 255, 255) );
	::SetBkColor( hbackdc, RGB(0, 0, 0) );
	TextOut( hbackdc, x, y, text, strlen(text) );
}

void DIBRenderer::Flush()
{
	BitBlt(hdc, 0, 0, SCN_WIDTH, SCN_HEIGHT, hbackdc, 0, 0, SRCCOPY);
}