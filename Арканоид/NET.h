namespace Network {

	void Init();
	void Shutdown();

	void CloseSocket(int sock);

	void Listen(bool enable);
	int Accept();
	int Connect(const char* host);

	int ReadMessage(Message& message, int sock);
	int WriteMessage(Message& message, int sock);
	//int SendDatagramm(Message& message);

};