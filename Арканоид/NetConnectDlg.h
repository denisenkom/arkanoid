#include <windows.h>

namespace NetConnectDlg {

extern HWND hwnd, hwndListView, hwndChat;

void UpdatePlayersList();
void Chat(int id, char* str);
void GameStarted();
int Run();

};