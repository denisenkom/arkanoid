#include "stdafx.h"
#pragma hdrstop

#include "winerrors.h"
#include "dxerrors.h"
#include "ddutil.h"

#include "a_shared.h"
#include "winarkanoid.h"
#include "win_gr.h"
#include "graphics.h"

#include "log.h"


using namespace std;


typedef HRESULT (WINAPI * pDirectDrawCreate_t)( GUID FAR *lpGUID, LPDIRECTDRAW FAR *lplpDD, IUnknown FAR *pUnkOuter );


DirectDrawRenderer DirectDrawRenderer1;


DirectDrawRenderer::DirectDrawRenderer()
	: hlib(0)
{
}

DirectDrawRenderer::~DirectDrawRenderer()
{
	if (hlib)
		Shutdown();
}

void DirectDrawRenderer::Init(bool fullscreen)
{
	if (hlib)
		return;

	this->fullscreen = fullscreen;

	HookDirectDraw(fullscreen);

	if (fullscreen)
		ddraw->SetDisplayMode(SCN_WIDTH, SCN_HEIGHT, SCN_BPP, SCN_REFRESH, 0);

	CreateFrameBuffer(fullscreen);
	CreateBackBuffer(fullscreen);


	/*RECT rect;
	rect.left = 0;
	rect.top = 0;
	rect.right = SCN_WIDTH;
	rect.bottom = SCN_HEIGHT;
	desc.dwSize = sizeof(desc);
	assert_hres(back->Lock(&rect, &desc, DDLOCK_WAIT, NULL));

	pbitmap = (unsigned short*)desc.lpSurface;
	pitch = desc.lPitch;*/
}

void DirectDrawRenderer::HookDirectDraw(bool fullscreen) {
	hlib = LoadLibrary("ddraw.dll");
	if (hlib == NULL)
		throw WINException("Failed to load ddraw.dll: ");

	static pDirectDrawCreate_t pDirectDrawCreate = NULL;
	pDirectDrawCreate = (pDirectDrawCreate_t)GetProcAddress(hlib, "DirectDrawCreate");

	HRESULT hres;

	LPDIRECTDRAW pdd;
	hres = pDirectDrawCreate(NULL, &pdd, NULL);
	if( FAILED(hres) )
		throw runtime_error("ошибка DirectDrawCreate");

	hres = pdd->QueryInterface( IID_IDirectDraw7, reinterpret_cast<void**>(&ddraw) );
	if( FAILED(hres) )
		throw runtime_error("ошибка DirectDrawCreate");

	DWORD cooplevel;
	if(fullscreen)
		cooplevel = DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN;
	else
		cooplevel = DDSCL_NORMAL;
	hres = ddraw->SetCooperativeLevel(g_hWnd, cooplevel);
	if( FAILED(hres) )
		throw runtime_error("ошибка DirectDrawCreate");
}

void DirectDrawRenderer::CreateFrameBuffer(bool fullscreen) {
	DDSURFACEDESC2 desc = {0};
	desc.dwSize = sizeof(desc);
	if(fullscreen) {
		desc.dwFlags =
			// DDSD_PIXELFORMAT |
			DDSD_CAPS |
			DDSD_BACKBUFFERCOUNT;
		desc.ddsCaps.dwCaps =
			DDSCAPS_COMPLEX |
			DDSCAPS_FLIP |
			DDSCAPS_PRIMARYSURFACE |
			DDSCAPS_VIDEOMEMORY;
	} else {
		desc.dwFlags =
			DDSD_CAPS /*|
			DDSD_BACKBUFFERCOUNT |
			DDSD_WIDTH |
			DDSD_HEIGHT*/;
		desc.ddsCaps.dwCaps =
			DDSCAPS_PRIMARYSURFACE |
			DDSCAPS_VIDEOMEMORY /*|
			DDSCAPS_FLIP |
			DDSCAPS_COMPLEX*/;
	}
	desc.dwBackBufferCount = 1;
	//desc.ddpfPixelFormat.dwSize = sizeof(desc.ddpfPixelFormat);
	//desc.ddpfPixelFormat.dwFlags = DDPF_RGB;
	//desc.ddpfPixelFormat.dwRGBBitCount = DDBD_16;
	//desc.ddpfPixelFormat.dwBBitMask = 0x001f;
	//desc.ddpfPixelFormat.dwGBitMask = 0x03e0;
	//desc.ddpfPixelFormat.dwRBitMask = 0x7c00;
	//desc.ddpfPixelFormat.dwRGBAlphaBitMask = 0;
	ddraw->CreateSurface(&desc, &prim, NULL);
}

void DirectDrawRenderer::CreateBackBuffer(bool fullscreen) {
	HRESULT hres;
	if (fullscreen)
	{
		DDSCAPS2 caps;
		caps.dwCaps = DDSCAPS_BACKBUFFER;
		caps.dwCaps2 = 0;
		caps.dwCaps3 = 0;
		caps.dwCaps4 = 0;
		caps.dwVolumeDepth = 0;
		hres = prim->GetAttachedSurface(&caps, &back);
		if( FAILED(hres) )
			throw runtime_error("Ошибка GetAttachedSurface");
	}
	else
	{
		DDSURFACEDESC2 desc;
		desc.dwSize = sizeof(desc);
		desc.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;
		desc.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_VIDEOMEMORY;
		desc.ddsCaps.dwCaps2 = 0;
		desc.ddsCaps.dwCaps3 = 0;
		desc.ddsCaps.dwCaps4 = 0;
		desc.ddsCaps.dwVolumeDepth = 0;
		desc.dwWidth = SCN_WIDTH;
		desc.dwHeight = SCN_HEIGHT;
		hres = ddraw->CreateSurface(&desc, &back, NULL);
		if( FAILED(hres) )
			throw runtime_error("Ошибка GetAttachedSurface");
	}
}

