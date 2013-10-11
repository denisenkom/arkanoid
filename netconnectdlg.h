#include <windows.h>

namespace NetConnectDlg {

extern HWND hwnd, hwndListView, hwndChat;

void UpdatePlayersList() throw();
void Chat(int id, char* str) throw();
void GameStarted() throw();
int Run();	// IDCANCEL/IDOK

};
