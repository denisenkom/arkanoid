
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
	extern sock;

	// настроечный интерфейс
	void Connect(const char* server_address);
	void SendClientInfo();
	void Disconnect() throw ();

	// обработка сообщений
	void ParseServerMessages();

	// chat
	void Say(const char* mes);

};