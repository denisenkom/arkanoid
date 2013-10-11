class WinsockDriver {
public:
	static unsigned int myAddr;
	static char host_name[];
	static int host_port;

	static void Init();
	static void Release();

	virtual int OpenSocket(int port);
	virtual void CloseSocket(int socket);

	virtual int Write(int sock, char* buf, int buflen, asockaddr addr);
	virtual int Read(int sock, char* buf, int buflen, asockaddr addr);

	virtual int CheckNewConnection();
};
