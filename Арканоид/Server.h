#define MAX_CLIENTS	4

namespace Server {

	extern bool active;
	extern Message datagramm;
	extern long FrameNo;

	void Shutdown();

	// ����������� ���������
	void Activate(bool enable);
	void CheckWaitingClients();
	void StartGame();

	// ��������� ���������
	void ParseClientsMessages();
	void FlushMessages();

	// ��������� ��� game
	void Sound(sound_enum sound_enum, float volume, long freq);
	void UpdatePlayersList();
};
