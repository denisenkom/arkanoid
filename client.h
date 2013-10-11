
struct remote_ent {
	float x, y;
	int visible;
	sprites_enum sprite;
};

namespace Client {

	extern bool connected;
	extern char name[256];
	extern remote_ent entities[MAX_ENTITIES];
	extern player players[MAX_PLAYERS];
	extern Message message;
	extern int sock;
	extern long LastFrame;

	// настроечный интерфейс
	void Connect(const char* server_address);
	void Disconnect() throw ();

	void BeginCollectingMessages();
	void SendClientInfo();
	void WriteClientMovements();
	void FlushMessages();

	// обработка сообщений
	void ParseServerMessage();

	// chat
	void Say(const char* mes);

};
