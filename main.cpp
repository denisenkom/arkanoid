#include "StdAfx.h"
#pragma hdrstop

#include "a_shared.h"
#include "winarkanoid.h"
#include "Graphics.h"
#include "NET.h"
#include "Server.h"
#include "Client.h"
#include "Host.h"
#include "Controls.h"
#include "Sound.h"
#include "NetConnectDlg.h"
#include "Screen.h"
#include "Realtime.h"
#include "log.h"
#include "Game.h"
#include "winerrors.h"


void PumpAnyWindowsMessages();


void Log_Print(const char* fmt, ...)
{
	FILE * file = fopen("log.txt", "a");
	va_list args;
	va_start(args, fmt);
	vfprintf(file, fmt, args);
	va_end(args);
	fclose(file);
}

static void All_Shutdown()
{
	Sound::Shutdown();
	Graphics::Shutdown();
	Server::Shutdown();
	Network::Shutdown();
}

HINSTANCE g_hInstance;

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	g_hInstance = hInstance;

	try
	{
		Network::Init();
		int res = NetConnectDlg::Run();
		if (res == IDCANCEL)
		{
			Network::Shutdown();
			return 0;
		}
		Graphics::Init();
		Screen::Init();
		try
		{
			Sound::Init();
		}
		catch (std::exception& ex)
		{
			Log_Print("Sound not initialized: %s\n", ex.what());
		}

		//Game::MakeScene();

		Server::FlushMessages();

		while (1)
		{
			PumpAnyWindowsMessages();

			static float time(0), oldtime(0);
			time = RealTime::getTime();
			float dt = time - oldtime;

			if (dt > 0.1f)
				dt = 0.1f;
			//else if (dt < 0.001)
			//	dt = 0.001;

			Host::frametime = dt;

			if (Server::active)
			{
				Server::datagramm.buffer.Clear();
				Server::ParseClientsMessages();
				Game::Phisics();
				Server::FlushMessages();
			}
			if (Client::connected)
			{
				Client::ParseServerMessages();
				Client::BeginCollectingMessages();
				Client::WriteClientMovements();
				Client::FlushMessages();
				Screen::Update();
			}
			oldtime = time;
		}
	}
	catch (std::exception& ex)
	{
		All_Shutdown();
		::MessageBox( NULL, ex.what(), NULL, MB_ICONSTOP );
		return 3;
	}

	return 3;
}

void PumpAnyWindowsMessages() {
	MSG msg;
	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		if (msg.message == WM_QUIT)
		{
			All_Shutdown();
			exit(msg.wParam);
		}
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}