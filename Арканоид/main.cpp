// Defines the entry point for the application.
//
#include <stdexcept>
#include <stdlib.h>

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
#include <stdio.h>
#include "winerrors.h"

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

HINSTANCE global_hInstance;

#include <geometry\geometry.h>

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	using MyGeometryTools::vec2;
	vec2<float> normal(-1, -1);
	normal.normalize();
	vec2<float> vec(-1, -1);
	vec.mirror(normal);

	global_hInstance = hInstance;

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

		Game::SpawnEntity(Ball(0, 100, 100, 10, 10, YELLOW));
		Game::SpawnEntity(Ball(0, 150, 150, -10, -10, YELLOW));
		/*srand(timeGetTime());
		for (unsigned i = 0; i < 10; i++)
			Game::SpawnEntity(Ball(0, rand()%600, rand()%600, rand()%200-100, rand()%200-100, YELLOW));*/
		Server::SendDatagrams();

		while (1)
		{
			MSG msg;
			while (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				if (msg.message == WM_QUIT)
				{
					All_Shutdown();
					return 0;
				}
				::TranslateMessage(&msg);
				::DispatchMessage(&msg);
			}
			static double time(0), oldtime(0);
			time = RealTime::getTime();
			double dt = time - oldtime;

			if (dt > 0.1)
				dt = 0.1;
			//else if (dt < 0.001)
			//	dt = 0.001;

			Host::frametime = dt;

			//double time1 = RealTime::getTime();
			if (Server::active)
			{
				Server::datagramm.buffer.Clear();
				Server::ParseClientsMessages();
				Game::Phisics();
				Server::SendDatagrams();
			}
			//double time2 = RealTime::getTime();
			if (Client::connected)
			{
				//double time1 = RealTime::getTime();
				Client::ParseServerMessages();
				//double time2 = RealTime::getTime();
				Client::message.buffer.Clear();
				Client::message.writeByte(CL_MOVE);
				if (Controls::keys[KEY_LEFT] || Controls::keys[KEY_UP])
					Client::message.writeByte(MOVE_LEFT);
				else if (Controls::keys[KEY_RIGHT] || Controls::keys[KEY_DOWN])
					Client::message.writeByte(MOVE_RIGHT);
				else
					Client::message.writeByte(NO_MOVE);
				Network::WriteMessage(Client::message, Client::sock);
				//double time3 = RealTime::getTime();
				Screen::Update();
				//double time4 = RealTime::getTime();
			}
			//double time3 = RealTime::getTime();
			//Log_Print("server %.3f : client %.3f\n", time2-time1, time3-time2);
			oldtime = time;
		}
	}
	catch (std::exception& ex)
	{
		All_Shutdown();
		assert_winerror(::MessageBox(NULL, ex.what(), NULL, MB_ICONSTOP));
		return -1;
	}

	return -1;
}