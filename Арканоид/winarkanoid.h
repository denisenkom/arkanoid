#include <windows.h>

#define	WINDOW_CLASS	"Arkanoid"
#define	WINDOW_STYLE	(WS_OVERLAPPED|WS_BORDER|WS_CAPTION|WS_VISIBLE)

extern HWND global_hWnd;
extern HINSTANCE global_hInstance;

extern int fActive;
extern int fMinimized;

LRESULT CALLBACK wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
