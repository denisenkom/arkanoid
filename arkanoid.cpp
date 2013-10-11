#include "resource.h"
#include "common.h"
#include "arkanoid.h"
#include "graphics.h"
#include "screen.h"
#include "controls.h"
#include "net.h"
#include "server.h"
#include "host.h"
#include "entities.h"
#include "game.h"
#include "netconnectdlg.h"
#include "realtime.h"

#define MAX_LOADSTRING	100

HINSTANCE Arkanoid::hInstance = NULL;
HINSTANCE Arkanoid::hPrevInstance = NULL;
LPCTSTR Arkanoid::lpCmdLine = NULL;
//HACCEL Arkanoid::hAccelTable = NULL;
LPCTSTR Arkanoid::szTitle = "Арканоид";
LPCTSTR Arkanoid::szWindowClass = "ARKANOID CLASS";
int Arkanoid::nExitCode = 0;
int Arkanoid::sleep_counter = 0;

