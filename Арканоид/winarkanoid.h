#include <windows.h>

#define	WINDOW_CLASS	"Arkanoid"
#define	WINDOW_STYLE	(WS_OVERLAPPED|WS_BORDER|WS_CAPTION|WS_VISIBLE)

extern HWND g_hWnd;
extern HINSTANCE g_hInstance;

extern int fActive;
extern int fMinimized;

LRESULT CALLBACK wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
