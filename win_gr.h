#ifndef WIN_GR_H_INCLUDED
#define WIN_GR_H_INCLUDED

#include <windows.h>
#include <ddraw.h>
#include "winexception.h"

//extern LPDIRECTDRAWSURFACE _prim;
//extern LPDIRECTDRAWSURFACE _back;

//extern HDC hdc;
//extern HDC hbackdc;

//extern unsigned short* pbitmap;
//extern unsigned pitch;

class Image
{
public:
	virtual ~Image() {};
};

class Renderer
{
public:
	virtual void Init(bool fullscreen) = 0;
	virtual void Shutdown() throw() = 0;
	virtual void DrawText(int x, int y, const char * text) throw() = 0;
	virtual void Flush() = 0;
	virtual Image* LoadImg(const char * file_or_resource_name) = 0;
	virtual void DrawImg(Image* img, int scx, int scy, rect_s bmp_rect) = 0;
	virtual void Clear() = 0;
};

class DirectDrawImage : public Image
{
public:
	LPDIRECTDRAWSURFACE7 surf;
	DirectDrawImage(const char * file_or_resource_name, LPDIRECTDRAW7 ddraw);
	virtual ~DirectDrawImage();
};

class DirectDrawRenderer : public Renderer
{
	HINSTANCE hlib;
	LPDIRECTDRAW7 ddraw;
	LPDIRECTDRAWSURFACE7 prim;
	LPDIRECTDRAWSURFACE7 back;
	bool fullscreen;
public:
	DirectDrawRenderer();
	~DirectDrawRenderer();
	virtual void Init(bool fullscreen);
	virtual void Shutdown() throw();
	virtual void DrawText(int x, int y, const char * text) throw();
	virtual void Flush();
	virtual Image* LoadImg(const char * file_or_resource_name);
	virtual void DrawImg(Image* img, int scx, int scy, rect_s bmp_rect);
	virtual void Clear();

private:
	void HookDirectDraw(bool fullscreen);
	void CreateFrameBuffer(bool fullscreen);
	void CreateBackBuffer(bool fullscreen);
};
extern DirectDrawRenderer DirectDrawRenderer1;

//void DDRAW_Init();
//void DDRAW_Shutdown() throw();
//void DDRAW_DrawText(int x, int y, const char * text) throw();

class DIBRenderer : public Renderer
{
	HDC hdc, hbackdc;
public:
	DIBRenderer();
	~DIBRenderer();
	virtual void Init(bool fullscreen);
	virtual void Shutdown() throw();
	virtual void DrawText(int x, int y, const char * text) throw();
	virtual void Flush();
};

#endif	// WIN_GR_H_INCLUDED
