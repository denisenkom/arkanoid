#include "stdafx.h"
#pragma hdrstop

using namespace std;

#include "winerrors.h"

#include "a_shared.h"
#include "winarkanoid.h"
#include "server.h"
#include "client.h"
#include "entities.h"
#include "game.h"
#include "netconnectdlg.h"
#include "arkanoid.h"
#include "graphics.h"
#include "resource.h"
#include "log.h"
#include "winexception.h"

HWND NetConnectDlg::hwnd = NULL;
HWND NetConnectDlg::hwndListView = NULL;
HWND NetConnectDlg::hwndChat = NULL;

volatile BOOL FinishNetworkThread = FALSE;
HANDLE hNetworkThread = NULL;

DWORD WINAPI NetworkThread(LPVOID) throw ()
{
	while (!FinishNetworkThread)
	{
		if (Server::active)
		{
			Server::datagramm.buffer.Clear();
			Server::CheckWaitingClients();
			Server::ParseClientsMessages();
			Server::FlushMessages();
		}
		if (Client::connected)
			Client::ParseServerMessages();
		::Sleep(100);
	}
	return 0;
}

BOOL OnCommand(WORD wNotifyCode, WORD wID, HWND hwndCtl) throw ()
{
	if (wNotifyCode == 0 || wNotifyCode == 1)	// from menu or accelerators
	{
		switch (wID)
		{
		case IDOK:
			if (!Server::active)
				return 1;

			Server::StartGame();
			return 1;

		case IDCANCEL:
			FinishNetworkThread = TRUE;
			EndDialog(NetConnectDlg::hwnd, IDCANCEL);
			return 1;

		case IDC_RUNSERVER:
			Client::Disconnect();
			Server::Activate(false);
			try
			{
				Server::Activate(true);
				try
				{
					Client::Connect("localhost");
				}
				catch (std::exception &ex)
				{
					Client::Disconnect();
					throw ex;
				}
			}
			catch (std::exception &ex)
			{
				Server::Activate(false);
				::MessageBoxA(0, (std::string("Ошибка при запуске сервера: ") += ex.what()).c_str(), NULL, MB_ICONSTOP);
			}
			return 1;

		case IDC_ATTACH:
			if (Server::active)
				Server::Activate(false);
			if (Client::connected)
				Client::Disconnect();
			char address[256];
			::GetWindowTextA( GetDlgItem(NetConnectDlg::hwnd, IDC_ADDRESS), address, sizeof(address) );
			try
			{
				Client::Connect(address);
				::MessageBoxA(NetConnectDlg::hwnd, "Подключение успешно", "Арканоид", MB_ICONINFORMATION);
			}
			catch (std::exception& ex)
			{
				::MessageBoxA(NetConnectDlg::hwnd, ex.what(), "Ошибка", MB_ICONEXCLAMATION);
			}
			return 1;

		case IDC_BROWSE:
			return 1;

		case IDC_SEND:
			if (!Client::connected)
				return 0;

			char message[256];
			::GetWindowText( GetDlgItem(NetConnectDlg::hwnd, IDC_CHATEDIT), message, sizeof(message) );
			Client::Say(message);
			return 1;

		default:
			return 0;
		}
	}
	else
	{
		if (wNotifyCode == EN_CHANGE && wID == IDC_NAME)
		{
			char name[256];
			::GetWindowText( hwndCtl, name, sizeof(name) );
			strcpy(Client::name, name);
			Client::SendClientInfo();
		}
		return 0;
	}
}

INT_PTR CALLBACK NetConnectDlgProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) throw ()
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
		NetConnectDlg::hwnd = hwndDlg;
		DWORD thread_id;
		hNetworkThread = ::CreateThread(NULL, 0, &NetworkThread, NULL, 0, &thread_id);
		NetConnectDlg::hwndListView = GetDlgItem(hwndDlg, IDC_LISTVIEW);
		NetConnectDlg::hwndChat = GetDlgItem(hwndDlg, IDC_CHAT);
		int i;
		for (i = 0; i < 4; i++)
			::SendMessage(NetConnectDlg::hwndListView, LB_INSERTSTRING, -1, (LPARAM)(LPCTSTR)"<пусто>");
		SetWindowText( GetDlgItem(hwndDlg, IDC_NAME), Client::name );
		SetWindowText( GetDlgItem(hwndDlg, IDC_ADDRESS), "localhost" );
		return 0;
	
	case WM_COMMAND:
		return OnCommand(HIWORD(wParam), LOWORD(wParam), (HWND)lParam);

	default:
		return 0;
	}
}

void NetConnectDlg::UpdatePlayersList() throw ()
{
	for (int i = 0; i < 4; i++)
	{
		SendMessage(NetConnectDlg::hwndListView, LB_DELETESTRING, i, 0);
		SendMessage(NetConnectDlg::hwndListView, LB_INSERTSTRING, i, (LPARAM)(LPCTSTR)(Client::players[i].name[0] != 0 ? Client::players[i].name : "<пусто>"));
	}
}

void NetConnectDlg::Chat(int id, char* str) throw ()
{
	char string[256];
	sprintf(string, "%s: %s", Client::players[id].name, str);
	SendMessage( hwndChat, LB_INSERTSTRING, -1, (LPARAM)(LPCTSTR)string );
}

void NetConnectDlg::GameStarted() throw ()
{
	FinishNetworkThread = TRUE;
	EndDialog(NetConnectDlg::hwnd, IDOK);
}

int NetConnectDlg::Run() {
	Log_Print("Starting connection dialog...\n");
	INT_PTR res = DialogBoxParam( g_hInstance, MAKEINTRESOURCE(IDD_NETWORK),
		NULL, &NetConnectDlgProc, 0l );
	if(res <= 0)
		throw runtime_error(
			( "Ошибка при создании диалога установки соединения: " +
			winerror() ).c_str() );
	::WaitForSingleObject(hNetworkThread, 1000);
	return res;
}
