#include "net_wins.h"
#include "stdafx.h"

void WinsockDriver::Init() {
	WSADATA wsadata;
	int ret;
	hostent* phostent;

	if (WSAStartup(MAKEWORD(1,1), &wsadata) == -1)
		throw Exception("NET::Init() WSAStartup failed\n");

	ret = gethostname(host_name, sizeof(host_name));
	if (ret == SOCKET_ERROR)
		throw Exception("NET::Init() cannot get host name");


	phostent = gethostbyname(host_name);
	if (phostent != NULL)
		myAddr = *(int*)phostent->h_addr_list[0];
	else
		myAddr = INADDR_ANY;
}

void WinsockDriver::Release() {
	WSACleanup();
}

WINSocket* WinsockDriver::Connect() {
	return NULL;
}

WINSocket* WinsockDriver::CheckNewConnection() {
	return NULL;
}

void WinsockDriver::SendMessage(WINSocket* sock, const NetMessage& mes) {
	u_int pack_len = sizeof(mes);
	u_int len = sock->send((char*)&mes, pack_len);
	if (pack_len != len)
		throw Exception("Данные переданы неправиьлно.");
}

void WinsockDriver::GetMessage(WINSocket* sock, NetMessage& mes) {
	u_int pack_len = sizeof(mes);
	u_int received = sock->recv((char*)&mes, pack_len);
	if (pack_len > received) {
		received += sock->recv((char*)&mes + received, pack_len-received);
		if (pack_len != received)
			throw Exception("Данные приняты неправиьлно.");
	}
	else if (pack_len < received)
		throw Exception("Данные приняты неправиьлно.");
}

int WinsockDriver::OpenSocket(int port) {
	int sock;

	sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (sock == INVALID_SOCKET) {
		throw Exception("NET::OpenSocket() cannot open socket");
	}

	u_long _true = 1;
	int ret;
	ret = ioctlsocket(sock, FIONBIO, &_true);
	if (ret == SOCKET_ERROR) {
		closesocket(sock);
		throw Exception("NET::OpenSocket() cannot set nonblocking mode");
	}

	sockaddr_in address;
	address.sin_family = AF_INET;
	address.sin_port = htons((u_short)port);
	address.sin_addr.s_addr = myAddr;
	ret = bind(sock, (struct sockaddr*)&address, sizeof(address));
	if (ret == SOCKET_ERROR) {
		closesocket(sock);
		throw Exception("NET::OpenSocket() cannot bind socket");
	}

	return sock;
}

void WinsockDriver::CloseSocket(int socket) {
	int ret;
	ret = closesocket(socket);
	if (ret == SOCKET_ERROR) throw Exception("NET::CloseSocket() error");
}

int WinsockDriver::Write(int sock, char* buf, int buflen, asockaddr addr) {
	int ret;
	ret = sendto(sock, buf, buflen, 0, (struct sockaddr*)&addr, sizeof(addr));
	if (ret == SOCKET_ERROR)
		throw Exception("NET::Write() failed");
	return ret;
}

int WinsockDriver::Read(int sock, char* buf, int buflen, asockaddr addr) {
	int ret, fromlen;
	fromlen = sizeof(addr);
	ret = recvfrom(sock, buf, buflen, 0, (struct sockaddr*)&addr, &fromlen);
	if (ret == SOCKET_ERROR) {
		int errcode = WSAGetLastError();
		if (errcode == WSAEWOULDBLOCK || errcode == WSAECONNREFUSED)
			return 0;
		throw Exception("NET::Read() failed");
	}
	return ret;
}