void DirectDrawRenderer::Clear()
{
	DDBLTFX fx;
	fx.dwSize = sizeof(fx);
	fx.dwFillColor = 0;
	back->Blt(NULL, NULL, NULL, DDBLT_COLORFILL, &fx);
}

void DirectDrawRenderer::Shutdown() throw()
{
	if (back)
	{
		//back->Unlock(pbitmap);
		back->Release();
		back = NULL;
	}
	if (prim)
	{
		prim->Release();
		prim = NULL;
	}

	if (ddraw)
	{
		ddraw->RestoreDisplayMode();
		ddraw->SetCooperativeLevel(NULL, DDSCL_NORMAL);
		ddraw->Release();
		ddraw = NULL;
	}

	if (hlib != NULL)
	{
		FreeLibrary(hlib);
		hlib = NULL;
	}
}

/*void DDRAW_Clear()
{
	RECT rect = {0, 0, SCN_WIDTH, SCN_HEIGHT};
	DDBLTFX bltfx = {0};
	bltfx.dwSize = sizeof(bltfx);
	bltfx.dwFillColor = RGB(0, 0, 0);
	_back->Blt(&rect, NULL, NULL, DDBLT_COLORFILL | DDBLT_WAIT, &bltfx);
}

int DDRAW_LoadImage(LPCTSTR img_name)
{
	LPDIRECTDRAWSURFACE surf = DDLoadBitmap(_ddraw, img_name, 0, 0);
	if (surf == NULL)
		return -1;

	HRESULT hres = DDSetColorKey(surf, CLR_INVALID);
	if (hres != DD_OK)
		return -1;

	for (int i = 0; i < MAX_IMAGE_CACHE; i++)
		if (_cache[i] == NULL)
			break;

	if (i >= MAX_IMAGE_CACHE)
		return -1;

	_cache[i] = surf;

	return i;
}

void DDRAW_DrawImage(int img_id, int scx, int scy, RECT &bmp_rect)
{
	RECT rect;
	rect.left = scx;
	rect.right = scx + bmp_rect.right - bmp_rect.left;
	rect.top = scy;
	rect.bottom = scy + bmp_rect.bottom - bmp_rect.top;

	_back->Blt(&rect, _cache[img_id], &bmp_rect, DDBLT_WAIT | DDBLT_KEYSRC, NULL);
}*/

void DirectDrawRenderer::DrawText(int x, int y, const char* text) throw ()
{
	HDC hdc;
	back->GetDC(&hdc);
	SetTextColor( hdc, RGB(255,255,255) );
	SetBkColor( hdc, RGB(0,0,0) );
	TextOut( hdc, x, y, text, strlen(text) );
	back->ReleaseDC(hdc);
}

void DirectDrawRenderer::Flush()
{
	//RECT rect;
	//DDSURFACEDESC desc;

	//assert_hres(back->Unlock(pbitmap));
	ddraw->WaitForVerticalBlank(DDWAITVB_BLOCKBEGIN, NULL);
	if (fullscreen)
	{
		prim->Flip(NULL, 0);
	}
	else
	{
		POINT point = {0, 0};
		ClientToScreen(g_hWnd, &point);
		prim->BltFast(point.x, point.y, back, NULL, 0);
	}

	/*rect.left = 0;
	rect.top = 0;
	rect.right = SCN_WIDTH;
	rect.bottom = SCN_HEIGHT;
	desc.dwSize = sizeof(desc);
	back->Lock(&rect, &desc, DDLOCK_WAIT, NULL);
	pbitmap = (unsigned short*)desc.lpSurface;*/
}

Image* DirectDrawRenderer::LoadImg(const char * name)
{
	return new DirectDrawImage(name, ddraw);
}

/*
	Очень медленно работает процедура BltFast и Blt
*/
void DirectDrawRenderer::DrawImg(Image* img, int scx, int scy, rect_s bmp_rect)
{
	//DirectDrawImage* ddimg = dynamic_cast<DirectDrawImage*>(img);
	DirectDrawImage* ddimg = reinterpret_cast<DirectDrawImage*>(img);
	RECT rect1;
	rect1.left = bmp_rect.left;
	rect1.top = bmp_rect.top;
	rect1.right = bmp_rect.left + bmp_rect.width;
	rect1.bottom = bmp_rect.top + bmp_rect.height;
	//assert_hres(back->BltFast(scx, scy, ddimg->surf, &rect1, DDBLTFAST_NOCOLORKEY | DDBLTFAST_WAIT));
	RECT rect2;
	rect2.left = scx;
	rect2.top = scy;
	rect2.right = scx + bmp_rect.width;
	rect2.bottom = scy + bmp_rect.height;

	back->Blt(&rect2, ddimg->surf, &rect1, DDBLT_ASYNC, NULL);
}

DirectDrawImage::DirectDrawImage(const char * name, LPDIRECTDRAW7 ddraw)
	: surf(::DDLoadBitmap(ddraw, name, 0, 0))
{
}

DirectDrawImage::~DirectDrawImage()
{
	if (surf)
		surf->Release();
}
