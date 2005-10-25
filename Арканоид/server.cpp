#include "StdAfx.h"
#pragma hdrstop

#include "a_shared.h"
#include "NET.h"
#include "Server.h"
#include "Game.h"

/* local variables */
static struct client
{
	bool connected;
	int socket;
	Message message;

	client() : connected(false), socket(0) {}
} clients[MAX_CLIENTS];
static int connected_clients = 0;

/* Local routines */

static void DropClient_ConnectionLost(int id)
{
	clients[id].connected = false;
	Network::CloseSocket(clients[id].socket);
	Game::players[id].active = false;
	delete Game::entities[id];
	Game::entities[id] = 0;

	Server::UpdatePlayersList();
}

/* Implementation */

bool Server::active = false;
Message Server::datagramm;

void Server::Shutdown()
{
	active = false;
	Network::Listen(false);
}

void Server::Activate(bool enable)
{
	active = enable;
	Network::Listen(enable);
}

/*void Server::Frame()
{
	if (!active)
		return;

	datagramm.buffer.Clear();

	if (connected_clients <= 0)
		return;

	ParseClientsMessages();

	Game::Phisics();
	SendDatagrams();
}*/

void Server::SendDatagrams()
{
	if (connected_clients <= 0)
		return;

	for (int i = 0; i < MAX_CLIENTS; i++)
		if (clients[i].connected)
		{
			int ret = Network::WriteMessage(datagramm, clients[i].socket);
			if (ret == -1)
				DropClient_ConnectionLost(i);
		}
}

void Server::UpdatePlayersList()
{
	datagramm.writeByte(SV_UPDATE_PLAYERS_LIST);
	for (int pi = 0; pi < 4; pi++)
	{
		datagramm.writeByte(Game::players[pi].active);
		datagramm.writeByte(Game::players[pi].color);
		datagramm.writeStr(Game::players[pi].name);
		datagramm.writeLong(Game::players[pi].pts);
	}
}

void Server::CheckWaitingClients()
{
	if (!active)
		return;

	int sock = Network::Accept();
	if (sock == 0 || sock == -1)
		return;

	for (int i = 0; i < MAX_CLIENTS; i++)
	{
		if (clients[i].connected)
			continue;

		clients[i].connected = true;
		clients[i].socket = sock;
		Game::players[i].active = true;
		Game::players[i].color = (color_enum)i;
		Game::players[i].pts = 0;
		connected_clients++;
		clients[i].message.buffer.Clear();
		clients[i].message.writeByte(SV_SEND_INFO);
		clients[i].message.writeByte(i);	// посылаем номер игрока
		Network::WriteMessage(clients[i].message, sock);
		clients[i].message.buffer.Clear();
		return;
	}
}

void Server::ParseClientsMessages()
{
	for (int i = 0; i < MAX_CLIENTS; i++)
		if (clients[i].connected)
		{
			// цикл по всем сообщениям клиента
			// ообщений может прийти несколько
			while (true)
			{
				int ret = Network::ReadMessage(clients[i].message, clients[i].socket);

				if (ret == -1)
				{
					DropClient_ConnectionLost(i);
					break;
				}

				if (ret == 0)
					break;

				clients[i].message.beginRead();
				while (!clients[i].message.eom())
				{
					cl_cmd cmd = (cl_cmd)clients[i].message.readByte();
					switch (cmd) {
					case CL_NOP:
						break;

					case CL_INFO:
						char* clname;
						clname = clients[i].message.readStr();
						strcpy(Game::players[i].name, clname);
						Server::UpdatePlayersList();
						break;

					case CL_DROP:
						DropClient_ConnectionLost(i);
						break;

					case CL_PRINT:
						Server::datagramm.writeByte(SV_PRINT);
						Server::datagramm.writeByte(i);
						Server::datagramm.writeStr(clients[i].message.readStr());
						break;

					case CL_MOVE:
						if (Game::entities[i] == 0)
							break;

						switch (clients[i].message.readByte())
						{
						case MOVE_LEFT:
							if (i == 0 || i == 1)
								Game::entities[i]->vx = -BOARD_SPEED;
							else
								Game::entities[i]->vy = BOARD_SPEED;
							break;
		
						case MOVE_RIGHT:
							if (i == 0 || i == 1)
								Game::entities[i]->vx = BOARD_SPEED;
							else
								Game::entities[i]->vy = -BOARD_SPEED;
							break;

						case NO_MOVE:
							Game::entities[i]->vx = 0;
							Game::entities[i]->vy = 0;
							break;
	
						}
						break;
	
					//default:
						// if wrong message
						//DropClient_Conection(i);
					}
				}
			}
		}
}

void Server::Sound(sound_enum sound_enum, float volume, long freq)
{
	datagramm.writeByte(SV_SOUND);
	datagramm.writeLong(sound_enum);
	datagramm.writeFloat(volume);
	datagramm.writeLong(freq);
}

char game_map[20][20] = {
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 2, 2, 2, 4, 0, 0, 0, 0, 4, 2, 2, 2, 0, 0, 0, 0},
	{0, 0, 0, 0, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 0, 0, 0, 0},
	{0, 0, 0, 0, 2, 1, 0, 0, 0, 5, 5, 0, 0, 0, 1, 2, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 2, 1, 0, 0, 0, 5, 5, 0, 0, 0, 1, 2, 0, 0, 0, 0},
	{0, 0, 0, 0, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 0, 0, 0, 0},
	{0, 0, 0, 0, 2, 2, 2, 3, 0, 0, 0, 0, 3, 2, 2, 2, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
};

void Server::StartGame()
{
	datagramm.buffer.Clear();
	//Game::LoadMap(game_map);
	for (int i = 0; i < 4; i++)
		if (Game::players[i].active)
		{
			Game::SpawnBoard(i);
			//Game::SpawnEntity(Ball(i, (float)i*30, 30, 20, 10, Game::players[i].color));
		}
	Game::paused = false;
	datagramm.writeByte(SV_START_GAME);
	SendDatagrams();
}