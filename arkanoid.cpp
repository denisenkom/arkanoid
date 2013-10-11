#include "resource.h"
#include "common.h"
#include "Arkanoid.h"
#include "Graphics.h"
#include "Screen.h"
#include "Controls.h"
#include "NET.h"
#include "Server.h"
#include "Host.h"
#include "Entities.h"
#include "Game.h"
#include "NetConnectDlg.h"
#include "d:\reuse\RealTime.h"

#define MAX_LOADSTRING	100

HINSTANCE Arkanoid::hInstance = NULL;
HINSTANCE Arkanoid::hPrevInstance = NULL;
LPCTSTR Arkanoid::lpCmdLine = NULL;
//HACCEL Arkanoid::hAccelTable = NULL;
LPCTSTR Arkanoid::szTitle = "Арканоид";
LPCTSTR Arkanoid::szWindowClass = "ARKANOID CLASS";
int Arkanoid::nExitCode = 0;
int Arkanoid::sleep_counter = 0;

