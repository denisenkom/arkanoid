#define MAX_CLIENTS	4

namespace Server {

	extern bool active;
	extern Message datagramm;

	void Shutdown();

	// настроечный интерфейс
	void Activate(bool enable);
	void CheckWaitingClients();
	void StartGame();

	// обработка сообщений
	void ParseClientsMessages();
	void SendDatagrams();

	// интерфейс для game
	void Sound(sound_enum sound_enum, float volume, long freq);
	void UpdatePlayersList();
};
