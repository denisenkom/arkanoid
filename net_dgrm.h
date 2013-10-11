#include "net.h"

class DatagrammDriver : NetworkDriver {
public:
	virtual int Connect(char* host);
	virtual void SendMessage(int sock, const SizeBuffer& mes);
	virtual void GetMessage(int sock, NetMessage& mes);
};
