#include "stdafx.h"
#pragma hdrstop

#include "a_shared.h"
#include "winarkanoid.h"
#include "graphics.h"
#include "net.h"
#include "server.h"
#include "client.h"
#include "host.h"
#include "controls.h"
#include "sound.h"
#include "netconnectdlg.h"
#include "screen.h"
#include "realtime.h"
#include "log.h"
#include "game.h"
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

extern char game_map[20][20];

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

		Server::FlushMessages();
		float oldtime = RealTime::getTime();
		while (1)
		{
			PumpAnyWindowsMessages();

			float currTime = RealTime::getTime();
			float dt = currTime - oldtime;
			//if (dt > 0.1f)
			//	dt = 0.1f;
			//else if (dt < 0.001)
			//	dt = 0.001;

			Host::frametime = dt;

			if (Server::active)
			{
				Server::FrameNo++;
				Server::datagramm.buffer.Clear();
				Server::datagramm.writeLong(Server::FrameNo);
				Server::ParseClientsMessages();
				Game::Phisics();
				Server::FlushMessages();
			}
			if (Client::connected)
			{
				bool skipFrame = false;
				while (1)
				{
					int ret = Network::ReadMessage(Client::message, Client::sock);

					if (ret == -1)
						throw std::runtime_error("Connection has broken.");

					if (ret == 0)
						break;

					Client::message.beginRead();
					long frame = Client::message.readLong();
					skipFrame = Client::LastFrame >= frame;
					if (!skipFrame)
					{
						Client::LastFrame = frame;
						Client::ParseServerMessage();
					}
				}
				if (!skipFrame)
				{
					Client::BeginCollectingMessages();
					Client::WriteClientMovements();
					Client::FlushMessages();
					Screen::Update();
				}
			}
			oldtime = currTime;
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
