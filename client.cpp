#include "StdAfx.h"
#pragma hdrstop

#include "a_shared.h"
#include "Screen.h"
#include "NET.h"
#include "Client.h"
#include "NetConnectDlg.h"
#include "Sound.h"
#include "Controls.h"
#include "Realtime.h"
#include "log.h"

/* Local variables */
static int player_id = 0;

/* Implementation */
int Client::sock = 0;
Message Client::message;
bool Client::connected = false;
char Client::name[256] = "mikhail";
remote_ent Client::entities[MAX_ENTITIES] = {0};
player Client::players[MAX_PLAYERS] = {0};

void Client::ParseServerMessages()
{
	if (!Client::connected)
		return;

	while (1) {
		int ret = Network::ReadMessage(message, sock);

		if (ret == -1)
			throw std::exception("Connection has broken.");

		if (ret == 0)
			return;

		message.beginRead();
		while (!message.eom())
		{
			sv_cmd cmd = (sv_cmd)message.readByte();
			switch (cmd)
			{
			case SV_SEND_INFO:
				player_id = message.readByte();
			break;

			case SV_UPDATE_PLAYERS_LIST: {
				for (int i = 0; i < 4; i++) {
					Client::players[i].active = message.readByte();
					Client::players[i].color = (color_enum)message.readByte();
					strcpy(Client::players[i].name, message.readStr());
					Client::players[i].pts = message.readLong();
				}
				NetConnectDlg::UpdatePlayersList();
			}
			break;

			case SV_SYNC_ENT: {
				int id = message.readLong();
				int mask = message.readByte();
				if (mask & UPDATE_POS) {
					Client::entities[id].x = message.readFloat();
					Client::entities[id].y = message.readFloat();
				}
				if (mask & UPDATE_VIS) {
					Client::entities[id].visible = message.readByte();
				}
				if (mask & UPDATE_SPR) {
					Client::entities[id].sprite = (sprites_enum)message.readLong();
				}
			}
			break;

			case SV_SOUND: {
				sound_enum snd = static_cast<sound_enum>(message.readLong());
				float volume = message.readFloat();
				long freq = message.readLong();
				Sound::PlaySnd(snd, volume, freq);
			}
			break;

			case SV_PRINT: {
				int pid = message.readByte();
				char* mes = message.readStr();
				NetConnectDlg::Chat(pid, mes);
			}
			break;

			case SV_START_GAME:
				NetConnectDlg::GameStarted();
				break;

			default:
				throw std::exception("Invalid server message.");
			}
		}
	}
}

/*void Client::Frame()
{
	if (!Client::connected)
		return;

	//double time1 = RealTime::getTime();
	ParseServerMessages();
	//double time2 = RealTime::getTime();
	message.buffer.Clear();
	message.writeByte(CL_MOVE);
	if (Controls::keys[KEY_LEFT] || Controls::keys[KEY_UP])
		message.writeByte(MOVE_LEFT);
	else if (Controls::keys[KEY_RIGHT] || Controls::keys[KEY_DOWN])
		message.writeByte(MOVE_RIGHT);
	else
		message.writeByte(NO_MOVE);
	Network::WriteMessage(message, sock);
	//double time3 = RealTime::getTime();
	Screen::Update();
	//double time4 = RealTime::getTime();
	//Log_Print("parse %f : send %f : screen %f\n", time2-time1, time3-time2, time4-time3);
}*/

void Client::Connect(const char* host)
{
	if (connected)
		Disconnect();

	sock = Network::Connect(host);
	if (sock == -1)
		throw std::exception("Cannot connect to server\n");

	message.buffer.Clear();
	SendClientInfo();
	Network::WriteMessage(message, sock);

	connected = true;
}

void Client::Disconnect() throw ()
{
	message.writeByte(CL_DROP);
	Network::WriteMessage(message, sock);
	Network::CloseSocket(sock);
}

void Client::Say(const char *mes)
{
	message.buffer.Clear();
	message.writeByte(CL_PRINT);
	message.writeStr(mes);
	Network::WriteMessage(message, sock);
}

void Client::BeginCollectingMessages() {
	Client::message.buffer.Clear();
}

void Client::SendClientInfo()
{
	message.writeByte(CL_INFO);
	message.writeStr(name);
}

void Client::WriteClientMovements() {
	message.writeByte(CL_MOVE);
	if (Controls::keys[KEY_LEFT] || Controls::keys[KEY_UP])
		message.writeByte(MOVE_LEFT);
	else if (Controls::keys[KEY_RIGHT] || Controls::keys[KEY_DOWN])
		message.writeByte(MOVE_RIGHT);
	else
		message.writeByte(NO_MOVE);
}

void Client::FlushMessages() {
	Network::WriteMessage(message, sock);
}