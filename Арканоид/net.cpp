#include "StdAfx.h"
#pragma hdrstop

#include "a_shared.h"
#include "NET.h"

static int port = 25000;

typedef struct
{
	short sa_family;
	unsigned char sa_data[14];
} asockaddr;

typedef struct
{
	int size;
	int crc;
	char data[NET_MAXMESSAGE];
} protodata;

Message::Message()
: buffer(NET_MAXMESSAGE)
{
	readpos = 0;
}

void Message::writeByte(char x)
{
	buffer.Write(&x, sizeof(x));
}

void Message::writeStr(const char *str)
{
	buffer.Write(str, strlen(str)+1);
}

void Message::writeLong(long x)
{
	buffer.Write((char*)&x, sizeof(x));
}

void Message::writeFloat(float x)
{
	buffer.Write((char*)&x, sizeof(x));
}

void Message::beginRead()
{
	readpos = buffer.data;
}

char Message::readByte()
{
	if (readpos+sizeof(char) > buffer.data + buffer.cursize)
		throw std::exception("END OF MESSAGE");

	return *readpos++;
}

char* Message::readStr()
{
	char* ret = readpos;
	for (; *readpos; readpos++);
	readpos++;
	if (readpos > buffer.data + buffer.cursize)
		throw std::exception("END OF MESSAGE");
	return ret;
}

long Message::readLong()
{
	if (readpos+sizeof(long) > buffer.data + buffer.cursize)
		throw std::exception("END OF MESSAGE");

	long ret = *(long*)readpos;
	readpos += sizeof(long);
	return ret;
}

float Message::readFloat()
{
	if (readpos+sizeof(float) > buffer.data + buffer.cursize)
		throw std::exception("END OF MESSAGE");

	float ret = *(float*)readpos;
	readpos += sizeof(float);
	return ret;
}

bool Message::eom()
{
	return (readpos >= buffer.data + buffer.cursize);
}

SizeBuffer::SizeBuffer(int startsize)
{
	data = 0;
	maxsize = 0;
	allowOverflow = false;
	overflowed = false;
	Alloc(startsize);
}

SizeBuffer::~SizeBuffer()
{
	Clear();
}

void SizeBuffer::Alloc(int startsize)
{
	if (data) Free();
	data = new char[startsize];
	maxsize = startsize;
}

void SizeBuffer::Free()
{
	if (data) delete data;
}

void SizeBuffer::Clear()
{
	cursize = 0;
}

void SizeBuffer::Write(const char *data, int length)
{
	if (cursize + length > maxsize) {
		if (!allowOverflow)
			throw std::exception("SizeBuffer overflow");
		else
		{
			overflowed = true;
			memcpy(this->data+cursize, data, maxsize - cursize);
			return;
		}
	}
	memcpy(this->data+cursize, data, length);
	cursize += length;
}

/* local variables */

static unsigned int myAddr = 0;
static int listening_socket = 0;
static int datagramm_socket = 0;
static bool initialized = false;
static protodata protocolBuffer;

/* Local routines */

static int OpenSocket() {
	if (!initialized)
		return -1;

	int sock, ret;
	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock == INVALID_SOCKET) return -1;

	unsigned long _true = 1;
	ret = ioctlsocket(sock, FIONBIO, &_true);
	if (ret == SOCKET_ERROR) goto error;

	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons((u_short)port);
	addr.sin_addr.s_addr = myAddr;
	ret = bind(sock, (struct sockaddr*)&addr, sizeof(addr));
	if (ret == SOCKET_ERROR) goto error;

	return sock;
error:
	closesocket(sock);
	return -1;
}

long hash(char* buffer, long size) {
	int rem = size%4;
	long accum = 0;

	long size_even = size >> 2;
	for (int i = 0; i < size_even; i++)
		accum ^= ((long*)buffer)[i];

	if (rem > 0) {
		char* rem_buffer_pos = buffer+size_even*4;
		long rem_buffer = *rem_buffer_pos;
		if (rem > 1)
			rem_buffer += *(rem_buffer_pos+1) << 8;
		if (rem > 2)
			rem_buffer += *(rem_buffer_pos+2) << 16;
		accum ^= rem_buffer;
	}
	return accum;
}

/* Implementation */

/*void ThrowSocketError(int res)
{
	switch (res)
	{
	case WSANOTINITIALISED:
		throw std::exception("WSANOTINITIALISED");
	case WSAENETDOWN:
		throw std::exception("WSAENETDOWN");
	case WSAENOTSOCK:
		throw std::exception("WSAENOTSOCK");
	case WSAEINPROGRESS:
		throw std::exception("WSAEINPROGRESS");
	case WSAEINTR:
		throw std::exception("WSAEINTR");
	case WSAEWOULDBLOCK:
		throw std::exception("WSAEWOULDBLOCK");
	}
}*/

