#define MAX_CLIENTS	4

namespace Server {

	extern bool active;
	extern Message datagramm;

	void Shutdown();

	// ����������� ���������
	void Activate(bool enable);
	void CheckWaitingClients();
	void StartGame();

	// ��������� ���������
	void ParseClientsMessages();
	void SendDatagrams();

	// ��������� ��� game
	void Sound(sound_enum sound_enum, float volume, long freq);
	void UpdatePlayersList();
};