void Network::Init()
{
	WSAData wsadata;
	int ret;

	ret = WSAStartup(MAKEWORD(2, 0), &wsadata);
	if (ret == SOCKET_ERROR)
	{
		int err = WSAGetLastError();
		if (err == WSAVERNOTSUPPORTED)
		{
			ret = WSAStartup(MAKEWORD(1, 1), &wsadata);
			if (ret == SOCKET_ERROR)
				throw std::exception("NET: Windows sockets v1.1 not available.");
		}
		else
			throw std::exception("NET: Windows sockets v2.0 not available.");
	}

	datagramm_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (datagramm_socket == INVALID_SOCKET)
		throw std::exception("NET: Cannot create datagramm socket.");

	BOOL _true = TRUE;
	ret = setsockopt(datagramm_socket, SOL_SOCKET, SO_BROADCAST, (char*)&_true, sizeof(_true));
	if (ret == SOCKET_ERROR)
		throw std::exception("NET: Cannot setup datagramm socket.");

	initialized = true;
}

void Network::Shutdown()
{
	initialized = false;

	if (listening_socket)
		closesocket(listening_socket);

	WSACleanup();
}

void Network::Listen(bool enable)
{
	if (!initialized)
		Init();

	if (enable)
	{
		if (!listening_socket)
			listening_socket = OpenSocket();
		if (listening_socket == INVALID_SOCKET)
		{
			listening_socket = 0;
			throw std::exception("Cannot create listening socket");
		}
		int ret = listen(listening_socket, SOMAXCONN);
		if (ret == SOCKET_ERROR)
			throw std::exception("Listen socket failed.");
	}
	else
	{
		closesocket(listening_socket);
		listening_socket = 0;
	}
}

int Network::Accept() {
	if (!initialized)
		Init();

	if (listening_socket == 0)
		return-1;

	int ret = accept(listening_socket, NULL, NULL);
	if (ret == INVALID_SOCKET)
		return -1;	

	return ret;
}

int Network::Connect(const char* host)
{
	if (!initialized)
		Init();

	hostent* phostent;
	phostent = gethostbyname(host);
	if (phostent == NULL) return -1;

	sockaddr_in address;
	address.sin_family = AF_INET;
	address.sin_port = htons((unsigned short)port);
	address.sin_addr.s_addr = *(int*)phostent->h_addr_list[0];

	int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock == INVALID_SOCKET) return -1;

	int ret = connect(sock, (struct sockaddr*)&address, sizeof(address));
	if (ret == SOCKET_ERROR) return -1;

	unsigned long _true = 1;
	ret = ioctlsocket(sock, FIONBIO, &_true);
	if (ret == SOCKET_ERROR) goto error;

	return sock;

error:
	closesocket(sock);
	return -1;
}

void Network::CloseSocket(int sock)
{
	if (!initialized)
		Init();

	int res = closesocket(sock);
	if (res == 0 || res == WSAENOTSOCK)
		return;
}

int Network::ReadMessage(Message& message, int sock) {
	if (!initialized)
		return -1;

	int ret;
	ret = recv(sock, (char*)&protocolBuffer.size, sizeof(protocolBuffer.size)+sizeof(protocolBuffer.crc), 0);
	if (ret == SOCKET_ERROR) {
		int retcode;

		retcode = WSAGetLastError();
		if (retcode == WSAEWOULDBLOCK) return 0;

		return -1;
	}
	if (ret != sizeof(protocolBuffer.size)+sizeof(protocolBuffer.crc))
		return -1;

	if (protocolBuffer.size > NET_MAXMESSAGE)
		throw std::exception("Network read message error message is too long.");

	ret = recv(sock, protocolBuffer.data, protocolBuffer.size, 0);
	if (ret != protocolBuffer.size) {
		return -1;
	}

	if (protocolBuffer.crc != hash(protocolBuffer.data, protocolBuffer.size))
		throw std::exception("Network received corrupt message: wrong crc.");

	//if (ret != protocolBuffer.size + (int)sizeof(protocolBuffer.size)) {
	//	return -1;
	//}

	message.buffer.cursize = ret;
	memcpy(message.buffer.data, protocolBuffer.data, protocolBuffer.size);

	return ret;
}

int Network::WriteMessage(Message& message, int sock) {
	if (!initialized)
		return -1;

	if (message.buffer.cursize == 0)
		return 0;

	int ret;

	protocolBuffer.size = message.buffer.cursize;
	if (protocolBuffer.size > NET_MAXMESSAGE)
		throw std::exception("Network send error: message too long.");
	
	memcpy(protocolBuffer.data, message.buffer.data, message.buffer.cursize);
	protocolBuffer.crc = hash(protocolBuffer.data, protocolBuffer.size);
	ret = send(sock, (char*)&protocolBuffer, message.buffer.cursize + sizeof(protocolBuffer.size) + sizeof(protocolBuffer.crc), 0);
	if (ret == SOCKET_ERROR) {
		return -1;
	}
	return ret;
}

/*int Network::SendDatagramm(Message& message) {
	if (!initialized)
		return -1;

	if (message.buffer.cursize == 0)
		return 0;

	int ret;
	sockaddr_in addr;
	addr.sin_port = Host::port;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_BROADCAST;

	protocolBuffer.size = message.buffer.cursize;
	memcpy(protocolBuffer.data, message.buffer.data, message.buffer.cursize);
	ret = sendto(datagramm_socket, (char*)&protocolBuffer, message.buffer.cursize + sizeof(protocolBuffer.size), 0, (sockaddr*)&addr, sizeof(addr));
	if (ret == SOCKET_ERROR) {
		return -1;
	}
	return ret;
}*/